//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief vendor_traits.h
//! VendorTraits specialization for the ACE GPU backend.
//!
//! Maps the common FHE framework (KeySerializer<T>, EncIo<T>,
//! KeyMgr<T>, etc.) to ACE's vendor types and memory layouts.
//! This is the ONLY point where the framework touches ACE internals.

#ifndef ACE_VENDOR_TRAITS_H
#define ACE_VENDOR_TRAITS_H

#include <cstdint>
#include <string>
#include <vector>

// ACE vendor headers — these define the key, plaintext, ciphertext types
#include "ace/he_engine/Ciphertext.h"
#include "ace/he_engine/Context.h"
#include "ace/he_engine/Evaluator.h"
#include "ace/he_engine/host/ckks.h"
#include "ace/he_engine/host/decryptor.h"
#include "ace/he_engine/host/encryptor.h"
#include "ace/he_engine/host/keygenerator.h"
#include "ace/he_engine/Plaintext.h"
#include "fhe/enc/enc_header.h"
#include "fhe/io/io_policy.h"
#include "fhe/vendor/ace/bts_mgr.h"
#include "fhe/vendor/types.h"

// NOLINTBEGIN (readability-identifier-naming)

//! Global rt-API type aliases for the ACE backend.
//!
//! These bare names (CIPHERTEXT/CIPHER/PLAIN/...) are used by the codegen
//! dispatch header (rt_ace.h) and the codegen-generated kernel .cu. They used
//! to live in a separate rt_def.h; folded here so vendor_traits.h is the
//! single point where ACE vendor types meet the framework surface. Kept at
//! global scope (NOT inside namespace ace) because rt_ace.h's inline
//! signatures reference them unqualified.
typedef acelib::DeviceCipher CIPHERTEXT;
typedef acelib::DeviceCipher CIPHERTEXT3;
typedef acelib::DeviceCipher* CIPHER;
typedef acelib::DeviceCipher* CIPHER3;
typedef acelib::DevicePlain PLAINTEXT;
typedef acelib::DevicePlain* PLAIN;

//! Scale and level types for encode/eval dispatch
typedef double SCALE_T;
typedef acelib::host::parms_id_type LEVEL_T;

#define CIPHER_DEFINED 1
#define PLAIN_DEFINED 1

// NOLINTEND (readability-identifier-naming)

namespace ace
{

//=============================================================================
// Key serialization data containers
//=============================================================================
// Defined at namespace scope (not nested in VendorTraits) — plain data
// carriers, no backend-vendor coupling that requires nesting. ACE keys are
// host-side SEAL objects; SK/PK/RLK share the same serialized form (blob +
// N + L), so they alias KeyAccess / KeyPopulate. GaloisKey has the extra
// galois element list and extends them. VendorTraits re-exports them as the
// per-key alias names the concept and KeySerializer expect (SecretKeyAccess,
// PublicKeyAccess, ...).

struct KeyAccess
{
    std::string serialized;        // Host key serialized via save()
    uint64_t poly_modulus_degree;  // N (for header)
    uint64_t coeff_modulus_size;   // L (for header)
};
struct GaloisKeyAccess : KeyAccess
{
    std::vector<uint32_t> galois_elts;  // Galois element IDs
};

struct KeyPopulate
{
    std::string serialized;  // Data to load via unsafe_load()
};
struct GaloisKeyPopulate : KeyPopulate
{
    std::vector<uint32_t> galois_elts;
};

//=============================================================================
// Plaintext serialization data containers
//=============================================================================
// Extend the common PtAccessBase / PtPopulateBase with ACE-specific fields
// (parms_id level hash + is_ntt_form). Defined at namespace scope; VendorTraits
// re-exports them via using.

struct PtAccess : fhert::PtAccessBase
{
    acelib::host::parms_id_type parms_id;  // Level identifier
    bool is_ntt_form = false;              // NTT form flag
};
struct PtPopulate : fhert::PtPopulateBase
{
    acelib::host::parms_id_type parms_id;
    bool is_ntt_form = false;
};

//=============================================================================
// VendorTraits
//=============================================================================

struct VendorTraits
{
    //=========================================================================
    // Type aliases
    //=========================================================================

    using SecretKey = acelib::host::SecretKey;
    using PublicKey = acelib::host::PublicKey;
    using RelinKey = acelib::host::RelinKeys;
    using GaloisKey = acelib::host::GaloisKeys;
    using Context = acelib::host::HostContext;
    using Plaintext = acelib::DevicePlain;
    using Ciphertext = acelib::DeviceCipher;
    using ScaleType = double;
    using LevelType = acelib::host::parms_id_type;

    //=========================================================================
    // Evaluator — extends common template with ACE-specific BTS dispatch
    //=========================================================================
    // ACE's Evaluator adds a non-owning eval_ptr (BTS mode) and a device
    // context pointer. active_eval() dispatches to the bootstrap evaluator
    // when in BTS mode, otherwise to the primary eval.

    struct Evaluator : fhert::Evaluator<acelib::host::CKKSEncoder, acelib::Evaluator>
    {
        acelib::Evaluator* eval_ptr = nullptr;  ///< non-owning (BTS: points to BtsMgr's EvalWrapper)
        acelib::ContextPro* ctx = nullptr;      ///< device context pointer

        /// Get the active evaluator pointer.
        /// In BTS mode, eval_ptr points to the bootstrap evaluator; otherwise eval.get().
        acelib::Evaluator* active_eval() const { return eval_ptr ? eval_ptr : eval.get(); }

        // Destructor: clear non-owning pointers before base unique_ptrs release.
        // This ensures eval_ptr/ctx don't dangle if base Evaluator is destroyed
        // after the objects they point to.
        ~Evaluator()
        {
            eval_ptr = nullptr;
            ctx = nullptr;
        }

        // Move: custom to handle eval_ptr/ctx (base handles encoder/eval)
        using Base = fhert::Evaluator<acelib::host::CKKSEncoder, acelib::Evaluator>;
        Evaluator() = default;
        Evaluator(const Evaluator&) = delete;
        Evaluator& operator=(const Evaluator&) = delete;
        Evaluator(Evaluator&& o) noexcept : Base(std::move(o)), eval_ptr(o.eval_ptr), ctx(o.ctx)
        {
            o.eval_ptr = nullptr;
            o.ctx = nullptr;
        }
        Evaluator& operator=(Evaluator&& o) noexcept
        {
            if (this != &o) {
                Base::operator=(std::move(o));
                eval_ptr = o.eval_ptr;
                ctx = o.ctx;
                o.eval_ptr = nullptr;
                o.ctx = nullptr;
            }
            return *this;
        }
    };

    //=========================================================================
    // Per-session CUDA stream (Mode B-P) — ace backend
    //=========================================================================
    // ACE runs on host/PSDK; no per-session CUDA stream. The placeholder keeps
    // the FheContext multi-session API uniform across backends. ace still needs
    // per-thread device-context activation (tl_device_context) — handled by
    // Activate_session_for_this_thread via set_device_context.
    using SessionStream = void*;  // unused placeholder (always nullptr)
    // ACE has no per-session CUDA stream (host/PSDK). A unique_ptr stub keeps
    // the FheContext multi-session storage (std::vector<SessionStreamOwner>)
    // uniform with the phantom backend, which uses unique_ptr<cuda_stream_wrapper>.
    struct SessionStreamStub
    {
    };
    using SessionStreamOwner = std::unique_ptr<SessionStreamStub>;

    //=========================================================================
    // Thread-local FHE-context state (Mode B-P multi-thread safety)
    //=========================================================================
    // TLS lives in a single TU (ace vendor_traits.cu, `static thread_local`) and
    // is accessed ONLY via these exported functions — mirroring the existing
    // get/set_device_context pattern. (NOT `static inline thread_local` in the
    // shared template header: that inlines reads into the kernel .so and any
    // non-main worker pthread resolves to a different TLS instance.)
    static void* current_context_ptr();
    static void set_current_context_ptr(void* p);
    static void* acquire_current_context_ptr();
    static int active_session_idx();
    static void set_active_session_idx(int idx);

    //=========================================================================
    // Crypto — per-session encrypt/decrypt (common template)
    //=========================================================================

    /// Uses the common Crypto template with ACE vendor types.
    /// Populated during init or switch_session; null in compute-only mode.
    using Crypto = fhert::Crypto<acelib::host::Encryptor, acelib::host::Decryptor>;

    //=========================================================================
    // BtsMgr
    //=========================================================================

    using BtsMgr = ace::BtsMgr;

    // Mode B-P per-session GaloisTool — phantom-only feature. ace backend
    // does not route per-session tools (B-P defaults to phantom); this
    // placeholder satisfies the FheContext template member so ace compiles.
    struct GaloisTool
    {
    };

    //=========================================================================
    // Key accessor — extract serialized data for key persistence
    //=========================================================================
    // ACE keys are host-side SEAL objects. We use their built-in save()
    // method to serialize to a binary blob, wrapped in the common
    // KeyHeader + KeyWriter/KeyReader protocol.

    // ACE keys are host-side SEAL objects; SK/PK/RLK share the exact same
    // serialized form (blob + N + L), so they alias one struct. GaloisKey has
    // the extra galois element list and extends it. The struct definitions
    // live at namespace scope (see above); here we only re-export the per-key
    // alias names the concept + KeySerializer expect.
    using SecretKeyAccess = KeyAccess;
    using PublicKeyAccess = KeyAccess;
    using RelinKeyAccess = KeyAccess;
    using GaloisKeyAccess = ace::GaloisKeyAccess;

    static SecretKeyAccess access_secret_key(const SecretKey& sk, const Context& ctx);
    static PublicKeyAccess access_public_key(const PublicKey& pk);
    static RelinKeyAccess access_relin_key(const RelinKey& rlk);
    static GaloisKeyAccess access_galois_key(const GaloisKey& rtk);

    //=========================================================================
    // Key populate — reconstruct key object from serialized data
    //=========================================================================

    // Same shape as Access: SK/PK/RLK populate from a single serialized blob;
    // GaloisKey also carries the element list.
    using SecretKeyPopulate = KeyPopulate;
    using PublicKeyPopulate = KeyPopulate;
    using RelinKeyPopulate = KeyPopulate;
    using GaloisKeyPopulate = ace::GaloisKeyPopulate;

    static void populate_secret_key(SecretKey& sk, const SecretKeyPopulate& data, uint64_t N, uint64_t L, Context& ctx);
    static void populate_public_key(PublicKey& pk, const PublicKeyPopulate& data, uint64_t N, uint64_t L, Context& ctx);
    static void populate_relin_key(RelinKey& rlk, const RelinKeyPopulate& data, uint64_t N, uint64_t L, Context& ctx);
    static void populate_galois_key(GaloisKey& rtk, const GaloisKeyPopulate& data, uint64_t N, uint64_t L,
                                    Context& ctx);

    //=========================================================================
    // Context accessors — used by FheContext, KeyMgr, KeySerializer
    //=========================================================================

    static uint64_t poly_degree(const Context& ctx);
    static uint64_t coeff_mod_size(const Context& ctx);

    /// Get galois elements from the key manager's galois keys.
    /// Returns empty vector if not available.
    static std::vector<uint32_t> get_galois_elts(const Context& ctx);

    /// Check if a key object has been generated / loaded.
    static bool is_key_generated(const SecretKey& sk);
    static bool is_key_generated(const PublicKey& pk);
    static bool is_key_generated(const RelinKey& rlk);
    static bool is_key_generated(const GaloisKey& rtk);

    //=========================================================================
    // Plaintext accessor / populate (extends common base)
    //=========================================================================
    // DevicePlain is a GPU object. We extract raw GPU data + metadata for
    // serialization, and reconstruct via resize() + cudaMemcpy for loading.
    // ACE extends the common base with parms_id (level hash) and is_ntt_form.

    // PtAccess / PtPopulate defined at namespace scope (see above); re-export
    // the alias names the common EncIo / KeySerializer templates expect.
    using PtAccess = ace::PtAccess;
    using PtPopulate = ace::PtPopulate;
    static PtAccess access_plaintext(const Plaintext& pt);

    static void populate_plaintext(Plaintext& pt, const PtPopulate& data, Context* ctx = nullptr,
                                   void* stream = nullptr);

    //=========================================================================
    // EncIo vendor hooks — used by the common EncIo<Traits> implementation
    //=========================================================================

    /// Derive the encoding level (chain_index) from a plaintext access struct.
    /// ACE stores the level as parms_id (a hash); we look up chain_index via
    /// the device context's context_data table.
    static uint64_t chain_index_from_access(const PtAccess& access)
    {
        auto* dev_ctx = get_device_context();
        if (dev_ctx) {
            auto ctx_data = dev_ctx->get_context_data(access.parms_id);
            if (ctx_data) {
                return ctx_data->chain_index();
            }
        }
        return 0;
    }

    /// Derive the EncHeader reserved2 field from a plaintext access struct.
    /// ACE uses this bit to store is_ntt_form.
    static uint32_t pt_reserved2_from_access(const PtAccess& access) { return access.is_ntt_form ? 1u : 0u; }

    /// Set vendor-specific level/layout fields in a PtPopulate struct from
    /// a deserialized EncHeader.  ACE converts chain_index back to parms_id
    /// and restores the is_ntt_form flag.
    static void set_level_info(PtPopulate& populate, const fhert::EncHeader& hdr)
    {
        auto* dev_ctx = get_device_context();
        if (dev_ctx) {
            populate.parms_id = dev_ctx->get_parms_id_from_level(static_cast<std::size_t>(hdr.chain_index));
        }
        populate.is_ntt_form = (hdr.reserved2 != 0);
    }

    //=========================================================================
    // Device context accessor (thread-local singleton)
    //=========================================================================
    // DevicePlain::resize() and encode operations require a ContextPro*,
    // but the common EncIo template only has access to ContextT*
    // (which is HostContext*). We store the device context in a thread-local
    // singleton, set during FheContext construction.

    static acelib::ContextPro* get_device_context();
    static void set_device_context(acelib::ContextPro* ctx);

    //=========================================================================
    // KeySerializer vendor hooks — used by the common KeySerializer<Traits>
    //=========================================================================

    /// IO location hint: ACE keys live in host memory (SEAL objects).
    static constexpr rtio::IoLoc io_location() { return rtio::IoLoc::Host; }

    /// Whether to skip saving a key that has not been generated.
    /// ACE always saves all keys (they are always available after generation).
    template <typename KeyT> static bool should_save_key(const KeyT& /*key*/) { return true; }

    /// Resolve galois elements: ACE passes them through as-is.
    static std::vector<uint32_t> resolve_galois_elts(const Context& /*ctx*/, const std::vector<uint32_t>& galois_elts)
    {
        return galois_elts;
    }

    //=========================================================================
    // Host type aliases (for De/Decode device-to-host conversion)
    //=========================================================================
    // ACE operates on device types (DeviceCipher/DevicePlain) for HE, but
    // decryption/encoding uses host types (host::Ciphertext/host::Plaintext).
    using HostCiphertext = acelib::host::Ciphertext;  ///< Host ciphertext for decrypt
    using HostPlaintext = acelib::host::Plaintext;    ///< Host plaintext for decode

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
#include "fhe/vendor/ace/he_ops_decl.h"

    //=========================================================================
    // Lifecycle — context creation, key generation, evaluator management
    //=========================================================================

    /// Create vendor context from compiler parameters (normal mode)
    static std::unique_ptr<Context> create_context(bool with_bootstrap);

    /// Create vendor context for encode-only mode
    static std::unique_ptr<Context> create_context_encode_only(bool with_bootstrap);

    /// Generate all keys (normal init path).
    /// ACE-specific: has use_sparse_wrapping parameter.
    static void generate_keys(Context& ctx, std::unique_ptr<SecretKey>& sk, std::unique_ptr<PublicKey>& pk,
                              std::unique_ptr<RelinKey>& rlk, std::unique_ptr<GaloisKey>& rtk,
                              bool use_sparse_wrapping = false);

    /// Create CKKS encoder (for encode-only path and normal path)
    static std::unique_ptr<acelib::host::CKKSEncoder> create_encoder(Context& ctx);

    /// Create evaluator (combines GPU evaluator + device context)
    static std::unique_ptr<Evaluator>
    create_evaluator(Context& ctx, const SecretKey& sk, const PublicKey& pk, const RelinKey& rlk, const GaloisKey& rtk,
                     acelib::host::CKKSEncoder* encoder,  // non-owning, just for parameter
                     double scaling_mod_size);

    /// Upload keys to the GPU evaluator after generation or KMS load.
    /// ACE-specific: requires MEMORY_LAYOUT, does not take Crypto&/Context&
    /// (keys are uploaded directly to the GPU evaluator object).
    static void upload_keys_to_evaluator(Evaluator& eval, const PublicKey& pk, const SecretKey& sk, const RelinKey& rlk,
                                         const GaloisKey& rtk,
                                         acelib::MEMORY_LAYOUT data_layout = acelib::MEMORY_LAYOUT::ROW_MAJOR);

    /// Initialize bootstrap managers.
    /// Returns the computed num_prime_parts.
    static uint64_t init_bootstrap(acelib::Evaluator* evaluator, BtsMgr& bootstrap, bool keys_loaded_from_kms,
                                   uint64_t scaling_mod_size, int32_t mul_depth, int32_t first_mod_size,
                                   int32_t scaling_mod_bits);

    /// Generate galois keys for the given rotation steps.
    /// ACE-specific: takes a KeyGenerator& (Phantom takes CKKSEvaluator&).
    static void create_galois_keys_from_steps(acelib::host::KeyGenerator& kgen, GaloisKey& rtk,
                                              const std::vector<int>& steps);

    /// Rebuild galois elements after KMS key load (no-op for ACE currently,
    /// since galois keys are regenerated from steps).
    static void rebuild_galois_tool(Context& ctx, const std::vector<uint32_t>& elts);
};

}  // namespace ace

// Compile-time interface check — ensures VendorTraits provides all
// required type aliases and static methods.
#include "fhe/vendor/concept.h"
static_assert(fhert::traits_check::is_valid_vendor_traits_v<ace::VendorTraits>,
              "VendorTraits does not satisfy the VendorTraits interface — "
              "missing required type alias or static method");

#endif  // ACE_VENDOR_TRAITS_H
