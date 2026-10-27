// SEAL-compatible C++ API implemented on top of the clib SDK (clib.h).
//
// This header provides the subset of the Microsoft SEAL 3.6 (modified for
// FHE-MP-CNN) class interface used by the cpu-ckks single-key CNN code:
//
//     seal::SEALContext, seal::CKKSEncoder, seal::Encryptor, seal::Evaluator,
//     seal::Decryptor, seal::Ciphertext, seal::Plaintext
//
// plus the supporting types those classes reference (EncryptionParameters,
// key classes, KeyGenerator, parms_id_type, MemoryPoolHandle, ...).
//
// Backend model: create ClibSession before all SEAL objects inside a
// clib::Run_application graph. Encode/encrypt declare ANT offline objects;
// Evaluator routes operations to clib. Destruction detaches the session.
// Without a session, message-backed objects simulate clear slot arithmetic.
// Decryptor returns that clear shadow, never an actual decryption of clib
// ciphertext. Use Ciphertext::save_output and ANT verification for accuracy.
// CoeffModulus entries are bookkeeping descriptors, not real primes.
// One SEAL rescale consumes two physical Q limbs. Only one session may be
// active; API calls are serial, with parallelism inside the clib kernels.

#ifndef IIEDECL_SEAL_API_H
#define IIEDECL_SEAL_API_H

#include "clib.h"

#include <complex>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace seal
{
    /**
    Identifies a set of encryption parameters. In the clib backend a parameter
    level is identified by its number of coefficient-modulus (Q) limbs.
    */
    struct parms_id_type
    {
        std::uint32_t q_limbs = 0;

        bool operator==(const parms_id_type &other) const noexcept
        {
            return q_limbs == other.q_limbs;
        }

        bool operator!=(const parms_id_type &other) const noexcept
        {
            return q_limbs != other.q_limbs;
        }
    };

    /**
    A parms_id_type value consisting of zeros (invalid/unset level).
    */
    extern const parms_id_type parms_id_zero;

    /**
    The encryption scheme type. Only CKKS is supported by the clib backend.
    */
    enum class scheme_type : int
    {
        none = 0x0,
        bfv = 0x1,
        ckks = 0x2,
        bgv = 0x3
    };

    /**
    Security level (kept for SEAL source compatibility; enforced by the clib
    material, not by this shim).
    */
    enum class sec_level_type : int
    {
        none = 0,
        tc128 = 128,
        tc192 = 192,
        tc256 = 256
    };

    /**
    Stub memory-pool handle kept for SEAL signature compatibility. The clib
    backend manages its own memory; pool arguments are accepted and ignored.
    */
    class MemoryPoolHandle
    {
    public:
        MemoryPoolHandle() = default;

        explicit operator bool() const noexcept
        {
            return true;
        }
    };

    class MemoryManager
    {
    public:
        static MemoryPoolHandle GetPool()
        {
            return MemoryPoolHandle{};
        }
    };

    /**
    A coefficient-modulus prime descriptor. Only the bit count and an
    approximate 64-bit value are tracked; the true primes live in the clib
    material.
    */
    class Modulus
    {
    public:
        Modulus() = default;

        explicit Modulus(std::uint64_t value) : value_(value)
        {
            std::uint64_t v = value;
            while (v != 0)
            {
                ++bit_count_;
                v >>= 1;
            }
        }

        std::uint64_t value() const noexcept
        {
            return value_;
        }

        int bit_count() const noexcept
        {
            return bit_count_;
        }

        bool is_prime() const noexcept
        {
            return true;
        }

    private:
        std::uint64_t value_ = 0;
        int bit_count_ = 0;
    };

    /**
    Creates a coeff_modulus from a list of bit sizes. The Modulus values are
    synthetic (2^bits - 1); they are only used for metadata such as
    total_coeff_modulus_bit_count. The clib material holds the real primes.
    */
    class CoeffModulus
    {
    public:
        static std::vector<Modulus> Create(
            std::size_t poly_modulus_degree, const std::vector<int> &bit_sizes);
    };

    /**
    Stores user-supplied encryption parameters. The clib backend derives the
    actual parameter set from the material directory; the values recorded here
    (scheme, poly_modulus_degree, number of coeff-modulus limbs) are used for
    bookkeeping until the context is opened, and must agree with the material.
    */
    class EncryptionParameters
    {
    public:
        explicit EncryptionParameters(scheme_type scheme) : scheme_(scheme)
        {}

        void set_poly_modulus_degree(std::size_t poly_modulus_degree)
        {
            poly_modulus_degree_ = poly_modulus_degree;
        }

        void set_coeff_modulus(const std::vector<Modulus> &coeff_modulus)
        {
            coeff_modulus_ = coeff_modulus;
        }

        /**
        FHE-MP-CNN SEAL extension: records the secret-key Hamming weight. The
        clib material already embeds the key, so this is metadata only.
        */
        void set_secret_key_hamming_weight(std::uint64_t hamming_weight)
        {
            secret_key_hamming_weight_ = hamming_weight;
        }

        /**
        Shim extension: directory passed to clib::Open_context. Defaults to
        $CLIB_MATERIAL_DIR, or "material" if unset.
        */
        void set_material_dir(const std::string &material_dir)
        {
            material_dir_ = material_dir;
        }

        /**
        Shim extension: output directory passed to clib::Open_context.
        Defaults to $CLIB_OUTPUT_DIR, or "." if unset.
        */
        void set_output_dir(const std::string &output_dir)
        {
            output_dir_ = output_dir;
        }

        scheme_type scheme() const noexcept
        {
            return scheme_;
        }

        std::size_t poly_modulus_degree() const noexcept
        {
            return poly_modulus_degree_;
        }

        const std::vector<Modulus> &coeff_modulus() const noexcept
        {
            return coeff_modulus_;
        }

        std::uint64_t secret_key_hamming_weight() const noexcept
        {
            return secret_key_hamming_weight_;
        }

        const std::string &material_dir() const noexcept
        {
            return material_dir_;
        }

        const std::string &output_dir() const noexcept
        {
            return output_dir_;
        }

    private:
        scheme_type scheme_ = scheme_type::none;
        std::size_t poly_modulus_degree_ = 0;
        std::vector<Modulus> coeff_modulus_;
        std::uint64_t secret_key_hamming_weight_ = 0;
        std::string material_dir_;
        std::string output_dir_;
    };

    /**
    Minimal parameter-qualifier record. Parameters are considered valid once
    the clib context accepted them (they are validated by the material).
    */
    class EncryptionParameterQualifiers
    {
    public:
        enum class error_type : int
        {
            none = -1,
            success = 0
        };

        error_type parameter_error = error_type::success;
        bool using_fft = true;
        bool using_ntt = true;
        bool using_batching = true;
        bool using_fast_plain_lift = true;
        bool using_descending_modulus_chain = true;
        sec_level_type sec_level = sec_level_type::none;

        bool parameters_set() const noexcept
        {
            return parameter_error == error_type::success;
        }

        const char *parameter_error_name() const noexcept
        {
            return "success";
        }

        const char *parameter_error_message() const noexcept
        {
            return "parameters are valid (clib backend)";
        }
    };

    namespace detail
    {
        struct ContextImpl;
        struct CipherOps;
    }

    class Ciphertext;
    class Plaintext;
    class CKKSEncoder;
    class Encryptor;
    class Evaluator;
    class Decryptor;

    /**
    Validates encryption parameters and owns the clib context handle. The clib
    context is opened lazily on the first backend operation; until then the
    recorded EncryptionParameters are used for level bookkeeping.
    */
    class SEALContext
    {
    public:
        /**
        Pre-computation data for one level of the modulus-switching chain.
        Levels are identified by Q-limb count; chain_index() is 1 for the top
        data level and grows by one per (2-limb) level drop, mirroring SEAL.
        */
        class ContextData
        {
        public:
            ContextData(const ContextData &) = delete;
            ContextData(ContextData &&) = default;
            ContextData &operator=(ContextData &&) = default;

            const EncryptionParameters &parms() const noexcept;

            const parms_id_type &parms_id() const noexcept
            {
                return parms_id_;
            }

            EncryptionParameterQualifiers qualifiers() const noexcept
            {
                return EncryptionParameterQualifiers{};
            }

            /**
            Sum of the bit counts of the first q_limbs coeff-modulus primes.
            */
            int total_coeff_modulus_bit_count() const noexcept;

            std::size_t chain_index() const noexcept
            {
                return chain_index_;
            }

            std::shared_ptr<const ContextData> prev_context_data() const;
            std::shared_ptr<const ContextData> next_context_data() const;

        private:
            friend class SEALContext;
            ContextData(std::shared_ptr<detail::ContextImpl> impl, parms_id_type parms_id);

            std::shared_ptr<detail::ContextImpl> impl_;
            parms_id_type parms_id_{};
            std::size_t chain_index_ = 0;
        };

        explicit SEALContext(
            const EncryptionParameters &parms, bool expand_mod_chain = true,
            sec_level_type sec_level = sec_level_type::tc128);

        SEALContext(const SEALContext &copy);

        SEALContext(SEALContext &&source) noexcept;

        SEALContext &operator=(const SEALContext &assign);

        SEALContext &operator=(SEALContext &&assign) noexcept;

        ~SEALContext();

        std::shared_ptr<const ContextData> get_context_data(parms_id_type parms_id) const;

        std::shared_ptr<const ContextData> key_context_data() const;

        std::shared_ptr<const ContextData> first_context_data() const;

        std::shared_ptr<const ContextData> last_context_data() const;

        bool parameters_set() const;

        const char *parameter_error_name() const;

        const char *parameter_error_message() const;

        const parms_id_type &key_parms_id() const noexcept
        {
            return key_parms_id_;
        }

        const parms_id_type &first_parms_id() const noexcept
        {
            return first_parms_id_;
        }

        const parms_id_type &last_parms_id() const noexcept
        {
            return last_parms_id_;
        }

        bool using_keyswitching() const noexcept
        {
            return true;
        }

        // ---- Shim extensions (not part of the SEAL API) ----

        /**
        Number of Q limbs at the top level (before any level drop).
        */
        std::uint32_t top_q_limbs() const;

        /**
        Number of CKKS slots (poly_modulus_degree / 2).
        */
        std::uint32_t slot_count() const;

        /**
        Ring degree N of the polynomial modulus.
        */
        std::uint32_t ring_degree() const;

        /**
        Optional clib profiling passthroughs. Enable immediately after
        construction if profiling is wanted.
        */
        void enable_profile();
        void begin_stage(const char *name);
        void end_stage();
        void write_profile(const char *output_dir);

    private:
        friend class CKKSEncoder;
        friend class Encryptor;
        friend class Evaluator;
        friend class Decryptor;
        friend class Ciphertext;
        friend class Plaintext;
        friend struct detail::ContextImpl;
        friend struct detail::CipherOps;

        std::shared_ptr<detail::ContextImpl> impl_;
        parms_id_type key_parms_id_{};
        parms_id_type first_parms_id_{};
        parms_id_type last_parms_id_{};
    };

    /**
    Key classes. Key material is owned by the clib context (loaded from the
    material directory); these objects are opaque markers kept so SEAL-based
    code compiles and reads naturally.
    */
    class PublicKey
    {
    public:
        PublicKey() = default;
    };

    class SecretKey
    {
    public:
        SecretKey() = default;
    };

    class KSwitchKeys
    {
    public:
        KSwitchKeys() = default;
    };

    class RelinKeys : public KSwitchKeys
    {
    public:
        RelinKeys() = default;
    };

    class GaloisKeys : public KSwitchKeys
    {
    public:
        GaloisKeys() = default;
    };

    /**
    Generates keys. In the clib backend the keys already exist inside the
    opened material, so the create_* functions are no-ops that simply keep
    SEAL-based driver code working unchanged.
    */
    class KeyGenerator
    {
    public:
        explicit KeyGenerator(const SEALContext &context) : context_(context)
        {}

        KeyGenerator(const SEALContext &context, std::uint64_t secret_key_hamming_weight)
            : context_(context), hamming_weight_(secret_key_hamming_weight)
        {}

        const SecretKey &secret_key() const noexcept
        {
            return secret_key_;
        }

        void create_public_key(PublicKey &destination) const
        {
            (void)destination;
        }

        void create_relin_keys(RelinKeys &destination) const
        {
            (void)destination;
        }

        void create_relin_keys(std::size_t count, RelinKeys &destination) const
        {
            (void)count;
            (void)destination;
        }

        void create_galois_keys(const std::vector<int> &steps, GaloisKeys &destination) const
        {
            (void)steps;
            (void)destination;
        }

        void create_galois_keys(GaloisKeys &destination) const
        {
            (void)destination;
        }

    private:
        SEALContext context_;
        SecretKey secret_key_;
        std::uint64_t hamming_weight_ = 0;
    };

    /**
    Stores a plaintext element. A Plaintext is either message-backed (values
    stored host-side by CKKSEncoder::encode) or handle-backed (a clib::PLAIN
    loaded with Plaintext::load). Copies of a handle-backed Plaintext share
    the underlying clib plaintext, which is treated as immutable.
    */
    class Plaintext
    {
    public:
        using pt_coeff_type = std::uint64_t;

        Plaintext() = default;

        Plaintext(const Plaintext &copy) = default;

        Plaintext(Plaintext &&source) = default;

        Plaintext &operator=(const Plaintext &assign) = default;

        Plaintext &operator=(Plaintext &&assign) = default;

        ~Plaintext() = default;

        bool is_zero() const;

        bool is_ntt_form() const noexcept
        {
            return is_ntt_form_;
        }

        /**
        Number of slots carrying values in message mode; 0 for a handle-backed
        or empty plaintext (the clib plaintext layout is opaque).
        */
        std::size_t coeff_count() const noexcept
        {
            return values_.size();
        }

        void release() noexcept;

        double &scale() noexcept
        {
            return scale_;
        }

        const double &scale() const noexcept
        {
            return scale_;
        }

        parms_id_type &parms_id() noexcept
        {
            return parms_id_;
        }

        const parms_id_type &parms_id() const noexcept
        {
            return parms_id_;
        }

        MemoryPoolHandle pool() const noexcept
        {
            return MemoryPoolHandle{};
        }

        // ---- Shim extensions ----

        /**
        Loads a pre-encoded clib plaintext from a file (clib::Load_plain).
        */
        void load(const SEALContext &context, const std::string &path);

        /**
        Whether this plaintext wraps a clib handle.
        */
        bool is_handle_backed() const noexcept
        {
            return static_cast<bool>(handle_);
        }

    private:
        friend class CKKSEncoder;
        friend class Encryptor;
        friend class Evaluator;
        friend class Decryptor;
        friend struct detail::CipherOps;

        std::shared_ptr<clib_plain> handle_;
        std::vector<std::complex<double>> values_;
        double scale_ = 1.0;
        parms_id_type parms_id_{};
        bool is_ntt_form_ = true;
    };

    /**
    Stores a ciphertext element. A Ciphertext holds at most one of: a
    clib::CIPHER (size 2), a clib::CIPHER3 (size 3, un-relinearized product),
    or message-backed slot values (from Encryptor::encrypt). Copying performs
    a deep copy through clib::Copy_ciph and requires an active SEALContext;
    size-3 clib ciphertexts cannot be copied (relinearize first).
    */
    class Ciphertext
    {
    public:
        using ct_coeff_type = std::uint64_t;

        Ciphertext() = default;

        explicit Ciphertext(const SEALContext &context);

        explicit Ciphertext(const SEALContext &context, parms_id_type parms_id);

        explicit Ciphertext(const SEALContext &context, parms_id_type parms_id, std::size_t size_capacity);

        Ciphertext(const Ciphertext &copy);

        Ciphertext(Ciphertext &&source) noexcept;

        Ciphertext &operator=(const Ciphertext &assign);

        Ciphertext &operator=(Ciphertext &&assign) noexcept;

        ~Ciphertext();

        void reserve(const SEALContext &context, parms_id_type parms_id, std::size_t size_capacity);

        void reserve(const SEALContext &context, std::size_t size_capacity);

        void reserve(std::size_t size_capacity);

        void resize(const SEALContext &context, parms_id_type parms_id, std::size_t size);

        void resize(const SEALContext &context, std::size_t size);

        void resize(std::size_t size);

        void release() noexcept;

        std::size_t coeff_modulus_size() const noexcept
        {
            return parms_id_.q_limbs;
        }

        std::size_t poly_modulus_degree() const noexcept
        {
            return ring_degree_;
        }

        /**
        Number of polynomials: 0 (empty), 2 (normal), or 3 (un-relinearized
        product).
        */
        std::size_t size() const noexcept;

        std::size_t size_capacity() const noexcept
        {
            return size();
        }

        bool is_transparent() const;

        bool is_ntt_form() const noexcept
        {
            return is_ntt_form_;
        }

        parms_id_type &parms_id() noexcept
        {
            return parms_id_;
        }

        const parms_id_type &parms_id() const noexcept
        {
            return parms_id_;
        }

        double &scale() noexcept
        {
            return scale_;
        }

        const double &scale() const noexcept
        {
            return scale_;
        }

        MemoryPoolHandle pool() const noexcept
        {
            return MemoryPoolHandle{};
        }

        // ---- Shim extensions ----

        /**
        Scale degree (1 = fresh/aligned, 2 = lazy scale after a multiply),
        from clib::Scale_degree for handle-backed ciphertexts.
        */
        std::uint32_t scale_degree() const noexcept
        {
            return scale_degree_;
        }

        /**
        Saves the ciphertext to a file (clib::Save_ciph).
        */
        void save(const SEALContext &context, const std::string &path) const;

        /**
        Loads a ciphertext from a file (clib::Load_ciph), replacing the
        current contents.
        */
        void load(const SEALContext &context, const std::string &path);

        /**
        Whether this ciphertext wraps a clib handle.
        */
        bool is_handle_backed() const noexcept
        {
            return ct2_ != nullptr || ct3_ != nullptr;
        }

        /**
        Declares this ciphertext as a named application output
        (clib::Save_output). The ANT verify step decrypts it offline and,
        when the golden-model shadow values are available, checks them
        against abs_tolerance as the plaintext reference. Returns true when
        an output was declared (clib backend active and handle-backed),
        false on a pure golden-model (message-backed) run.
        */
        bool save_output(const char *name, double abs_tolerance) const;

    private:
        friend class CKKSEncoder;
        friend class Encryptor;
        friend class Evaluator;
        friend class Decryptor;
        friend struct detail::CipherOps;

        void free_handles() noexcept;
        void install(clib::CIPHER handle);
        void install(clib::CIPHER3 handle);

        clib::CIPHER ct2_ = nullptr;
        clib::CIPHER3 ct3_ = nullptr;
        std::vector<std::complex<double>> shadow_;
        std::size_t shadow_size_ = 2;
        double scale_ = 1.0;
        std::uint32_t scale_degree_ = 1;
        parms_id_type parms_id_{};
        std::uint32_t ring_degree_ = 0;
        bool is_ntt_form_ = true;
    };

    /**
    Encodes vectors of real numbers into plaintext slot values. In this shim
    the encoder is message-oriented: encode() stores the slot values in the
    destination Plaintext together with the scale and level, and decode()
    reads them back. (The clib backend has no host-side encoder; the actual
    polynomial encoding happens inside the clib library.)
    */
    class CKKSEncoder
    {
    public:
        explicit CKKSEncoder(const SEALContext &context);

        template <
            typename T, typename = std::enable_if_t<
                            std::is_same<std::remove_cv_t<T>, double>::value ||
                            std::is_same<std::remove_cv_t<T>, std::complex<double>>::value>>
        void encode(
            const std::vector<T> &values, parms_id_type parms_id, double scale, Plaintext &destination,
            MemoryPoolHandle pool = MemoryManager::GetPool())
        {
            (void)pool;
            std::vector<std::complex<double>> as_complex(values.begin(), values.end());
            encode_message(as_complex, parms_id, scale, destination);
        }

        template <
            typename T, typename = std::enable_if_t<
                            std::is_same<std::remove_cv_t<T>, double>::value ||
                            std::is_same<std::remove_cv_t<T>, std::complex<double>>::value>>
        void encode(
            const std::vector<T> &values, double scale, Plaintext &destination,
            MemoryPoolHandle pool = MemoryManager::GetPool())
        {
            encode(values, context_.first_parms_id(), scale, destination, std::move(pool));
        }

        void encode(
            double value, parms_id_type parms_id, double scale, Plaintext &destination,
            MemoryPoolHandle pool = MemoryManager::GetPool())
        {
            (void)pool;
            encode_broadcast(std::complex<double>(value, 0.0), parms_id, scale, destination);
        }

        void encode(
            double value, double scale, Plaintext &destination, MemoryPoolHandle pool = MemoryManager::GetPool())
        {
            encode(value, context_.first_parms_id(), scale, destination, std::move(pool));
        }

        void encode(
            std::complex<double> value, parms_id_type parms_id, double scale, Plaintext &destination,
            MemoryPoolHandle pool = MemoryManager::GetPool())
        {
            (void)pool;
            encode_broadcast(value, parms_id, scale, destination);
        }

        void encode(
            std::complex<double> value, double scale, Plaintext &destination,
            MemoryPoolHandle pool = MemoryManager::GetPool())
        {
            encode(value, context_.first_parms_id(), scale, destination, std::move(pool));
        }

        void encode(std::int64_t value, parms_id_type parms_id, Plaintext &destination);

        void encode(std::int64_t value, Plaintext &destination)
        {
            encode(value, context_.first_parms_id(), destination);
        }

        template <
            typename T, typename = std::enable_if_t<
                            std::is_same<std::remove_cv_t<T>, double>::value ||
                            std::is_same<std::remove_cv_t<T>, std::complex<double>>::value>>
        void decode(
            const Plaintext &plain, std::vector<T> &destination, MemoryPoolHandle pool = MemoryManager::GetPool())
        {
            (void)pool;
            std::vector<std::complex<double>> as_complex;
            decode_message(plain, as_complex);
            destination.resize(as_complex.size());
            for (std::size_t i = 0; i < as_complex.size(); i++)
            {
                destination[i] = from_slot<T>(as_complex[i]);
            }
        }

        std::size_t slot_count() const noexcept
        {
            return slots_;
        }

        /**
        FHE-MP-CNN SEAL extension: decode() returns only the first
        sparse_slots values.
        */
        void set_sparse_slots(std::size_t sparse_slots)
        {
            sparse_slots_ = sparse_slots;
        }

    private:
        template <typename T>
        static T from_slot(const std::complex<double> &value);

        void encode_message(
            const std::vector<std::complex<double>> &values, parms_id_type parms_id, double scale,
            Plaintext &destination);

        void encode_broadcast(std::complex<double> value, parms_id_type parms_id, double scale, Plaintext &destination);

        void decode_message(const Plaintext &plain, std::vector<std::complex<double>> &destination) const;

        SEALContext context_;
        std::size_t slots_ = 0;
        std::size_t sparse_slots_ = 0;
    };

    /**
    Encrypts Plaintext objects into Ciphertext objects. The clib backend is
    evaluation-only, so encryption is realized at the message level: the slot
    values are wrapped into a message-backed Ciphertext. Encrypting a
    handle-backed (loaded) plaintext throws; bring pre-encrypted inputs in
    with Ciphertext::load instead.
    */
    class Encryptor
    {
    public:
        Encryptor(const SEALContext &context, const PublicKey &public_key);

        Encryptor(const SEALContext &context, const SecretKey &secret_key);

        Encryptor(const SEALContext &context, const PublicKey &public_key, const SecretKey &secret_key);

        void set_public_key(const PublicKey &public_key)
        {
            (void)public_key;
        }

        void set_secret_key(const SecretKey &secret_key)
        {
            (void)secret_key;
        }

        void encrypt(
            const Plaintext &plain, Ciphertext &destination, MemoryPoolHandle pool = MemoryManager::GetPool()) const;

        void encrypt_zero(Ciphertext &destination, MemoryPoolHandle pool = MemoryManager::GetPool()) const;

        void encrypt_zero(
            parms_id_type parms_id, Ciphertext &destination, MemoryPoolHandle pool = MemoryManager::GetPool()) const;

        void encrypt_symmetric(
            const Plaintext &plain, Ciphertext &destination, MemoryPoolHandle pool = MemoryManager::GetPool()) const;

        void encrypt_zero_symmetric(Ciphertext &destination, MemoryPoolHandle pool = MemoryManager::GetPool()) const;

        void encrypt_zero_symmetric(
            parms_id_type parms_id, Ciphertext &destination, MemoryPoolHandle pool = MemoryManager::GetPool()) const;

    private:
        SEALContext context_;
    };

    /**
    Decrypts Ciphertext objects into Plaintext objects. Only message-backed
    ciphertexts (produced by this shim's Encryptor) can be decrypted: the clib
    backend is evaluation-only and exposes no decryption API, so decrypting a
    handle-backed ciphertext throws std::runtime_error.
    */
    class Decryptor
    {
    public:
        Decryptor(const SEALContext &context, const SecretKey &secret_key);

        void decrypt(const Ciphertext &encrypted, Plaintext &destination);

        int invariant_noise_budget(const Ciphertext &encrypted);

    private:
        SEALContext context_;
    };

    /**
    Provides operations on ciphertexts. Operations on handle-backed
    ciphertexts route to the clib backend one operation per call, with no
    implicit alignment, relinearization, or rescale (matching clib.h
    semantics). Operations on message-backed ciphertexts are simulated
    element-wise on the slot values.
    */
    class Evaluator
    {
    public:
        Evaluator(const SEALContext &context, CKKSEncoder &encoder);

        void negate_inplace(Ciphertext &encrypted) const;

        void negate(const Ciphertext &encrypted, Ciphertext &destination) const
        {
            destination = encrypted;
            negate_inplace(destination);
        }

        void add_inplace(Ciphertext &encrypted1, const Ciphertext &encrypted2) const;

        void add(const Ciphertext &encrypted1, const Ciphertext &encrypted2, Ciphertext &destination) const;

        void add_many(const std::vector<Ciphertext> &encrypteds, Ciphertext &destination) const;

        void sub_inplace(Ciphertext &encrypted1, const Ciphertext &encrypted2) const;

        void sub(const Ciphertext &encrypted1, const Ciphertext &encrypted2, Ciphertext &destination) const;

        void multiply_inplace(
            Ciphertext &encrypted1, const Ciphertext &encrypted2,
            MemoryPoolHandle pool = MemoryManager::GetPool()) const;

        void multiply(
            const Ciphertext &encrypted1, const Ciphertext &encrypted2, Ciphertext &destination,
            MemoryPoolHandle pool = MemoryManager::GetPool()) const;

        void square_inplace(Ciphertext &encrypted, MemoryPoolHandle pool = MemoryManager::GetPool()) const;

        void square(
            const Ciphertext &encrypted, Ciphertext &destination,
            MemoryPoolHandle pool = MemoryManager::GetPool()) const;

        void relinearize_inplace(
            Ciphertext &encrypted, const RelinKeys &relin_keys,
            MemoryPoolHandle pool = MemoryManager::GetPool()) const;

        void relinearize(
            const Ciphertext &encrypted, const RelinKeys &relin_keys, Ciphertext &destination,
            MemoryPoolHandle pool = MemoryManager::GetPool()) const
        {
            destination = encrypted;
            relinearize_inplace(destination, relin_keys, std::move(pool));
        }

        void mod_switch_to_next(
            const Ciphertext &encrypted, Ciphertext &destination,
            MemoryPoolHandle pool = MemoryManager::GetPool()) const;

        void mod_switch_to_next_inplace(Ciphertext &encrypted, MemoryPoolHandle pool = MemoryManager::GetPool()) const;

        void mod_switch_to_next_inplace(Plaintext &plain) const;

        void mod_switch_to_next(const Plaintext &plain, Plaintext &destination) const
        {
            destination = plain;
            mod_switch_to_next_inplace(destination);
        }

        void mod_switch_to_inplace(
            Ciphertext &encrypted, parms_id_type parms_id, MemoryPoolHandle pool = MemoryManager::GetPool()) const;

        void mod_switch_to(
            const Ciphertext &encrypted, parms_id_type parms_id, Ciphertext &destination,
            MemoryPoolHandle pool = MemoryManager::GetPool()) const;

        void mod_switch_to_inplace(Plaintext &plain, parms_id_type parms_id) const;

        void mod_switch_to(const Plaintext &plain, parms_id_type parms_id, Plaintext &destination) const
        {
            destination = plain;
            mod_switch_to_inplace(destination, parms_id);
        }

        void rescale_to_next(
            const Ciphertext &encrypted, Ciphertext &destination,
            MemoryPoolHandle pool = MemoryManager::GetPool()) const;

        void rescale_to_next_inplace(Ciphertext &encrypted, MemoryPoolHandle pool = MemoryManager::GetPool()) const;

        void rescale_to_inplace(
            Ciphertext &encrypted, parms_id_type parms_id, MemoryPoolHandle pool = MemoryManager::GetPool()) const;

        void rescale_to(
            const Ciphertext &encrypted, parms_id_type parms_id, Ciphertext &destination,
            MemoryPoolHandle pool = MemoryManager::GetPool()) const
        {
            destination = encrypted;
            rescale_to_inplace(destination, parms_id, std::move(pool));
        }

        void multiply_many(
            const std::vector<Ciphertext> &encrypteds, const RelinKeys &relin_keys, Ciphertext &destination,
            MemoryPoolHandle pool = MemoryManager::GetPool()) const;

        void exponentiate_inplace(
            Ciphertext &encrypted, std::uint64_t exponent, const RelinKeys &relin_keys,
            MemoryPoolHandle pool = MemoryManager::GetPool()) const;

        void exponentiate(
            const Ciphertext &encrypted, std::uint64_t exponent, const RelinKeys &relin_keys,
            Ciphertext &destination, MemoryPoolHandle pool = MemoryManager::GetPool()) const
        {
            destination = encrypted;
            exponentiate_inplace(destination, exponent, relin_keys, std::move(pool));
        }

        void add_plain_inplace(Ciphertext &encrypted, const Plaintext &plain) const;

        void add_plain(const Ciphertext &encrypted, const Plaintext &plain, Ciphertext &destination) const;

        void sub_plain_inplace(Ciphertext &encrypted, const Plaintext &plain) const;

        void sub_plain(const Ciphertext &encrypted, const Plaintext &plain, Ciphertext &destination) const;

        void multiply_plain_inplace(
            Ciphertext &encrypted, const Plaintext &plain, MemoryPoolHandle pool = MemoryManager::GetPool()) const;

        void multiply_plain(
            const Ciphertext &encrypted, const Plaintext &plain, Ciphertext &destination,
            MemoryPoolHandle pool = MemoryManager::GetPool()) const;

        void transform_to_ntt_inplace(
            Plaintext &plain, parms_id_type parms_id, MemoryPoolHandle pool = MemoryManager::GetPool()) const;

        void transform_to_ntt(
            const Plaintext &plain, parms_id_type parms_id, Plaintext &destination_ntt,
            MemoryPoolHandle pool = MemoryManager::GetPool()) const
        {
            destination_ntt = plain;
            transform_to_ntt_inplace(destination_ntt, parms_id, std::move(pool));
        }

        void transform_to_ntt_inplace(Ciphertext &encrypted) const;

        void transform_to_ntt(const Ciphertext &encrypted, Ciphertext &destination_ntt) const
        {
            destination_ntt = encrypted;
            transform_to_ntt_inplace(destination_ntt);
        }

        void transform_from_ntt_inplace(Ciphertext &encrypted_ntt) const;

        void transform_from_ntt(const Ciphertext &encrypted_ntt, Ciphertext &destination) const
        {
            destination = encrypted_ntt;
            transform_from_ntt_inplace(destination);
        }

        /**
        Not supported: the clib backend exposes rotations only, not arbitrary
        Galois automorphisms. Always throws std::runtime_error.
        */
        void apply_galois_inplace(
            Ciphertext &encrypted, std::uint32_t galois_elt, const GaloisKeys &galois_keys,
            MemoryPoolHandle pool = MemoryManager::GetPool()) const;

        void apply_galois(
            const Ciphertext &encrypted, std::uint32_t galois_elt, const GaloisKeys &galois_keys,
            Ciphertext &destination, MemoryPoolHandle pool = MemoryManager::GetPool()) const
        {
            destination = encrypted;
            apply_galois_inplace(destination, galois_elt, galois_keys, std::move(pool));
        }

        void rotate_rows_inplace(
            Ciphertext &encrypted, int steps, const GaloisKeys &galois_keys,
            MemoryPoolHandle pool = MemoryManager::GetPool()) const;

        void rotate_rows(
            const Ciphertext &encrypted, int steps, const GaloisKeys &galois_keys, Ciphertext &destination,
            MemoryPoolHandle pool = MemoryManager::GetPool()) const
        {
            destination = encrypted;
            rotate_rows_inplace(destination, steps, galois_keys, std::move(pool));
        }

        void rotate_columns_inplace(
            Ciphertext &encrypted, const GaloisKeys &galois_keys,
            MemoryPoolHandle pool = MemoryManager::GetPool()) const;

        void rotate_columns(
            const Ciphertext &encrypted, const GaloisKeys &galois_keys, Ciphertext &destination,
            MemoryPoolHandle pool = MemoryManager::GetPool()) const
        {
            destination = encrypted;
            rotate_columns_inplace(destination, galois_keys, std::move(pool));
        }

        void rotate_vector_inplace(
            Ciphertext &encrypted, int steps, const GaloisKeys &galois_keys,
            MemoryPoolHandle pool = MemoryManager::GetPool()) const;

        void rotate_vector(
            const Ciphertext &encrypted, int steps, const GaloisKeys &galois_keys, Ciphertext &destination,
            MemoryPoolHandle pool = MemoryManager::GetPool()) const
        {
            destination = encrypted;
            rotate_vector_inplace(destination, steps, galois_keys, std::move(pool));
        }

        /**
        Complex conjugation has no clib counterpart; on message-backed
        ciphertexts it is simulated, on handle-backed ciphertexts it throws
        std::runtime_error.
        */
        void complex_conjugate_inplace(
            Ciphertext &encrypted, const GaloisKeys &galois_keys,
            MemoryPoolHandle pool = MemoryManager::GetPool()) const;

        void complex_conjugate(
            const Ciphertext &encrypted, const GaloisKeys &galois_keys, Ciphertext &destination,
            MemoryPoolHandle pool = MemoryManager::GetPool()) const
        {
            destination = encrypted;
            complex_conjugate_inplace(destination, galois_keys, std::move(pool));
        }

        // ---- FHE-MP-CNN SEAL extensions ----

        void add_const_inplace(Ciphertext &encrypted, double value) const;

        void add_const(const Ciphertext &encrypted, double value, Ciphertext &destination) const
        {
            destination = encrypted;
            add_const_inplace(destination, value);
        }

        void multiply_const_inplace(Ciphertext &encrypted, double value) const;

        void multiply_const(const Ciphertext &encrypted, double value, Ciphertext &destination) const
        {
            destination = encrypted;
            multiply_const_inplace(destination, value);
        }

        template <typename T>
        void multiply_vector_inplace(Ciphertext &encrypted, const std::vector<T> &value) const
        {
            multiply_vector_message_inplace(
                encrypted, std::vector<std::complex<double>>(value.begin(), value.end()));
        }

        template <typename T>
        void multiply_vector(const Ciphertext &encrypted, const std::vector<T> &value, Ciphertext &destination) const
        {
            destination = encrypted;
            multiply_vector_inplace(destination, value);
        }

        void add_inplace_reduced_error(Ciphertext &encrypted1, const Ciphertext &encrypted2) const;

        void add_reduced_error(
            const Ciphertext &encrypted1, const Ciphertext &encrypted2, Ciphertext &destination) const
        {
            if (&encrypted2 == &destination)
            {
                add_inplace_reduced_error(destination, encrypted1);
            }
            else
            {
                destination = encrypted1;
                add_inplace_reduced_error(destination, encrypted2);
            }
        }

        void double_inplace(Ciphertext &encrypted) const
        {
            add_inplace(encrypted, encrypted);
        }

        void sub_inplace_reduced_error(Ciphertext &encrypted1, const Ciphertext &encrypted2) const;

        void sub_reduced_error(
            const Ciphertext &encrypted1, const Ciphertext &encrypted2, Ciphertext &destination) const
        {
            if (&encrypted2 == &destination)
            {
                sub_inplace_reduced_error(destination, encrypted1);
            }
            else
            {
                destination = encrypted1;
                sub_inplace_reduced_error(destination, encrypted2);
            }
        }

        void multiply_inplace_reduced_error(
            Ciphertext &encrypted1, const Ciphertext &encrypted2, const RelinKeys &relin_keys) const;

        void multiply_reduced_error(
            const Ciphertext &encrypted1, const Ciphertext &encrypted2, const RelinKeys &relin_keys,
            Ciphertext &destination) const
        {
            if (&encrypted2 == &destination)
            {
                multiply_inplace_reduced_error(destination, encrypted1, relin_keys);
            }
            else
            {
                destination = encrypted1;
                multiply_inplace_reduced_error(destination, encrypted2, relin_keys);
            }
        }

        template <typename T>
        void multiply_vector_inplace_reduced_error(Ciphertext &encrypted, const std::vector<T> &value) const
        {
            multiply_vector_message_inplace(
                encrypted, std::vector<std::complex<double>>(value.begin(), value.end()));
        }

        template <typename T>
        void multiply_vector_reduced_error(
            const Ciphertext &encrypted, const std::vector<T> &value, Ciphertext &destination) const
        {
            destination = encrypted;
            multiply_vector_inplace_reduced_error(destination, value);
        }

        // ---- Shim extensions ----

        /**
        Bootstraps a ciphertext (clib::Bootstrap: Q=2 input, refreshed output).
        */
        void bootstrap_inplace(Ciphertext &encrypted) const;

        void bootstrap(const Ciphertext &encrypted, Ciphertext &destination) const
        {
            destination = encrypted;
            bootstrap_inplace(destination);
        }

    private:
        Evaluator(const Evaluator &) = delete;
        Evaluator(Evaluator &&) = delete;
        Evaluator &operator=(const Evaluator &) = delete;
        Evaluator &operator=(Evaluator &&) = delete;

        void multiply_vector_message_inplace(
            Ciphertext &encrypted, const std::vector<std::complex<double>> &value) const;

        SEALContext context_;
        CKKSEncoder &encoder_;
    };

    template <>
    inline double CKKSEncoder::from_slot<double>(const std::complex<double> &value)
    {
        return value.real();
    }

    template <>
    inline std::complex<double> CKKSEncoder::from_slot<std::complex<double>>(const std::complex<double> &value)
    {
        return value;
    }

    // -----------------------------------------------------------------------
    // clib backend session control
    // -----------------------------------------------------------------------

    /**
    Adopts a clib context that was opened by the application driver
    (clib::Run_application). While a context is adopted, seal:: objects use
    the real clib backend:

      * SEALContext operations resolve to the adopted context instead of
        opening their own (Open_context belongs to the driver);
      * Encryptor::encrypt declares each input through clib::Encrypt_input
        (recorded in export mode, loaded from ANT-encrypted material in
        compute mode);
      * Evaluator operations on ciphertexts execute the real clib kernels
        (Add_ciph / Mul_ciph / Mul_plain / Rotate_ciph / Rescale_ciph /
        Modswitch / Relin / Bootstrap / ...), with Encode_plain providing
        on-the-fly name-keyed plaintexts for host-side encoded values;
      * Ciphertext::save_output declares verifiable outputs
        (clib::Save_output) for the ANT decrypt/verify step.

    Each encryption/encode request is named deterministically from its slot
    values (and level), so the export and compute runs declare identical
    objects. A golden-model shadow of the slot values is kept alongside the
    handles: Decryptor::decrypt returns it as the plaintext reference (the
    clib backend exposes no host decryption), so save_output can pass it to
    ANT verify, which decrypts end-to-end and checks it against the
    reference.

    The message-backed golden model (no adopted context) keeps working
    unchanged for flows without a clib runtime.

    The caller must release all seal:: objects before release_clib_context
    returns and the driver closes the context.
    */
    void adopt_clib_context(clib::CONTEXT ctx);

    /**
    Detaches the session adopted with adopt_clib_context. Handles created
    during the session must already be destroyed.
    */
    void release_clib_context() noexcept;

    // Declare before SEAL objects so their destructors run before detachment.
    class ClibSession {
    public:
        explicit ClibSession(clib::CONTEXT ctx) { adopt_clib_context(ctx); }
        ~ClibSession() { release_clib_context(); }
        ClibSession(const ClibSession&) = delete;
        ClibSession& operator=(const ClibSession&) = delete;
    };


    /**
    Whether a clib context is currently adopted.
    */
    bool clib_backend_active() noexcept;
} // namespace seal

#endif // IIEDECL_SEAL_API_H
