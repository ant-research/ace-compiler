//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief vendor_traits.h
//! VendorTraits specialization for the Phantom GPU backend.
//!
//! Maps the common FHE framework (KeySerializer<T>, EncIo<T>,
//! KeyMgr<T>, etc.) to Phantom's vendor types and memory layouts.
//! This is the ONLY point where the framework touches Phantom internals.

#ifndef PHANTOM_VENDOR_TRAITS_H
#define PHANTOM_VENDOR_TRAITS_H

#include <cstdint>
#include <vector>

// Phantom vendor headers — these define the key, plaintext, ciphertext types
#include "fhe/enc/ciphertext_io.h"
#include "fhe/enc/enc_header.h"
#include "fhe/io/io_policy.h"
#include "fhe/vendor/phantom/bts_mgr.h"
#include "fhe/vendor/types.h"
#include "phantom.h"

// NOLINTBEGIN (readability-identifier-naming)

//! Global rt-API type aliases for the Phantom backend.
//!
//! These bare names (CIPHERTEXT/CIPHER/PLAIN/...) are used by the codegen
//! dispatch header (rt_phantom.h) and the codegen-generated kernel .cu. They
//! used to live in a separate rt_def.h; folded here so vendor_traits.h is the
//! single point where Phantom vendor types meet the framework surface.
namespace phantom
{
}  // namespace phantom

typedef PhantomCiphertext CIPHERTEXT;
typedef PhantomCiphertext CIPHERTEXT3;
typedef PhantomCiphertext* CIPHER;
typedef PhantomCiphertext* CIPHER3;
typedef PhantomPlaintext PLAINTEXT;
typedef PhantomPlaintext* PLAIN;

#define CIPHER_DEFINED 1
#define PLAIN_DEFINED 1

// NOLINTEND (readability-identifier-naming)

namespace phantom
{

//=============================================================================
// Key serialization data containers
//=============================================================================
// Defined at namespace scope (not nested in VendorTraits) — plain data
// carriers for raw GPU buffer pointers + metadata. Each key type has a
// distinct field layout (Phantom exposes per-key raw memory shapes), so each
// keeps its own struct; VendorTraits re-exports them under the per-key alias
// names the concept + KeySerializer expect.

struct SecretKeyAccess
{
    const uint64_t* data_rns_ptr;
    const uint64_t* sk_array_ptr;
    uint64_t sk_max_power;
};
struct PublicKeyAccess
{
    const uint64_t* pk_data_ptr;
    uint64_t pk_size;
    uint64_t chain_index;
    double scale;
    uint64_t poly_modulus_degree;  // N (for header construction)
    uint64_t coeff_modulus_size;   // L (for header construction)
};
struct RelinKeyAccess
{
    std::vector<const uint64_t*> key_data_ptrs;
    std::array<uint64_t, 4> parms_id;
};
struct GaloisKeyAccess
{
    std::vector<std::vector<const uint64_t*>> relin_key_ptrs;
    std::vector<uint32_t> galois_elts;
    std::array<uint64_t, 4> parms_id;
};
struct SecretKeyPopulate
{
    uint64_t* data_rns;
    uint64_t* sk_array;
    uint64_t sk_max_pow;
};
struct PublicKeyPopulate
{
    uint64_t* pk_data;
    uint64_t pk_size;
    uint64_t chain_index;
    double scale;
};
struct RelinKeyPopulate
{
    std::vector<uint64_t*> key_data;
    uint64_t num_keys;
    std::array<uint64_t, 4> parms_id;
};
struct GaloisKeyPopulate
{
    std::vector<std::vector<uint64_t*>> relin_key_data;
    std::vector<uint32_t> galois_elts;
    std::array<uint64_t, 4> parms_id;
};

//=============================================================================
// Plaintext serialization data containers
//=============================================================================
// Extend the common PtAccessBase / PtPopulateBase with Phantom's chain_index
// (integer encoding level). Phantom plaintexts are always NTT form, so no
// is_ntt_form flag. Defined at namespace scope; VendorTraits re-exports them.

struct PtAccess : fhert::PtAccessBase
{
    uint64_t chain_index = 0;  // Encoding level (integer index)
};
struct PtPopulate : fhert::PtPopulateBase
{
    uint64_t chain_index = 0;
};

//=============================================================================
// Ciphertext serialization data containers
//=============================================================================
// CtAccess / CtPopulate carry ciphertext-specific fields beyond PtAccessBase:
// size (polys, usually 2), chain_index (level), is_ntt_form, is_asymmetric,
// noise_scale_deg. P1: is_asymmetric / noise_scale_deg are read in
// access_ciphertext but NOT restored in populate_ciphertext (no public
// setters on PhantomCiphertext; CKKS eval does not read them). parms_id is
// not carried (eval routes by chain_index; both ends share the same context).

struct CtAccess
{
    const void* gpu_data = nullptr;
    uint64_t poly_modulus_degree = 0;
    uint64_t coeff_modulus_size = 0;
    uint64_t size = 0;  // number of polys
    uint64_t chain_index = 0;
    double scale = 0.0;
    size_t data_size = 0;  // size * N * L * sizeof(uint64_t)
    bool is_ntt_form = true;
    bool is_asymmetric = false;
    uint32_t noise_scale_deg = 0;
};
struct CtPopulate
{
    const void* host_data = nullptr;
    size_t data_size = 0;
    uint64_t poly_modulus_degree = 0;
    uint64_t coeff_modulus_size = 0;
    uint64_t size = 0;
    uint64_t chain_index = 0;
    double scale = 0.0;
    bool is_ntt_form = true;
    bool is_asymmetric = false;
    uint32_t noise_scale_deg = 0;
};

//=============================================================================
// VendorTraits
//=============================================================================

struct VendorTraits
{
    //=========================================================================
    // Type aliases
    //=========================================================================

    using SecretKey = PhantomSecretKey;
    using PublicKey = PhantomPublicKey;
    using RelinKey = PhantomRelinKey;
    using GaloisKey = PhantomGaloisKey;
    using Context = PhantomContext;
    using Plaintext = PhantomPlaintext;
    using Ciphertext = PhantomCiphertext;
    using ScaleType = double;  // SCALE_T
    using LevelType = size_t;  // LEVEL_T

    //=========================================================================
    // Evaluator — uses common template directly
    //=========================================================================
    // Phantom's Evaluator is a simple encoder + evaluator pair with no
    // BTS-specific extensions. CKKSEvaluator wraps encoder + decryptor +
    // evaluator + relin/galois keys internally.

    using Evaluator = fhert::Evaluator<PhantomCKKSEncoder, CKKSEvaluator>;

    //=========================================================================
    // Per-session CUDA stream (Mode B-P) — phantom backend
    //=========================================================================
    // Each session gets its own cuda_stream_wrapper so concurrent sessions run
    // HE ops on independent CUDA streams. fhe_lib sets phantom's thread_local
    // tl_current_stream to the active session's stream on each worker thread.
    using SessionStream = phantom::util::cuda_stream_wrapper*;  // non-owning (ownership in FheContext)
    using SessionStreamOwner = std::unique_ptr<phantom::util::cuda_stream_wrapper>;

    //=========================================================================
    // Thread-local FHE-context state (Mode B-P multi-thread safety)
    //=========================================================================
    // CRITICAL: these must NOT be `static inline thread_local` in a shared
    // template header — the compiled FHE kernel .so inlines reads of them and,
    // on a non-main worker pthread, each DSO resolves to its OWN TLS instance
    // (nullptr/0), crashing. Instead the TLS lives in a single TU (vendor_traits.cu,
    // `static thread_local`) and is accessed ONLY via these exported functions —
    // same pattern as ace::VendorTraits::get/set_device_context below, and as the
    // phantom stream override (get_thread_stream/set_thread_stream).
    // The void* avoids pulling FheContext<> into the backend header; FheContext
    // casts to/from FheContext<Backend>*.
    static void* current_context_ptr();
    static void set_current_context_ptr(void* p);
    static void* acquire_current_context_ptr();  // returns and clears (for ClearCurrent)
    static int active_session_idx();
    static void set_active_session_idx(int idx);

    /// The active session's CUDA stream (thread-local current_stream after
    /// Activate_session). Lets populate/diagnostic paths allocate + copy ct data
    /// on the SAME stream the encrypt path uses (so eval reads it stream-ordered,
    /// no cross-stream visibility gap). Default-stream allocation was the suspected
    /// cause of the run_cipher eval divergence (8.7).
    static cudaStream_t current_stream_value();

    //=========================================================================
    // Crypto — per-session encrypt/decrypt (common template)
    //=========================================================================

    /// Uses the common Crypto template with Phantom vendor types.
    /// Populated during init or switch_session; null in compute-only mode.
    using Crypto = fhert::Crypto<Encryptor, Decryptor>;

    //=========================================================================
    // BtsMgr
    //=========================================================================

    using BtsMgr = phantom::BtsMgr;

    //=========================================================================
    // GaloisTool (Mode B-P per-session tool copy)
    //=========================================================================
    // Each session owns its own PhantomGaloisTool copy (built on the session
    // stream, ~18 MiB), routed at runtime via thread-local override (see
    // phantom globals.h set_thread_galois_tool / evaluate.cuh
    // current_galois_tool). Lets apply_galois/rotate/conjugate read a
    // stream-ordered tool WITHOUT regenerating the 22 GiB galois key set.
    using GaloisTool = ::PhantomGaloisTool;

    //=========================================================================
    // Key accessor — extract raw GPU pointers for serialization
    //=========================================================================

    // Key Access / Populate struct definitions live at namespace scope (see
    // above); here we only re-export the per-key alias names the concept +
    // KeySerializer expect.
    using SecretKeyAccess = phantom::SecretKeyAccess;
    using PublicKeyAccess = phantom::PublicKeyAccess;
    using RelinKeyAccess = phantom::RelinKeyAccess;
    using GaloisKeyAccess = phantom::GaloisKeyAccess;

    static SecretKeyAccess access_secret_key(const SecretKey& sk, const Context& ctx);
    static PublicKeyAccess access_public_key(const PublicKey& pk);
    static RelinKeyAccess access_relin_key(const RelinKey& rlk);
    static GaloisKeyAccess access_galois_key(const GaloisKey& rtk);

    //=========================================================================
    // Key populate — fill a key object from deserialized raw data
    //=========================================================================

    using SecretKeyPopulate = phantom::SecretKeyPopulate;
    using PublicKeyPopulate = phantom::PublicKeyPopulate;
    using RelinKeyPopulate = phantom::RelinKeyPopulate;
    using GaloisKeyPopulate = phantom::GaloisKeyPopulate;

    static void populate_secret_key(SecretKey& sk, const SecretKeyPopulate& data, uint64_t N, uint64_t L, Context& ctx);
    static void populate_public_key(PublicKey& pk, const PublicKeyPopulate& data, uint64_t N, uint64_t L, Context& ctx);
    static void populate_relin_key(RelinKey& rlk, const RelinKeyPopulate& data, uint64_t N, uint64_t L, Context& ctx);
    static void populate_galois_key(GaloisKey& rtk, const GaloisKeyPopulate& data, uint64_t N, uint64_t L,
                                    Context& ctx);

    /// Populate ONE relin-key slot of an (on-demand) galois key at index i,
    /// from raw component device pointers already read from disk. Unlike
    /// populate_galois_key (which appends in order), this assigns slot i
    /// directly so Ensure_galois_key can fill slots out of order. The galois
    /// key object must already have relin_keys_ sized to >= i+1 (empty slots).
    /// components: nk device pointers, each holding 2*N*L uint64_t. parms_id
    /// is copied onto the slot (not read by the keyswitch path, but set for
    /// parity with the full-load populate).
    static void populate_one_relin_key(GaloisKey& rtk, size_t i, const std::vector<uint64_t*>& components,
                                       const std::array<uint64_t, 4>& parms_id, uint64_t N, uint64_t L);

    /// Pre-size the galois key's relin-key vector to n empty slots (for the
    /// ON_DEMAND empty-shell init), so populate_one_relin_key can assign into
    /// any slot out of order. Sets gen_flag_/parms_id_ on the galois key
    /// (not read by the keyswitch path, set for parity).
    static void prealloc_galois_slots(GaloisKey& rtk, size_t n, const std::array<uint64_t, 4>& parms_id);

    /// Evict (free) the key at slot i of the galois key: move-assign an empty
    /// PhantomRelinKey into relin_keys_[i], so the previous key's cudaAutoPtr
    /// destructs and cudaFreeAsync's its HBM. Vendor evaluates get_relin_keys(i)
    /// inline each call (no cached raw ptr), so an emptied slot is safe until
    /// populate_one_relin_key refills it. friend access (same as populate).
    static void clear_galois_slot(GaloisKey& rtk, size_t i);

    /// Map a rotation step to its galois element using the context's galois
    /// tool (must have been rebuilt via rebuild_galois_tool first). Pure
    /// function of N — independent of which per-session tool the evaluator
    /// uses at runtime, so ctx.key_galois_tool_ is safe to query here.
    static uint32_t galois_elt_from_step(const Context& ctx, int step);

    /// Decompose a rotation step into the galois elements its key-switch
    /// actually dereferences. Phantom serves large/odd rotation steps via NAF
    /// decomposition (evaluate.cu ~826): step -> naf(step) = sum ±2^i, each
    /// component temp_step = ±2^i rotated recursively, consuming the galois
    /// key for elt = get_elt_from_step(temp_step) (= 5^(±2^i)). The full step's
    /// own galois_elt_from_step(step) is NOT directly loaded; only these NAF
    /// component elts are. This returns exactly that set so ON_DEMAND can
    /// prefetch only the keys the vendor will use (typically log(|step|) elts),
    /// not the entire galois set. Empty if step's own elt IS directly loaded.
    static std::vector<uint32_t> naf_component_elts(const Context& ctx, int step);

    //=========================================================================
    // Context accessors — used by FheContext, KeyMgr, KeySerializer
    //=========================================================================

    /// Get polynomial degree from context
    static uint64_t poly_degree(const Context& ctx);

    /// Get coefficient modulus size from context
    static uint64_t coeff_mod_size(const Context& ctx);

    /// Get galois elements from the context's galois tool.
    /// Returns empty vector if tool is not initialized.
    static std::vector<uint32_t> get_galois_elts(const Context& ctx);

    /// Rebuild the galois tool in the context (used after KMS key load).
    static void rebuild_galois_tool(Context& ctx, const std::vector<uint32_t>& elts);

    /// Check if a key object has been generated (has valid data).
    static bool is_key_generated(const SecretKey& sk);
    static bool is_key_generated(const PublicKey& pk);
    static bool is_key_generated(const RelinKey& rlk);
    static bool is_key_generated(const GaloisKey& rtk);

    //=========================================================================
    // Plaintext accessor / populate (extends common base)
    //=========================================================================
    // Phantom extends the common base with chain_index (integer level).
    // Phantom plaintexts are always in NTT form, so no is_ntt_form flag.

    // PtAccess / PtPopulate defined at namespace scope (see above); re-export
    // the alias names the common EncIo / KeySerializer templates expect.
    using PtAccess = phantom::PtAccess;
    using PtPopulate = phantom::PtPopulate;
    static PtAccess access_plaintext(const Plaintext& pt);

    static void populate_plaintext(Plaintext& pt, const PtPopulate& data, Context* ctx = nullptr,
                                   void* stream = nullptr);

    //=========================================================================
    // EncIo vendor hooks — used by the common EncIo<Traits> implementation
    //=========================================================================

    /// Derive the encoding level (chain_index) from a plaintext access struct.
    /// Phantom stores chain_index directly on the access struct.
    static uint64_t chain_index_from_access(const PtAccess& access) { return access.chain_index; }

    /// Derive the EncHeader reserved2 field from a plaintext access struct.
    /// Phantom plaintexts are always in NTT form; reserved2 is unused.
    static uint32_t pt_reserved2_from_access(const PtAccess& /*access*/) { return 0u; }

    /// Set vendor-specific level/layout fields in a PtPopulate struct from
    /// a deserialized EncHeader.  Phantom copies chain_index directly.
    static void set_level_info(PtPopulate& populate, const fhert::EncHeader& hdr)
    {
        populate.chain_index = hdr.chain_index;
    }

    //=========================================================================
    // Ciphertext accessor / populate (CiphertextSerializer<Traits> hooks)
    //=========================================================================
    using CtAccess = phantom::CtAccess;
    using CtPopulate = phantom::CtPopulate;
    static CtAccess access_ciphertext(const Ciphertext& ct);
    static void populate_ciphertext(Ciphertext& ct, const CtPopulate& data, Context* ctx = nullptr,
                                    void* stream = nullptr);

    /// chain_index from a CtAccess (same as plaintext).
    static uint64_t chain_index_from_ct_access(const CtAccess& access) { return access.chain_index; }

    /// Set vendor-specific level/layout fields in a CtPopulate from a CtHeader.
    static void set_ct_level_info(CtPopulate& populate, const fhert::CtHeader& hdr)
    {
        populate.chain_index = hdr.chain_index;
        populate.is_ntt_form = hdr.is_ntt_form != 0;
        populate.is_asymmetric = hdr.is_asymmetric != 0;
        populate.noise_scale_deg = hdr.noise_scale_deg;
    }

    //=========================================================================
    // KeySerializer vendor hooks — used by the common KeySerializer<Traits>
    //=========================================================================

    /// IO location hint: Phantom keys live in GPU memory.
    static constexpr rtio::IoLoc io_location() { return rtio::IoLoc::Device; }

    /// Whether to skip saving a key that has not been generated.
    /// Phantom checks is_key_generated() before saving.
    template <typename KeyT> static bool should_save_key(const KeyT& key) { return is_key_generated(key); }

    /// Resolve galois elements: if empty, extract from context.
    static std::vector<uint32_t> resolve_galois_elts(const Context& ctx, const std::vector<uint32_t>& galois_elts)
    {
        if (!galois_elts.empty())
            return galois_elts;
        return get_galois_elts(ctx);
    }

    //=========================================================================
    // Host type aliases (for De/Decode device-to-host conversion)
    //=========================================================================
    // Phantom operates directly on GPU types — no separate host type.
    using HostCiphertext = PhantomCiphertext;  ///< Same as Ciphertext (GPU-native)
    using HostPlaintext = PhantomPlaintext;    ///< Same as Plaintext (GPU-native)

//=========================================================================
// HE operation dispatch hooks
//=========================================================================
// The inline dispatch hooks live in a per-backend fragment (he_ops_decl.h)
// to keep this header organized by responsibility. The fragment is
// #include'd HERE, INSIDE struct VendorTraits, so its methods remain members
// of VendorTraits with identical ODR / Itanium mangling — required for
// cross-DSO JIT correctness (both the dlopen'd kernel .so and this lib
// instantiate these inline ops; changing the defining class breaks symbol
// unification). Do NOT wrap the fragment in a separate class/namespace.
#include "fhe/vendor/phantom/he_ops_decl.h"

    //=========================================================================
    // Lifecycle — context creation, key generation, evaluator management
    //=========================================================================

    /// Create vendor context from compiler parameters (normal mode)
    static std::unique_ptr<Context> create_context(bool with_bootstrap);

    /// Create vendor context for encode-only mode
    static std::unique_ptr<Context> create_context_encode_only(bool with_bootstrap);

    /// Generate all keys (normal init path)
    static void generate_keys(Context& ctx, std::unique_ptr<SecretKey>& sk, std::unique_ptr<PublicKey>& pk,
                              std::unique_ptr<RelinKey>& rlk, std::unique_ptr<GaloisKey>& rtk);

    /// Create encoder (for encode-only path and normal path)
    static std::unique_ptr<PhantomCKKSEncoder> create_encoder(Context& ctx);

    /// Create evaluator (combines encoder + keys + scaling for HE operations)
    static std::unique_ptr<CKKSEvaluator> create_evaluator(Context& ctx, const SecretKey& sk, const PublicKey& pk,
                                                           const RelinKey& rlk, const GaloisKey& rtk,
                                                           PhantomCKKSEncoder* encoder, double scaling_mod_size);

    /// Initialize bootstrap managers for given slot sizes.
    /// Returns the computed num_prime_parts.
    static uint64_t init_bootstrap(CKKSEvaluator* evaluator, BtsMgr& bootstrap, bool keys_loaded_from_kms,
                                   uint64_t scaling_mod_size, int32_t mul_depth, int32_t first_mod_size,
                                   int32_t scaling_mod_bits);

    /// Generate galois keys for the given rotation steps.
    /// Phantom-specific: operates on CKKSEvaluator (ACE uses KeyGenerator&).
    static void create_galois_keys_from_steps(CKKSEvaluator& evaluator, std::vector<int>& steps);

    /// Update the keys held by an existing evaluator and crypto struct
    /// (for session switching).  Reassigns the encryptor, decryptor,
    /// relin_keys, and galois_keys inside the CKKSEvaluator and
    /// populates the Crypto struct with the new session's encryptor/decryptor.
    /// Phantom-specific: takes Crypto& and Context& (ACE takes MEMORY_LAYOUT).
    static void upload_keys_to_evaluator(CKKSEvaluator& evaluator, Crypto& crypto, Context& ctx, const SecretKey& sk,
                                         const PublicKey& pk, const RelinKey& rlk, const GaloisKey& rtk);
};

}  // namespace phantom

// Compile-time interface check — ensures VendorTraits provides all
// required type aliases and static methods.
#include "fhe/vendor/concept.h"
static_assert(fhert::traits_check::is_valid_vendor_traits_v<phantom::VendorTraits>,
              "VendorTraits does not satisfy the VendorTraits interface — "
              "missing required type alias or static method");

#endif  // PHANTOM_VENDOR_TRAITS_H
