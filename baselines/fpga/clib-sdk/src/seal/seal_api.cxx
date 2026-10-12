// Implementation of the SEAL-compatible API declared in seal_api.h on top of
// the clib SDK (clib.h). See seal_api.h for the backend model documentation
// (handle-backed vs. message-backed objects, lazy context open, level model).

#include "seal/seal_api.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <map>
#include <stdexcept>
#include <utility>

namespace seal
{
    const parms_id_type parms_id_zero{};

    std::vector<Modulus> CoeffModulus::Create(
        std::size_t poly_modulus_degree, const std::vector<int> &bit_sizes)
    {
        (void)poly_modulus_degree;
        std::vector<Modulus> result;
        result.reserve(bit_sizes.size());
        for (int bits : bit_sizes)
        {
            if (bits <= 0 || bits > 63)
            {
                throw std::invalid_argument("CoeffModulus::Create: bit count out of range");
            }
            result.emplace_back((std::uint64_t(1) << bits) - 1);
        }
        return result;
    }

    namespace detail
    {
        /**
        Session state for the clib backend context adopted from the
        application driver (clib::Run_application). All create/encode/encrypt
        requests are named deterministically from their content, so the export
        and compute runs of the same application declare identical objects.
        */
        struct BackendSession
        {
            clib::CONTEXT ctx = nullptr;
            std::uint64_t seq = 0;
            std::map<std::pair<std::uint64_t, char>, std::string> names;

            void reset(clib::CONTEXT adopted)
            {
                ctx = adopted;
                seq = 0;
                names.clear();
            }

            /**
            Deterministic name for a (kind, level, values) request; identical
            values collide on the same name, which the backend requires.
            */
            const std::string &object_name(
                char kind, std::uint32_t q_limbs, const std::vector<double> &values)
            {
                std::uint64_t hash = 1469598103934665603ull;
                auto mix = [&hash](std::uint64_t v) {
                    for (int i = 0; i < 8; i++)
                    {
                        hash ^= (v & 0xffu);
                        hash *= 1099511628211ull;
                        v >>= 8;
                    }
                };
                mix(q_limbs);
                mix(values.size());
                for (double v : values)
                {
                    std::uint64_t bits;
                    static_assert(sizeof(bits) == sizeof(v), "double must be 64-bit");
                    std::memcpy(&bits, &v, sizeof(bits));
                    mix(bits);
                }
                const auto key = std::make_pair(hash, kind);
                const auto it = names.find(key);
                if (it != names.end())
                {
                    return it->second;
                }
                char name[32];
                std::snprintf(name, sizeof(name), "%c%04llu_%012llx", kind,
                              static_cast<unsigned long long>(seq++),
                              static_cast<unsigned long long>(hash));
                return names.emplace(key, name).first->second;
            }
        };

        inline BackendSession &backend_session()
        {
            static BackendSession session;
            return session;
        }

        /**
        Real part of the slot values (CKKS encoding of real tensors).
        */
        inline std::vector<double> to_reals(const std::vector<std::complex<double>> &values)
        {
            std::vector<double> reals(values.size());
            for (std::size_t i = 0; i < values.size(); i++)
            {
                reals[i] = values[i].real();
            }
            return reals;
        }

        /**
        Owns the clib context. The context is opened lazily on the first
        operation that needs the backend, so that pure message-mode
        (encode/encrypt/evaluate/decrypt) workflows never touch clib. A
        context adopted via adopt_clib_context is used instead of opening
        one, and never closed here.
        */
        struct ContextImpl
        {
            explicit ContextImpl(const EncryptionParameters &p) : parms(p)
            {}

            ContextImpl(const ContextImpl &) = delete;
            ContextImpl &operator=(const ContextImpl &) = delete;

            ~ContextImpl()
            {
                if (ctx != nullptr && owned_)
                {
                    try
                    {
                        clib::Close_context(ctx);
                    }
                    catch (...)
                    {}
                }
            }

            clib::CONTEXT open()
            {
                if (ctx == nullptr)
                {
                    if (backend_session().ctx != nullptr)
                    {
                        // Adopt the context opened by the application driver
                        // (clib::Run_application); Open_context/Close_context
                        // belong to the driver in that flow.
                        ctx = backend_session().ctx;
                        info = clib::Context_info(ctx);
                        owned_ = false;
                        return ctx;
                    }
                    std::string mdir = parms.material_dir();
                    if (mdir.empty())
                    {
                        const char *env = std::getenv("CLIB_MATERIAL_DIR");
                        mdir = (env != nullptr) ? env : "material";
                    }
                    std::string odir = parms.output_dir();
                    if (odir.empty())
                    {
                        const char *env = std::getenv("CLIB_OUTPUT_DIR");
                        odir = (env != nullptr) ? env : ".";
                    }
                    ctx = clib::Open_context(mdir.c_str(), odir.c_str());
                    info = clib::Context_info(ctx);
                    owned_ = true;
                }
                return ctx;
            }

            std::uint32_t top_limbs() const
            {
                if (ctx != nullptr)
                {
                    return info.q_limbs;
                }
                return static_cast<std::uint32_t>(parms.coeff_modulus().size());
            }

            std::uint32_t ring_degree() const
            {
                if (ctx != nullptr)
                {
                    return info.ring_degree;
                }
                return static_cast<std::uint32_t>(parms.poly_modulus_degree());
            }

            std::uint32_t slots() const
            {
                if (ctx != nullptr)
                {
                    return info.slots;
                }
                return static_cast<std::uint32_t>(parms.poly_modulus_degree() / 2);
            }

            EncryptionParameters parms;
            clib::CONTEXT ctx = nullptr;
            clib::INFO info{};
            bool owned_ = true;
        };

        std::weak_ptr<ContextImpl> &active_context_slot()
        {
            static std::weak_ptr<ContextImpl> slot;
            return slot;
        }

        void set_active_context(const std::shared_ptr<ContextImpl> &impl)
        {
            active_context_slot() = impl;
        }

        std::shared_ptr<ContextImpl> active_context(const char *op)
        {
            auto impl = active_context_slot().lock();
            if (!impl)
            {
                throw std::logic_error(std::string(op) + ": no active seal::SEALContext");
            }
            return impl;
        }

        // Method declarations; definitions appear after the data classes.
        struct CipherOps
        {
            static clib::CONTEXT backend(const SEALContext &context, const char *op);
            static std::vector<std::complex<double>> elementwise(
                const std::vector<std::complex<double>> &a, const std::vector<std::complex<double>> &b, int kind,
                const char *op);
            static void adopt_message(Ciphertext &dst, const Ciphertext &src);
            static void free_temp(clib::CIPHER handle) noexcept;
            static void free_temp(clib::CIPHER3 handle) noexcept;

            static void sync(const SEALContext &context, Ciphertext &ct);
            static void require_valid(const Ciphertext &ct, const char *op);
            static bool is_message(const Ciphertext &ct);
            static clib::CIPHER require_cipher2(const Ciphertext &ct, const char *op);
            static void require_same_mode(const Ciphertext &x, const Ciphertext &y, const char *op);
            static void require_same_level(const Ciphertext &x, const Ciphertext &y, const char *op);
            static double dropped_prime_product(const SEALContext &context, std::uint32_t q_limbs);

            static void add(
                const SEALContext &context, Ciphertext &dst, const Ciphertext &x, const Ciphertext &y, bool sub,
                const char *op);
            static void multiply(const SEALContext &context, Ciphertext &dst, const Ciphertext &x, const Ciphertext &y, const char *op);
            static void relin(const SEALContext &context, Ciphertext &dst, const Ciphertext &x, const char *op);
            static void rescale_next(const SEALContext &context, Ciphertext &dst, const Ciphertext &x, const char *op);
            static void modswitch(
                const SEALContext &context, Ciphertext &dst, const Ciphertext &x, std::uint32_t target_q_limbs,
                const char *op);
            static void rotate(const SEALContext &context, Ciphertext &dst, const Ciphertext &x, std::int32_t steps, const char *op);
            static void negate(const SEALContext &context, Ciphertext &dst, const Ciphertext &x, const char *op);
            static void mul_plain(const SEALContext &context, Ciphertext &dst, const Ciphertext &x, const Plaintext &p, const char *op);
            static void add_plain(
                const SEALContext &context, Ciphertext &dst, const Ciphertext &x, const Plaintext &p, bool sub,
                const char *op);
            /**
            Encodes a host-valued plaintext on the fly with clib::Encode_plain
            (name keyed on values + level), for use by the real-backend path
            when the application driver session is active.
            */
            static std::shared_ptr<clib_plain> encode_plain_otf(
                const SEALContext &context, const Plaintext &p, const Ciphertext &x, bool negate, const char *op);
            static void mul_integer(const SEALContext &context, Ciphertext &dst, const Ciphertext &x, std::int32_t value, const char *op);
            static void conjugate(const SEALContext &context, Ciphertext &dst, const Ciphertext &x, const char *op);
            static void bootstrap(const SEALContext &context, Ciphertext &dst, const Ciphertext &x, const char *op);
            static void align_levels(
                const SEALContext &context, const Ciphertext &x, const Ciphertext &y, Ciphertext &xadj,
                Ciphertext &yadj, const Ciphertext *&xp, const Ciphertext *&yp, const char *op);
        };
    } // namespace detail

    // -----------------------------------------------------------------------
    // clib backend session control
    // -----------------------------------------------------------------------

    void adopt_clib_context(clib::CONTEXT ctx)
    {
        if (ctx == nullptr)
        {
            throw std::invalid_argument("adopt_clib_context: context handle is null");
        }
        if (detail::backend_session().ctx != nullptr)
            throw std::logic_error("adopt_clib_context: a session is already active");
        detail::backend_session().reset(ctx);
    }

    void release_clib_context() noexcept
    {
        detail::backend_session().reset(nullptr);
    }

    bool clib_backend_active() noexcept
    {
        return detail::backend_session().ctx != nullptr;
    }

    // -----------------------------------------------------------------------
    // SEALContext
    // -----------------------------------------------------------------------

    SEALContext::SEALContext(const EncryptionParameters &parms, bool expand_mod_chain, sec_level_type sec_level)
        : impl_(std::make_shared<detail::ContextImpl>(parms))
    {
        (void)expand_mod_chain;
        (void)sec_level;
        std::uint32_t top = static_cast<std::uint32_t>(parms.coeff_modulus().size());
        key_parms_id_ = parms_id_type{ top };
        first_parms_id_ = parms_id_type{ top };
        last_parms_id_ = parms_id_type{ top >= 2 ? 2u : top };
        detail::set_active_context(impl_);
    }

    SEALContext::SEALContext(const SEALContext &copy) = default;

    SEALContext::SEALContext(SEALContext &&source) noexcept = default;

    SEALContext &SEALContext::operator=(const SEALContext &assign) = default;

    SEALContext &SEALContext::operator=(SEALContext &&assign) noexcept = default;

    SEALContext::~SEALContext() = default;

    SEALContext::ContextData::ContextData(std::shared_ptr<detail::ContextImpl> impl, parms_id_type parms_id)
        : impl_(std::move(impl)), parms_id_(parms_id)
    {
        std::uint32_t top = impl_->top_limbs();
        chain_index_ = (parms_id.q_limbs >= top) ? 1 : 1 + (top - parms_id.q_limbs) / 2;
    }

    const EncryptionParameters &SEALContext::ContextData::parms() const noexcept
    {
        return impl_->parms;
    }

    int SEALContext::ContextData::total_coeff_modulus_bit_count() const noexcept
    {
        int total = 0;
        const auto &mods = impl_->parms.coeff_modulus();
        std::uint32_t count = std::min<std::uint32_t>(parms_id_.q_limbs, static_cast<std::uint32_t>(mods.size()));
        for (std::uint32_t i = 0; i < count; i++)
        {
            total += mods[i].bit_count();
        }
        return total;
    }

    std::shared_ptr<const SEALContext::ContextData> SEALContext::ContextData::prev_context_data() const
    {
        std::uint32_t top = impl_->top_limbs();
        if (parms_id_.q_limbs >= top)
        {
            // First data level: the previous node is the key parameter node.
            if (chain_index_ == 0)
            {
                return nullptr;
            }
            std::shared_ptr<ContextData> key_node(new ContextData(impl_, parms_id_type{ top }));
            key_node->chain_index_ = 0;
            return key_node;
        }
        return std::shared_ptr<const ContextData>(new ContextData(impl_, parms_id_type{ parms_id_.q_limbs + 2 }));
    }

    std::shared_ptr<const SEALContext::ContextData> SEALContext::ContextData::next_context_data() const
    {
        if (chain_index_ == 0)
        {
            // Key parameter node: the next node is the first data level.
            std::uint32_t top = impl_->top_limbs();
            return std::shared_ptr<const ContextData>(new ContextData(impl_, parms_id_type{ top }));
        }
        if (parms_id_.q_limbs <= 2)
        {
            return nullptr;
        }
        return std::shared_ptr<const ContextData>(new ContextData(impl_, parms_id_type{ parms_id_.q_limbs - 2 }));
    }

    std::shared_ptr<const SEALContext::ContextData> SEALContext::get_context_data(parms_id_type parms_id) const
    {
        if (!impl_ || parms_id.q_limbs == 0 || parms_id.q_limbs > impl_->top_limbs())
        {
            return nullptr;
        }
        return std::shared_ptr<const ContextData>(new ContextData(impl_, parms_id));
    }

    std::shared_ptr<const SEALContext::ContextData> SEALContext::key_context_data() const
    {
        if (!impl_)
        {
            return nullptr;
        }
        std::shared_ptr<ContextData> key_node(new ContextData(impl_, key_parms_id_));
        key_node->chain_index_ = 0;
        return key_node;
    }

    std::shared_ptr<const SEALContext::ContextData> SEALContext::first_context_data() const
    {
        return get_context_data(first_parms_id_);
    }

    std::shared_ptr<const SEALContext::ContextData> SEALContext::last_context_data() const
    {
        return get_context_data(last_parms_id_);
    }

    bool SEALContext::parameters_set() const
    {
        return impl_ && impl_->parms.scheme() == scheme_type::ckks && impl_->parms.poly_modulus_degree() > 0 &&
            !impl_->parms.coeff_modulus().empty();
    }

    const char *SEALContext::parameter_error_name() const
    {
        if (!impl_)
        {
            return "none";
        }
        if (impl_->parms.scheme() != scheme_type::ckks)
        {
            return "invalid_scheme";
        }
        if (impl_->parms.poly_modulus_degree() == 0)
        {
            return "invalid_poly_modulus_degree";
        }
        if (impl_->parms.coeff_modulus().empty())
        {
            return "invalid_coeff_modulus_size";
        }
        return "success";
    }

    const char *SEALContext::parameter_error_message() const
    {
        if (!impl_)
        {
            return "SEALContext is empty";
        }
        if (impl_->parms.scheme() != scheme_type::ckks)
        {
            return "scheme must be CKKS for the clib backend";
        }
        if (impl_->parms.poly_modulus_degree() == 0)
        {
            return "poly_modulus_degree is not set";
        }
        if (impl_->parms.coeff_modulus().empty())
        {
            return "coeff_modulus is not set";
        }
        return "parameters are valid (clib backend)";
    }

    std::uint32_t SEALContext::top_q_limbs() const
    {
        if (!impl_)
        {
            throw std::logic_error("SEALContext::top_q_limbs: context is empty");
        }
        return impl_->top_limbs();
    }

    std::uint32_t SEALContext::slot_count() const
    {
        if (!impl_)
        {
            throw std::logic_error("SEALContext::slot_count: context is empty");
        }
        return impl_->slots();
    }

    std::uint32_t SEALContext::ring_degree() const
    {
        if (!impl_)
        {
            throw std::logic_error("SEALContext::ring_degree: context is empty");
        }
        return impl_->ring_degree();
    }

    void SEALContext::enable_profile()
    {
        if (!impl_)
        {
            throw std::logic_error("SEALContext::enable_profile: context is empty");
        }
        clib::Enable_profile(impl_->open());
    }

    void SEALContext::begin_stage(const char *name)
    {
        if (!impl_)
        {
            throw std::logic_error("SEALContext::begin_stage: context is empty");
        }
        clib::Begin_stage(impl_->open(), name);
    }

    void SEALContext::end_stage()
    {
        if (!impl_)
        {
            throw std::logic_error("SEALContext::end_stage: context is empty");
        }
        clib::End_stage(impl_->open());
    }

    void SEALContext::write_profile(const char *output_dir)
    {
        if (!impl_)
        {
            throw std::logic_error("SEALContext::write_profile: context is empty");
        }
        clib::Write_profile(impl_->open(), output_dir);
    }

    // -----------------------------------------------------------------------
    // Plaintext
    // -----------------------------------------------------------------------

    bool Plaintext::is_zero() const
    {
        if (handle_)
        {
            return false; // opaque clib plaintext
        }
        return std::all_of(values_.begin(), values_.end(), [](const std::complex<double> &v) {
            return v == std::complex<double>(0.0, 0.0);
        });
    }

    void Plaintext::release() noexcept
    {
        handle_.reset();
        values_.clear();
        values_.shrink_to_fit();
        scale_ = 1.0;
        parms_id_ = parms_id_zero;
        is_ntt_form_ = true;
    }

    void Plaintext::load(const SEALContext &context, const std::string &path)
    {
        if (!context.impl_)
        {
            throw std::invalid_argument("Plaintext::load: context is empty");
        }
        clib::PLAIN raw = clib::Load_plain(context.impl_->open(), path.c_str());
        handle_ = std::shared_ptr<clib_plain>(raw, [](clib_plain *p) {
            clib::PLAIN h = p;
            try
            {
                clib::Free_plain(h);
            }
            catch (...)
            {}
        });
        values_.clear();
        scale_ = 1.0;
        parms_id_ = parms_id_zero; // the clib plaintext level is embedded in the material
        is_ntt_form_ = true;
    }

    // -----------------------------------------------------------------------
    // Ciphertext
    // -----------------------------------------------------------------------

    void Ciphertext::free_handles() noexcept
    {
        if (ct2_ != nullptr)
        {
            try
            {
                clib::Free_ciph(ct2_);
            }
            catch (...)
            {}
            ct2_ = nullptr;
        }
        if (ct3_ != nullptr)
        {
            try
            {
                clib::Free_ciph3(ct3_);
            }
            catch (...)
            {}
            ct3_ = nullptr;
        }
    }

    void Ciphertext::install(clib::CIPHER handle)
    {
        free_handles();
        ct2_ = handle;
        shadow_.clear();
        shadow_size_ = 2;
    }

    void Ciphertext::install(clib::CIPHER3 handle)
    {
        free_handles();
        ct3_ = handle;
        shadow_.clear();
        shadow_size_ = 3;
    }

    void Ciphertext::release() noexcept
    {
        free_handles();
        shadow_.clear();
        shadow_.shrink_to_fit();
        shadow_size_ = 2;
        scale_ = 1.0;
        scale_degree_ = 1;
        parms_id_ = parms_id_zero;
        ring_degree_ = 0;
        is_ntt_form_ = true;
    }

    Ciphertext::Ciphertext(const SEALContext &context) : Ciphertext(context, context.first_parms_id(), 2)
    {}

    Ciphertext::Ciphertext(const SEALContext &context, parms_id_type parms_id)
        : Ciphertext(context, parms_id, 2)
    {}

    Ciphertext::Ciphertext(const SEALContext &context, parms_id_type parms_id, std::size_t size_capacity)
    {
        if (size_capacity < 2 || size_capacity > 3)
        {
            throw std::invalid_argument("Ciphertext: size_capacity must be 2 or 3 for the clib backend");
        }
        if (!context.impl_)
        {
            throw std::invalid_argument("Ciphertext: context is empty");
        }
        clib::CONTEXT ctx = context.impl_->open();
        if (size_capacity == 3)
        {
            install(clib::New_ciph3(ctx));
        }
        else
        {
            install(clib::New_ciph(ctx));
        }
        ring_degree_ = context.ring_degree();
        scale_ = 1.0;
        scale_degree_ = 1;
        parms_id_ = parms_id;
        // Refresh the metadata from the backend (the requested level is
        // advisory; clib allocates fresh ciphertexts at the top level).
        if (ct2_ != nullptr)
        {
            detail::CipherOps::sync(context, *this);
        }
    }

    Ciphertext::Ciphertext(const Ciphertext &copy)
    {
        *this = copy;
    }

    Ciphertext::Ciphertext(Ciphertext &&source) noexcept
    {
        *this = std::move(source);
    }

    Ciphertext &Ciphertext::operator=(const Ciphertext &assign)
    {
        if (this == &assign)
        {
            return *this;
        }
        clib::CIPHER new_handle = nullptr;
        if (assign.ct2_ != nullptr)
        {
            auto impl = detail::active_context("Ciphertext copy");
            clib::CONTEXT ctx = impl->open();
            new_handle = clib::New_ciph(ctx);
            try
            {
                clib::Copy_ciph(ctx, new_handle, assign.ct2_);
            }
            catch (...)
            {
                clib::CIPHER tmp = new_handle;
                try
                {
                    clib::Free_ciph(tmp);
                }
                catch (...)
                {}
                throw;
            }
        }
        else if (assign.ct3_ != nullptr)
        {
            throw std::logic_error(
                "Ciphertext copy: cannot copy an un-relinearized (size 3) ciphertext with the clib backend");
        }
        free_handles();
        ct2_ = new_handle;
        shadow_ = assign.shadow_;
        shadow_size_ = assign.shadow_size_;
        scale_ = assign.scale_;
        scale_degree_ = assign.scale_degree_;
        parms_id_ = assign.parms_id_;
        ring_degree_ = assign.ring_degree_;
        is_ntt_form_ = assign.is_ntt_form_;
        return *this;
    }

    Ciphertext &Ciphertext::operator=(Ciphertext &&assign) noexcept
    {
        if (this == &assign)
        {
            return *this;
        }
        free_handles();
        ct2_ = assign.ct2_;
        ct3_ = assign.ct3_;
        assign.ct2_ = nullptr;
        assign.ct3_ = nullptr;
        shadow_ = std::move(assign.shadow_);
        shadow_size_ = assign.shadow_size_;
        scale_ = assign.scale_;
        scale_degree_ = assign.scale_degree_;
        parms_id_ = assign.parms_id_;
        ring_degree_ = assign.ring_degree_;
        is_ntt_form_ = assign.is_ntt_form_;
        return *this;
    }

    Ciphertext::~Ciphertext()
    {
        free_handles();
    }

    std::size_t Ciphertext::size() const noexcept
    {
        if (ct3_ != nullptr)
        {
            return 3;
        }
        if (ct2_ != nullptr)
        {
            return 2;
        }
        return shadow_.empty() ? 0 : shadow_size_;
    }

    bool Ciphertext::is_transparent() const
    {
        if (is_handle_backed())
        {
            return false; // produced by the clib backend
        }
        return shadow_.empty() ||
            std::all_of(shadow_.begin(), shadow_.end(), [](const std::complex<double> &v) {
                return v == std::complex<double>(0.0, 0.0);
            });
    }

    void Ciphertext::reserve(const SEALContext &context, parms_id_type parms_id, std::size_t size_capacity)
    {
        (void)context;
        (void)parms_id;
        if (size_capacity < 2 || size_capacity > 3)
        {
            throw std::invalid_argument("Ciphertext::reserve: size_capacity must be 2 or 3 for the clib backend");
        }
        // No-op: clib ciphertext storage is fixed-size and managed internally.
    }

    void Ciphertext::reserve(const SEALContext &context, std::size_t size_capacity)
    {
        reserve(context, context.first_parms_id(), size_capacity);
    }

    void Ciphertext::reserve(std::size_t size_capacity)
    {
        if (size_capacity < 2 || size_capacity > 3)
        {
            throw std::invalid_argument("Ciphertext::reserve: size_capacity must be 2 or 3 for the clib backend");
        }
    }

    void Ciphertext::resize(const SEALContext &context, parms_id_type parms_id, std::size_t size)
    {
        if (size < 2 || size > 3)
        {
            throw std::invalid_argument("Ciphertext::resize: size must be 2 or 3 for the clib backend");
        }
        if (!context.impl_)
        {
            throw std::invalid_argument("Ciphertext::resize: context is empty");
        }
        if (size == this->size() && is_handle_backed())
        {
            return;
        }
        clib::CONTEXT ctx = context.impl_->open();
        if (size == 3)
        {
            install(clib::New_ciph3(ctx));
        }
        else
        {
            install(clib::New_ciph(ctx));
        }
        ring_degree_ = context.ring_degree();
        scale_ = 1.0;
        scale_degree_ = 1;
        parms_id_ = parms_id;
        if (ct2_ != nullptr)
        {
            detail::CipherOps::sync(context, *this);
        }
    }

    void Ciphertext::resize(const SEALContext &context, std::size_t size)
    {
        resize(context, context.first_parms_id(), size);
    }

    void Ciphertext::resize(std::size_t size)
    {
        if (size == this->size())
        {
            return;
        }
        throw std::logic_error(
            "Ciphertext::resize: resizing an existing ciphertext is not supported by the clib backend; "
            "use resize(context, parms_id, size)");
    }

    void Ciphertext::save(const SEALContext &context, const std::string &path) const
    {
        if (!context.impl_)
        {
            throw std::invalid_argument("Ciphertext::save: context is empty");
        }
        if (ct2_ == nullptr)
        {
            throw std::logic_error(
                "Ciphertext::save: only relinearized (size 2) clib-backed ciphertexts can be saved");
        }
        clib::Save_ciph(context.impl_->open(), ct2_, path.c_str());
    }

    void Ciphertext::load(const SEALContext &context, const std::string &path)
    {
        if (!context.impl_)
        {
            throw std::invalid_argument("Ciphertext::load: context is empty");
        }
        clib::CONTEXT ctx = context.impl_->open();
        install(clib::Load_ciph(ctx, path.c_str()));
        detail::CipherOps::sync(context, *this);
    }

    bool Ciphertext::save_output(const char *name, double abs_tolerance) const
    {
        detail::BackendSession &session = detail::backend_session();
        if (session.ctx == nullptr)
        {
            return false; // golden-model run: nothing to declare
        }
        if (ct2_ == nullptr)
        {
            if (ct3_ != nullptr)
            {
                throw std::logic_error("save_output: relinearize the (size 3) ciphertext first");
            }
            throw std::logic_error("save_output: ciphertext is empty");
        }
        if (!(abs_tolerance >= 0.0) || !std::isfinite(abs_tolerance))
        {
            throw std::invalid_argument("save_output: tolerance must be finite and non-negative");
        }
        // The golden shadow doubles as the plaintext reference; where no
        // mirror is available the output is saved for decryption only.
        std::vector<double> reals = detail::to_reals(shadow_);
        const double *expected = nullptr;
        std::size_t count = 0;
        if (!reals.empty())
        {
            expected = reals.data();
            count = reals.size();
        }
        clib::Save_output(session.ctx, name, ct2_, expected, count, abs_tolerance);
        return true;
    }

    // -----------------------------------------------------------------------
    // detail::CipherOps — operation kernels for both backend modes
    // -----------------------------------------------------------------------

    namespace detail
    {
        clib::CONTEXT CipherOps::backend(const SEALContext &context, const char *op)
        {
            if (!context.impl_)
            {
                throw std::invalid_argument(std::string(op) + ": context is empty");
            }
            return context.impl_->open();
        }

        // Slot-wise combination of two message vectors. A size-1 vector is
        // broadcast; a shorter vector is zero-padded.
        std::vector<std::complex<double>> CipherOps::elementwise(
            const std::vector<std::complex<double>> &a, const std::vector<std::complex<double>> &b, int kind,
            const char *op)
        {
            if (a.empty() || b.empty())
            {
                throw std::invalid_argument(std::string(op) + ": operand has no message values");
            }
            const std::size_t count = std::max(a.size(), b.size());
            std::vector<std::complex<double>> out(count);
            for (std::size_t i = 0; i < count; i++)
            {
                const std::complex<double> av = (a.size() == 1) ? a[0] : (i < a.size() ? a[i] : 0.0);
                const std::complex<double> bv = (b.size() == 1) ? b[0] : (i < b.size() ? b[i] : 0.0);
                switch (kind)
                {
                case 0: out[i] = av + bv; break;
                case 1: out[i] = av - bv; break;
                default: out[i] = av * bv; break;
                }
            }
            return out;
        }

        // Copies message-mode state (and metadata) from src to dst when they
        // are distinct objects.
        void CipherOps::adopt_message(Ciphertext &dst, const Ciphertext &src)
        {
            if (&dst == &src)
            {
                return;
            }
            dst.free_handles();
            dst.shadow_ = src.shadow_;
            dst.shadow_size_ = src.shadow_size_;
            dst.scale_ = src.scale_;
            dst.scale_degree_ = src.scale_degree_;
            dst.parms_id_ = src.parms_id_;
            dst.ring_degree_ = src.ring_degree_;
            dst.is_ntt_form_ = src.is_ntt_form_;
        }

        void CipherOps::free_temp(clib::CIPHER handle) noexcept
        {
            try
            {
                clib::Free_ciph(handle);
            }
            catch (...)
            {}
        }

        void CipherOps::free_temp(clib::CIPHER3 handle) noexcept
        {
            try
            {
                clib::Free_ciph3(handle);
            }
            catch (...)
            {}
        }

        void CipherOps::sync(const SEALContext &context, Ciphertext &ct)
        {
            clib::INFO info = clib::Cipher_info(context.impl_->open(), ct.ct2_);
            // The backend scale is deliberately not read: INFO.scale is NaN in
            // export mode and ANT-determined in compute mode, while the
            // application's scale-dependent encode values must stay
            // identical across the export and compute runs. The shim keeps
            // its own deterministic SEAL-style scale bookkeeping instead
            // (level/degree/ring do come from the backend).
            ct.parms_id_.q_limbs = info.q_limbs;
            ct.ring_degree_ = info.ring_degree;
            ct.scale_degree_ = clib::Scale_degree(context.impl_->ctx, ct.ct2_);
            ct.is_ntt_form_ = true;
        }

        void CipherOps::require_valid(const Ciphertext &ct, const char *op)
        {
            if (ct.ct2_ == nullptr && ct.ct3_ == nullptr && ct.shadow_.empty())
            {
                throw std::invalid_argument(std::string(op) + ": ciphertext is empty");
            }
        }

        bool CipherOps::is_message(const Ciphertext &ct)
        {
            return ct.ct2_ == nullptr && ct.ct3_ == nullptr;
        }

        clib::CIPHER CipherOps::require_cipher2(const Ciphertext &ct, const char *op)
        {
            if (ct.ct2_ == nullptr)
            {
                throw std::invalid_argument(
                    std::string(op) + ": requires a relinearized (size 2) clib-backed ciphertext");
            }
            return ct.ct2_;
        }

        void CipherOps::require_same_mode(const Ciphertext &x, const Ciphertext &y, const char *op)
        {
            if (is_message(x) != is_message(y))
            {
                throw std::runtime_error(
                    std::string(op) + ": cannot mix clib-backed and message-backed ciphertexts");
            }
        }

        void CipherOps::require_same_level(const Ciphertext &x, const Ciphertext &y, const char *op)
        {
            if (x.parms_id_.q_limbs != y.parms_id_.q_limbs)
            {
                throw std::invalid_argument(
                    std::string(op) +
                    ": ciphertexts are at different levels (match levels with mod_switch or use the "
                    "*_reduced_error variants)");
            }
        }

        double CipherOps::dropped_prime_product(const SEALContext &context, std::uint32_t q_limbs)
        {
            // One level drop consumes 2 Q limbs (clib.h); the scale is divided
            // by the product of the two topmost remaining primes.
            double prod = 1.0;
            if (context.impl_)
            {
                const auto &mods = context.impl_->parms.coeff_modulus();
                for (std::uint32_t i = 0; i < 2; i++)
                {
                    if (q_limbs > i && q_limbs - 1 - i < mods.size())
                    {
                        prod *= static_cast<double>(mods[q_limbs - 1 - i].value());
                    }
                }
            }
            return prod;
        }

        void CipherOps::add(
            const SEALContext &context, Ciphertext &dst, const Ciphertext &x, const Ciphertext &y, bool sub,
            const char *op)
        {
            require_valid(x, op);
            require_valid(y, op);
            require_same_mode(x, y, op);
            require_same_level(x, y, op);
            if (is_message(x))
            {
                if (x.shadow_size_ != y.shadow_size_)
                {
                    throw std::invalid_argument(std::string(op) + ": ciphertext sizes do not match");
                }
                std::vector<std::complex<double>> values = elementwise(x.shadow_, y.shadow_, sub ? 1 : 0, op);
                adopt_message(dst, x);
                dst.shadow_ = std::move(values);
                return;
            }
            clib::CONTEXT ctx = backend(context, op);
            const bool reuse = (&dst != &x && &dst != &y && dst.ct2_ != nullptr);
            clib::CIPHER out = reuse ? dst.ct2_ : clib::New_ciph(ctx);
            try
            {
                if (sub)
                {
                    clib::Sub_ciph(ctx, out, require_cipher2(x, op), require_cipher2(y, op));
                }
                else
                {
                    clib::Add_ciph(ctx, out, require_cipher2(x, op), require_cipher2(y, op));
                }
            }
            catch (...)
            {
                if (!reuse)
                {
                    free_temp(out);
                }
                throw;
            }
            if (!reuse)
            {
                dst.install(out);
            }
            sync(context, dst);
            dst.scale_ = x.scale_;
            if (!x.shadow_.empty())
            {
                // Golden-model mirror of the slot values.
                dst.shadow_ = elementwise(x.shadow_, y.shadow_, sub ? 1 : 0, op);
            }
        }

        void CipherOps::multiply(
            const SEALContext &context, Ciphertext &dst, const Ciphertext &x, const Ciphertext &y, const char *op)
        {
            require_valid(x, op);
            require_valid(y, op);
            require_same_mode(x, y, op);
            require_same_level(x, y, op);
            if (is_message(x))
            {
                std::vector<std::complex<double>> values = elementwise(x.shadow_, y.shadow_, 2, op);
                adopt_message(dst, x);
                dst.shadow_ = std::move(values);
                dst.shadow_size_ = x.shadow_size_ + y.shadow_size_ - 1;
                dst.scale_ = x.scale_ * y.scale_;
                dst.scale_degree_ = x.scale_degree_ + y.scale_degree_;
                return;
            }
            clib::CONTEXT ctx = backend(context, op);
            clib::CIPHER3 out = clib::New_ciph3(ctx);
            try
            {
                clib::Mul_ciph(ctx, out, require_cipher2(x, op), require_cipher2(y, op));
            }
            catch (...)
            {
                free_temp(out);
                throw;
            }
            const double scale = x.scale_ * y.scale_;
            const std::uint32_t scale_degree = x.scale_degree_ + y.scale_degree_;
            const parms_id_type level = x.parms_id_;
            const std::uint32_t ring = (x.ring_degree_ != 0) ? x.ring_degree_ : y.ring_degree_;
            dst.install(out);
            // Cipher_info/Scale_degree only accept degree-1 (CIPHER) handles,
            // so the size-3 metadata is derived from the inputs.
            dst.scale_ = scale;
            dst.scale_degree_ = scale_degree;
            dst.parms_id_ = level;
            dst.ring_degree_ = ring;
            dst.is_ntt_form_ = true;
            if (!x.shadow_.empty())
            {
                // Golden-model mirror of the slot values.
                dst.shadow_ = elementwise(x.shadow_, y.shadow_, 2, op);
            }
        }

        void CipherOps::relin(const SEALContext &context, Ciphertext &dst, const Ciphertext &x, const char *op)
        {
            require_valid(x, op);
            if (is_message(x))
            {
                adopt_message(dst, x);
                dst.shadow_size_ = 2;
                return;
            }
            if (x.ct3_ == nullptr)
            {
                // Already relinearized (size 2): a no-op, as in SEAL.
                if (&dst != &x)
                {
                    dst = x;
                }
                return;
            }
            clib::CONTEXT ctx = backend(context, op);
            clib::CIPHER out = clib::New_ciph(ctx);
            try
            {
                clib::Relin(ctx, out, x.ct3_);
            }
            catch (...)
            {
                free_temp(out);
                throw;
            }
            dst.install(out);
            sync(context, dst);
            dst.scale_ = x.scale_;
            if (!x.shadow_.empty())
            {
                // Golden-model mirror: relinearization preserves the slots.
                dst.shadow_ = x.shadow_;
            }
        }

        void CipherOps::rescale_next(const SEALContext &context, Ciphertext &dst, const Ciphertext &x, const char *op)
        {
            require_valid(x, op);
            if (is_message(x))
            {
                if (x.parms_id_.q_limbs <= 2)
                {
                    throw std::invalid_argument(std::string(op) + ": ciphertext is already at the lowest level");
                }
                const double dropped = dropped_prime_product(context, x.parms_id_.q_limbs);
                adopt_message(dst, x);
                dst.parms_id_.q_limbs -= 2;
                if (dropped > 1.0)
                {
                    dst.scale_ /= dropped;
                }
                if (dst.scale_degree_ > 1)
                {
                    dst.scale_degree_ -= 1;
                }
                return;
            }
            clib::CONTEXT ctx = backend(context, op);
            clib::CIPHER out = clib::New_ciph(ctx);
            try
            {
                clib::Rescale_ciph(ctx, out, require_cipher2(x, op));
            }
            catch (...)
            {
                free_temp(out);
                throw;
            }
            dst.install(out);
            sync(context, dst);
            // Deterministic bookkeeping: a rescale also divides the scale
            // by the product of the dropped primes (as the message branch,
            // which the scale-dependent encode values are designed against).
            const double dropped = dropped_prime_product(context, x.parms_id_.q_limbs);
            dst.scale_ = (dropped > 1.0) ? x.scale_ / dropped : x.scale_;
            if (!x.shadow_.empty())
            {
                // Golden-model mirror: the slots are unchanged by a rescale.
                dst.shadow_ = x.shadow_;
            }
        }

        void CipherOps::modswitch(
            const SEALContext &context, Ciphertext &dst, const Ciphertext &x, std::uint32_t target_q_limbs,
            const char *op)
        {
            require_valid(x, op);
            if (target_q_limbs > x.parms_id_.q_limbs)
            {
                throw std::invalid_argument(std::string(op) + ": target level is above the ciphertext level");
            }
            if (target_q_limbs == x.parms_id_.q_limbs)
            {
                if (&dst != &x)
                {
                    dst = x;
                }
                return;
            }
            if (target_q_limbs < 2 || (target_q_limbs % 2) != 0)
            {
                throw std::invalid_argument(
                    std::string(op) + ": target Q-limb count must be even and at least 2 for the clib backend");
            }
            if (is_message(x))
            {
                adopt_message(dst, x);
                dst.parms_id_.q_limbs = target_q_limbs;
                return;
            }
            clib::CONTEXT ctx = backend(context, op);
            clib::CIPHER out = clib::New_ciph(ctx);
            try
            {
                clib::Modswitch(ctx, out, require_cipher2(x, op), target_q_limbs);
            }
            catch (...)
            {
                free_temp(out);
                throw;
            }
            dst.install(out);
            sync(context, dst);
            dst.scale_ = x.scale_;
            if (!x.shadow_.empty())
            {
                // Golden-model mirror: the slots are unchanged by a mod switch.
                dst.shadow_ = x.shadow_;
            }
        }

        void CipherOps::rotate(
            const SEALContext &context, Ciphertext &dst, const Ciphertext &x, std::int32_t steps, const char *op)
        {
            require_valid(x, op);
            if (is_message(x))
            {
                adopt_message(dst, x);
                const std::size_t n = dst.shadow_.size();
                if (n > 1)
                {
                    const std::int64_t period = static_cast<std::int64_t>(n);
                    const std::int64_t shift = ((static_cast<std::int64_t>(steps) % period) + period) % period;
                    // Positive steps rotate the slot vector cyclically to the
                    // left, matching SEAL rotate_vector and clib::Rotate_ciph.
                    std::rotate(dst.shadow_.begin(), dst.shadow_.begin() + shift, dst.shadow_.end());
                }
                return;
            }
            clib::CONTEXT ctx = backend(context, op);
            clib::CIPHER out = clib::New_ciph(ctx);
            try
            {
                clib::Rotate_ciph(ctx, out, require_cipher2(x, op), steps);
            }
            catch (...)
            {
                free_temp(out);
                throw;
            }
            dst.install(out);
            sync(context, dst);
            dst.scale_ = x.scale_;
            if (!x.shadow_.empty() && x.shadow_.size() > 1)
            {
                // Golden-model mirror: positive steps rotate the slot vector
                // cyclically to the left, as in SEAL rotate_vector.
                const std::int64_t period = static_cast<std::int64_t>(x.shadow_.size());
                const std::int64_t shift = ((static_cast<std::int64_t>(steps) % period) + period) % period;
                dst.shadow_ = x.shadow_;
                std::rotate(dst.shadow_.begin(), dst.shadow_.begin() + shift, dst.shadow_.end());
            }
        }

        void CipherOps::negate(const SEALContext &context, Ciphertext &dst, const Ciphertext &x, const char *op)
        {
            require_valid(x, op);
            if (is_message(x))
            {
                adopt_message(dst, x);
                for (auto &v : dst.shadow_)
                {
                    v = -v;
                }
                return;
            }
            // clib has no negation primitive: out = 0 - x.
            clib::CONTEXT ctx = backend(context, op);
            clib::CIPHER zero = clib::New_ciph(ctx);
            clib::CIPHER out = clib::New_ciph(ctx);
            try
            {
                clib::Zero_ciph(ctx, zero, require_cipher2(x, op));
                clib::Sub_ciph(ctx, out, zero, require_cipher2(x, op));
            }
            catch (...)
            {
                free_temp(zero);
                free_temp(out);
                throw;
            }
            free_temp(zero);
            dst.install(out);
            sync(context, dst);
            dst.scale_ = x.scale_;
            if (!x.shadow_.empty())
            {
                // Golden-model mirror of the slot values.
                dst.shadow_ = x.shadow_;
                for (auto &v : dst.shadow_)
                {
                    v = -v;
                }
            }
        }

        std::shared_ptr<clib_plain> CipherOps::encode_plain_otf(
            const SEALContext &context, const Plaintext &p, const Ciphertext &x, bool negate, const char *op)
        {
            BackendSession &session = backend_session();
            if (session.ctx == nullptr || p.handle_ || p.values_.empty())
            {
                return nullptr;
            }
            // clib Mul_plain / Add_plain require the plaintext Q to match the
            // ciphertext Q; the SEAL-style encode level (plain.parms_id_) is
            // only advisory when it disagrees.
            std::uint32_t q = x.parms_id_.q_limbs;
            if (q < 2 || (q % 2) != 0)
            {
                throw std::invalid_argument(std::string(op) + ": ciphertext is not at a valid clib level");
            }
            std::vector<double> reals = to_reals(p.values_);
            // Zero-pad to the full slot count, exactly as a SEAL encode does.
            const std::size_t slots = context.slot_count();
            if (reals.size() > slots)
            {
                throw std::invalid_argument(std::string(op) + ": plaintext has more values than slots");
            }
            reals.resize(slots, 0.0);
            if (negate)
            {
                // clib has no Sub_plain; x - p is computed as x + (-p).
                for (double &v : reals)
                {
                    v = -v;
                }
            }
            const std::string name = session.object_name('p', q, reals);
            clib::PLAIN raw = clib::Encode_plain(session.ctx, name.c_str(), reals.data(), reals.size(), q);
            return std::shared_ptr<clib_plain>(raw, [](clib_plain *h) {
                clib::PLAIN handle = h;
                try
                {
                    clib::Free_plain(handle);
                }
                catch (...)
                {}
            });
        }

        void CipherOps::mul_plain(
            const SEALContext &context, Ciphertext &dst, const Ciphertext &x, const Plaintext &p, const char *op)
        {
            require_valid(x, op);
            if (is_message(x))
            {
                if (p.handle_)
                {
                    throw std::runtime_error(
                        std::string(op) + ": cannot mix a clib-backed plaintext with a message-backed ciphertext");
                }
                std::vector<std::complex<double>> values = elementwise(x.shadow_, p.values_, 2, op);
                adopt_message(dst, x);
                dst.shadow_ = std::move(values);
                dst.scale_ *= p.scale_;
                dst.scale_degree_ += 1;
                return;
            }
            std::shared_ptr<clib_plain> temp = (!p.handle_) ? encode_plain_otf(context, p, x, false, op) : nullptr;
            const clib::PLAIN plain = (temp != nullptr) ? temp.get() : p.handle_.get();
            if (plain == nullptr)
            {
                throw std::runtime_error(
                    std::string(op) +
                    ": plaintext is not backed by the clib backend and no backend session is active; "
                    "encode it offline and load it with Plaintext::load");
            }
            clib::CONTEXT ctx = backend(context, op);
            clib::CIPHER out = clib::New_ciph(ctx);
            try
            {
                clib::Mul_plain(ctx, out, require_cipher2(x, op), plain);
            }
            catch (...)
            {
                free_temp(out);
                throw;
            }
            dst.install(out);
            sync(context, dst);
            dst.scale_ = x.scale_ * p.scale_;
            if (!x.shadow_.empty())
            {
                // Golden-model mirror of the slot values.
                dst.shadow_ = elementwise(x.shadow_, p.values_, 2, op);
            }
        }

        void CipherOps::add_plain(
            const SEALContext &context, Ciphertext &dst, const Ciphertext &x, const Plaintext &p, bool sub,
            const char *op)
        {
            require_valid(x, op);
            if (is_message(x))
            {
                if (p.handle_)
                {
                    throw std::runtime_error(
                        std::string(op) + ": cannot mix a clib-backed plaintext with a message-backed ciphertext");
                }
                std::vector<std::complex<double>> values = elementwise(x.shadow_, p.values_, sub ? 1 : 0, op);
                adopt_message(dst, x);
                dst.shadow_ = std::move(values);
                return;
            }
            if (p.handle_ && sub)
            {
                throw std::runtime_error(
                    std::string(op) + ": the clib backend does not support ciphertext-minus-plaintext");
            }
            std::shared_ptr<clib_plain> temp = (!p.handle_) ? encode_plain_otf(context, p, x, sub, op) : nullptr;
            const clib::PLAIN plain = (temp != nullptr) ? temp.get() : p.handle_.get();
            if (plain == nullptr)
            {
                throw std::runtime_error(
                    std::string(op) +
                    ": plaintext is not backed by the clib backend and no backend session is active; "
                    "encode it offline and load it with Plaintext::load");
            }
            clib::CONTEXT ctx = backend(context, op);
            clib::CIPHER out = clib::New_ciph(ctx);
            try
            {
                clib::Add_plain(ctx, out, require_cipher2(x, op), plain);
            }
            catch (...)
            {
                free_temp(out);
                throw;
            }
            dst.install(out);
            sync(context, dst);
            dst.scale_ = x.scale_;
            if (!x.shadow_.empty())
            {
                // Golden-model mirror of the slot values.
                dst.shadow_ = elementwise(x.shadow_, p.values_, sub ? 1 : 0, op);
            }
        }

        void CipherOps::mul_integer(
            const SEALContext &context, Ciphertext &dst, const Ciphertext &x, std::int32_t value, const char *op)
        {
            require_valid(x, op);
            if (is_message(x))
            {
                adopt_message(dst, x);
                const std::complex<double> factor(static_cast<double>(value), 0.0);
                for (auto &v : dst.shadow_)
                {
                    v *= factor;
                }
                return;
            }
            clib::CONTEXT ctx = backend(context, op);
            clib::CIPHER out = clib::New_ciph(ctx);
            try
            {
                clib::Mul_Integer(ctx, out, require_cipher2(x, op), value);
            }
            catch (...)
            {
                free_temp(out);
                throw;
            }
            dst.install(out);
            sync(context, dst);
            dst.scale_ = x.scale_;
            if (!x.shadow_.empty())
            {
                // Golden-model mirror of the slot values.
                dst.shadow_ = x.shadow_;
                const std::complex<double> factor(static_cast<double>(value), 0.0);
                for (auto &v : dst.shadow_)
                {
                    v *= factor;
                }
            }
        }

        void CipherOps::conjugate(const SEALContext &context, Ciphertext &dst, const Ciphertext &x, const char *op)
        {
            (void)context;
            require_valid(x, op);
            if (is_message(x))
            {
                adopt_message(dst, x);
                for (auto &v : dst.shadow_)
                {
                    v = std::conj(v);
                }
                return;
            }
            throw std::runtime_error(
                std::string(op) + ": the clib backend does not expose complex conjugation");
        }

        void CipherOps::bootstrap(const SEALContext &context, Ciphertext &dst, const Ciphertext &x, const char *op)
        {
            require_valid(x, op);
            if (is_message(x))
            {
                if (x.parms_id_.q_limbs != 0 && x.parms_id_.q_limbs != 2)
                {
                    throw std::invalid_argument(
                        std::string(op) + ": input must be at the lowest level (2 Q limbs)");
                }
                adopt_message(dst, x);
                // The native ANT bootstrap consumes 30 physical limbs.
                if (context.top_q_limbs() < 32)
                    throw std::invalid_argument("bootstrap requires at least Q32");
                dst.parms_id_.q_limbs = context.top_q_limbs() - 30;
                dst.scale_degree_ = 1;
                return;
            }
            clib::CONTEXT ctx = backend(context, op);
            clib::CIPHER out = clib::New_ciph(ctx);
            try
            {
                clib::Bootstrap(ctx, out, require_cipher2(x, op));
            }
            catch (...)
            {
                free_temp(out);
                throw;
            }
            dst.install(out);
            sync(context, dst);
            dst.scale_ = x.scale_;
            if (!x.shadow_.empty())
            {
                // Golden-model mirror: the slots are (approximately) preserved
                // by the bootstrap; the mirror tracks the exact input values.
                dst.shadow_ = x.shadow_;
            }
        }

        void CipherOps::align_levels(
            const SEALContext &context, const Ciphertext &x, const Ciphertext &y, Ciphertext &xadj,
            Ciphertext &yadj, const Ciphertext *&xp, const Ciphertext *&yp, const char *op)
        {
            xp = &x;
            yp = &y;
            if (x.parms_id_.q_limbs > y.parms_id_.q_limbs)
            {
                modswitch(context, xadj, x, y.parms_id_.q_limbs, op);
                xp = &xadj;
            }
            else if (y.parms_id_.q_limbs > x.parms_id_.q_limbs)
            {
                modswitch(context, yadj, y, x.parms_id_.q_limbs, op);
                yp = &yadj;
            }
        }
    } // namespace detail

    // -----------------------------------------------------------------------
    // CKKSEncoder
    // -----------------------------------------------------------------------

    CKKSEncoder::CKKSEncoder(const SEALContext &context) : context_(context)
    {
        if (!context.parameters_set())
        {
            throw std::invalid_argument("CKKSEncoder: encryption parameters are not valid");
        }
        slots_ = context.slot_count();
        sparse_slots_ = slots_;
    }

    void CKKSEncoder::encode_message(
        const std::vector<std::complex<double>> &values, parms_id_type parms_id, double scale,
        Plaintext &destination)
    {
        if (values.size() > slots_)
        {
            throw std::invalid_argument("CKKSEncoder::encode: values has invalid size");
        }
        if (!(scale > 0.0))
        {
            throw std::invalid_argument("CKKSEncoder::encode: scale must be strictly positive");
        }
        // Zero-pad to the full slot count so that rotations have the correct
        // period, as in SEAL.
        destination.handle_.reset();
        destination.values_.assign(slots_, std::complex<double>(0.0, 0.0));
        std::copy(values.begin(), values.end(), destination.values_.begin());
        destination.scale_ = scale;
        destination.parms_id_ = (parms_id.q_limbs != 0) ? parms_id : context_.first_parms_id();
        destination.is_ntt_form_ = true;
    }

    void CKKSEncoder::encode_broadcast(
        std::complex<double> value, parms_id_type parms_id, double scale, Plaintext &destination)
    {
        if (!(scale > 0.0))
        {
            throw std::invalid_argument("CKKSEncoder::encode: scale must be strictly positive");
        }
        destination.handle_.reset();
        destination.values_.assign(slots_, value);
        destination.scale_ = scale;
        destination.parms_id_ = (parms_id.q_limbs != 0) ? parms_id : context_.first_parms_id();
        destination.is_ntt_form_ = true;
    }

    void CKKSEncoder::encode(std::int64_t value, parms_id_type parms_id, Plaintext &destination)
    {
        // Integer encoding carries no scale, as in SEAL.
        encode_broadcast(std::complex<double>(static_cast<double>(value), 0.0), parms_id, 1.0, destination);
    }

    void CKKSEncoder::decode_message(const Plaintext &plain, std::vector<std::complex<double>> &destination) const
    {
        if (plain.handle_)
        {
            throw std::runtime_error(
                "CKKSEncoder::decode: plaintext is clib-backed; the clib backend does not expose host decoding");
        }
        if (plain.values_.empty())
        {
            throw std::invalid_argument("CKKSEncoder::decode: plaintext has no message values");
        }
        const std::size_t count = std::min(sparse_slots_, plain.values_.size());
        destination.assign(plain.values_.begin(), plain.values_.begin() + count);
    }

    // -----------------------------------------------------------------------
    // Encryptor
    // -----------------------------------------------------------------------

    Encryptor::Encryptor(const SEALContext &context, const PublicKey &public_key) : context_(context)
    {
        (void)public_key;
        if (!context.parameters_set())
        {
            throw std::invalid_argument("Encryptor: encryption parameters are not valid");
        }
    }

    Encryptor::Encryptor(const SEALContext &context, const SecretKey &secret_key) : context_(context)
    {
        (void)secret_key;
        if (!context.parameters_set())
        {
            throw std::invalid_argument("Encryptor: encryption parameters are not valid");
        }
    }

    Encryptor::Encryptor(const SEALContext &context, const PublicKey &public_key, const SecretKey &secret_key)
        : context_(context)
    {
        (void)public_key;
        (void)secret_key;
        if (!context.parameters_set())
        {
            throw std::invalid_argument("Encryptor: encryption parameters are not valid");
        }
    }

    void Encryptor::encrypt(const Plaintext &plain, Ciphertext &destination, MemoryPoolHandle pool) const
    {
        (void)pool;
        if (plain.handle_)
        {
            throw std::runtime_error(
                "Encryptor::encrypt: cannot re-encrypt a clib-backed plaintext; "
                "load input ciphertexts with Ciphertext::load");
        }
        if (plain.values_.empty())
        {
            throw std::invalid_argument("Encryptor::encrypt: plaintext has no message values");
        }
        destination.free_handles();
        destination.shadow_ = plain.values_;
        destination.shadow_size_ = 2;
        destination.scale_ = plain.scale_;
        destination.scale_degree_ = 1;
        destination.parms_id_ = (plain.parms_id_.q_limbs != 0) ? plain.parms_id_ : context_.first_parms_id();
        destination.ring_degree_ = context_.ring_degree();
        destination.is_ntt_form_ = true;
        if (clib_backend_active())
        {
            // Real backend path: declare the encrypted input. In the export
            // run Encrypt_input records (name, values, level); in the compute
            // run it loads the ANT-encrypted ciphertext for the same name.
            detail::BackendSession &session = detail::backend_session();
            const std::uint32_t q = destination.parms_id_.q_limbs;
            if (q < 2 || (q % 2) != 0 || q > context_.top_q_limbs())
            {
                throw std::invalid_argument("Encryptor::encrypt: plaintext level is not a valid clib level");
            }
            std::vector<double> reals = detail::to_reals(plain.values_);
            if (reals.size() != context_.slot_count())
            {
                reals.resize(context_.slot_count(), 0.0);
            }
            const std::string name = session.object_name('e', q, reals);
            clib::CIPHER handle = clib::Encrypt_input(
                session.ctx, name.c_str(), reals.data(), reals.size(), q);
            destination.install(handle);
            detail::CipherOps::sync(context_, destination);
            // Keep the golden shadow: decrypt returns it as the plaintext
            // reference; the clib backend itself has no host decryption.
            destination.shadow_ = plain.values_;
            destination.shadow_size_ = 2;
        }
    }

    void Encryptor::encrypt_zero(Ciphertext &destination, MemoryPoolHandle pool) const
    {
        encrypt_zero(context_.first_parms_id(), destination, std::move(pool));
    }

    void Encryptor::encrypt_zero(parms_id_type parms_id, Ciphertext &destination, MemoryPoolHandle pool) const
    {
        (void)pool;
        if (clib_backend_active())
        {
            throw std::runtime_error(
                "Encryptor::encrypt_zero: the clib backend has no zero-encryption; "
                "encrypt an all-zero plaintext instead");
        }
        destination.free_handles();
        destination.shadow_.assign(context_.slot_count(), std::complex<double>(0.0, 0.0));
        destination.shadow_size_ = 2;
        destination.scale_ = 1.0;
        destination.scale_degree_ = 1;
        destination.parms_id_ = (parms_id.q_limbs != 0) ? parms_id : context_.first_parms_id();
        destination.ring_degree_ = context_.ring_degree();
        destination.is_ntt_form_ = true;
    }

    void Encryptor::encrypt_symmetric(const Plaintext &plain, Ciphertext &destination, MemoryPoolHandle pool) const
    {
        encrypt(plain, destination, std::move(pool));
    }

    void Encryptor::encrypt_zero_symmetric(Ciphertext &destination, MemoryPoolHandle pool) const
    {
        encrypt_zero(context_.first_parms_id(), destination, std::move(pool));
    }

    void Encryptor::encrypt_zero_symmetric(
        parms_id_type parms_id, Ciphertext &destination, MemoryPoolHandle pool) const
    {
        encrypt_zero(parms_id, destination, std::move(pool));
    }

    // -----------------------------------------------------------------------
    // Decryptor
    // -----------------------------------------------------------------------

    Decryptor::Decryptor(const SEALContext &context, const SecretKey &secret_key) : context_(context)
    {
        (void)secret_key;
        if (!context.parameters_set())
        {
            throw std::invalid_argument("Decryptor: encryption parameters are not valid");
        }
    }

    void Decryptor::decrypt(const Ciphertext &encrypted, Plaintext &destination)
    {
        if (encrypted.ct2_ != nullptr || encrypted.ct3_ != nullptr)
        {
            if (encrypted.shadow_.empty())
            {
                throw std::runtime_error(
                    "Decryptor::decrypt: the clib backend is evaluation-only and cannot decrypt; "
                    "declare the ciphertext with Ciphertext::save_output so the ANT verify step "
                    "can decrypt it offline");
            }
            // The clib backend has no host decryption. Return the golden-model
            // shadow of the slot values: the application can use them as the
            // plaintext reference; the real decrypted values are produced by
            // the ANT verify step (which also checks them against this
            // reference where one was declared).
            destination.handle_.reset();
            destination.values_ = encrypted.shadow_;
            destination.scale_ = encrypted.scale_;
            destination.parms_id_ = encrypted.parms_id_;
            destination.is_ntt_form_ = encrypted.is_ntt_form_;
            return;
        }
        if (encrypted.shadow_.empty())
        {
            throw std::invalid_argument("Decryptor::decrypt: ciphertext is empty");
        }
        destination.handle_.reset();
        destination.values_ = encrypted.shadow_;
        destination.scale_ = encrypted.scale_;
        destination.parms_id_ = encrypted.parms_id_;
        destination.is_ntt_form_ = encrypted.is_ntt_form_;
    }

    int Decryptor::invariant_noise_budget(const Ciphertext &encrypted)
    {
        (void)encrypted;
        throw std::runtime_error("Decryptor::invariant_noise_budget: not supported by the clib backend");
    }

    // -----------------------------------------------------------------------
    // Evaluator
    // -----------------------------------------------------------------------

    Evaluator::Evaluator(const SEALContext &context, CKKSEncoder &encoder)
        : context_(context), encoder_(encoder)
    {
        if (!context.parameters_set())
        {
            throw std::invalid_argument("Evaluator: encryption parameters are not valid");
        }
    }

    void Evaluator::negate_inplace(Ciphertext &encrypted) const
    {
        Ciphertext result;
        detail::CipherOps::negate(context_, result, encrypted, "negate_inplace");
        encrypted = std::move(result);
    }

    void Evaluator::add_inplace(Ciphertext &encrypted1, const Ciphertext &encrypted2) const
    {
        Ciphertext result;
        detail::CipherOps::add(context_, result, encrypted1, encrypted2, false, "add_inplace");
        encrypted1 = std::move(result);
    }

    void Evaluator::add(const Ciphertext &encrypted1, const Ciphertext &encrypted2, Ciphertext &destination) const
    {
        Ciphertext result;
        detail::CipherOps::add(context_, result, encrypted1, encrypted2, false, "add");
        destination = std::move(result);
    }

    void Evaluator::add_many(const std::vector<Ciphertext> &encrypteds, Ciphertext &destination) const
    {
        if (encrypteds.empty())
        {
            throw std::invalid_argument("add_many: encrypteds is empty");
        }
        Ciphertext acc = encrypteds[0];
        for (std::size_t i = 1; i < encrypteds.size(); i++)
        {
            Ciphertext next;
            detail::CipherOps::add(context_, next, acc, encrypteds[i], false, "add_many");
            acc = std::move(next);
        }
        destination = std::move(acc);
    }

    void Evaluator::sub_inplace(Ciphertext &encrypted1, const Ciphertext &encrypted2) const
    {
        Ciphertext result;
        detail::CipherOps::add(context_, result, encrypted1, encrypted2, true, "sub_inplace");
        encrypted1 = std::move(result);
    }

    void Evaluator::sub(const Ciphertext &encrypted1, const Ciphertext &encrypted2, Ciphertext &destination) const
    {
        Ciphertext result;
        detail::CipherOps::add(context_, result, encrypted1, encrypted2, true, "sub");
        destination = std::move(result);
    }

    void Evaluator::multiply_inplace(Ciphertext &encrypted1, const Ciphertext &encrypted2, MemoryPoolHandle pool) const
    {
        (void)pool;
        Ciphertext result;
        detail::CipherOps::multiply(context_, result, encrypted1, encrypted2, "multiply_inplace");
        encrypted1 = std::move(result);
    }

    void Evaluator::multiply(
        const Ciphertext &encrypted1, const Ciphertext &encrypted2, Ciphertext &destination,
        MemoryPoolHandle pool) const
    {
        (void)pool;
        Ciphertext result;
        detail::CipherOps::multiply(context_, result, encrypted1, encrypted2, "multiply");
        destination = std::move(result);
    }

    void Evaluator::square_inplace(Ciphertext &encrypted, MemoryPoolHandle pool) const
    {
        (void)pool;
        Ciphertext result;
        detail::CipherOps::multiply(context_, result, encrypted, encrypted, "square_inplace");
        encrypted = std::move(result);
    }

    void Evaluator::square(const Ciphertext &encrypted, Ciphertext &destination, MemoryPoolHandle pool) const
    {
        destination = encrypted;
        square_inplace(destination, std::move(pool));
    }

    void Evaluator::relinearize_inplace(Ciphertext &encrypted, const RelinKeys &relin_keys, MemoryPoolHandle pool) const
    {
        (void)relin_keys;
        (void)pool;
        Ciphertext result;
        detail::CipherOps::relin(context_, result, encrypted, "relinearize_inplace");
        encrypted = std::move(result);
    }

    void Evaluator::mod_switch_to_next(const Ciphertext &encrypted, Ciphertext &destination, MemoryPoolHandle pool) const
    {
        destination = encrypted;
        mod_switch_to_next_inplace(destination, std::move(pool));
    }

    void Evaluator::mod_switch_to_next_inplace(Ciphertext &encrypted, MemoryPoolHandle pool) const
    {
        (void)pool;
        detail::CipherOps::require_valid(encrypted, "mod_switch_to_next_inplace");
        const std::uint32_t q = encrypted.parms_id_.q_limbs;
        if (q <= 2)
        {
            throw std::invalid_argument("mod_switch_to_next_inplace: ciphertext is already at the lowest level");
        }
        Ciphertext result;
        detail::CipherOps::modswitch(context_, result, encrypted, q - 2, "mod_switch_to_next_inplace");
        encrypted = std::move(result);
    }

    void Evaluator::mod_switch_to_next_inplace(Plaintext &plain) const
    {
        if (plain.handle_)
        {
            return; // the level of a clib plaintext is embedded in the material
        }
        if (plain.parms_id_.q_limbs <= 2)
        {
            throw std::invalid_argument("mod_switch_to_next_inplace: plaintext is already at the lowest level");
        }
        plain.parms_id_.q_limbs -= 2;
    }

    void Evaluator::mod_switch_to_inplace(Ciphertext &encrypted, parms_id_type parms_id, MemoryPoolHandle pool) const
    {
        (void)pool;
        Ciphertext result;
        detail::CipherOps::modswitch(context_, result, encrypted, parms_id.q_limbs, "mod_switch_to_inplace");
        encrypted = std::move(result);
    }

    void Evaluator::mod_switch_to(
        const Ciphertext &encrypted, parms_id_type parms_id, Ciphertext &destination, MemoryPoolHandle pool) const
    {
        destination = encrypted;
        mod_switch_to_inplace(destination, parms_id, std::move(pool));
    }

    void Evaluator::mod_switch_to_inplace(Plaintext &plain, parms_id_type parms_id) const
    {
        if (plain.handle_)
        {
            return; // the level of a clib plaintext is embedded in the material
        }
        if (parms_id.q_limbs > plain.parms_id_.q_limbs)
        {
            throw std::invalid_argument("mod_switch_to_inplace: target level is above the plaintext level");
        }
        plain.parms_id_.q_limbs = parms_id.q_limbs;
    }

    void Evaluator::rescale_to_next(const Ciphertext &encrypted, Ciphertext &destination, MemoryPoolHandle pool) const
    {
        (void)pool;
        Ciphertext result;
        detail::CipherOps::rescale_next(context_, result, encrypted, "rescale_to_next");
        destination = std::move(result);
    }

    void Evaluator::rescale_to_next_inplace(Ciphertext &encrypted, MemoryPoolHandle pool) const
    {
        (void)pool;
        Ciphertext result;
        detail::CipherOps::rescale_next(context_, result, encrypted, "rescale_to_next_inplace");
        encrypted = std::move(result);
    }

    void Evaluator::rescale_to_inplace(Ciphertext &encrypted, parms_id_type parms_id, MemoryPoolHandle pool) const
    {
        (void)pool;
        detail::CipherOps::require_valid(encrypted, "rescale_to_inplace");
        const std::uint32_t target = parms_id.q_limbs;
        if (target > encrypted.parms_id_.q_limbs)
        {
            throw std::invalid_argument("rescale_to_inplace: target level is above the ciphertext level");
        }
        if ((encrypted.parms_id_.q_limbs - target) % 2 != 0 || target < 2)
        {
            throw std::invalid_argument(
                "rescale_to_inplace: target level must be at least 2 Q limbs and an even number of limbs below "
                "the ciphertext level");
        }
        Ciphertext result;
        detail::CipherOps::adopt_message(result, encrypted);
        if (!detail::CipherOps::is_message(encrypted))
        {
            result = encrypted;
        }
        while (result.parms_id_.q_limbs > target)
        {
            Ciphertext next;
            detail::CipherOps::rescale_next(context_, next, result, "rescale_to_inplace");
            result = std::move(next);
        }
        encrypted = std::move(result);
    }

    void Evaluator::multiply_many(
        const std::vector<Ciphertext> &encrypteds, const RelinKeys &relin_keys, Ciphertext &destination,
        MemoryPoolHandle pool) const
    {
        (void)pool;
        if (encrypteds.empty())
        {
            throw std::invalid_argument("multiply_many: encrypteds is empty");
        }
        Ciphertext acc = encrypteds[0];
        for (std::size_t i = 1; i < encrypteds.size(); i++)
        {
            Ciphertext product;
            detail::CipherOps::multiply(context_, product, acc, encrypteds[i], "multiply_many");
            Ciphertext next;
            detail::CipherOps::relin(context_, next, product, "multiply_many");
            acc = std::move(next);
        }
        (void)relin_keys;
        destination = std::move(acc);
    }

    void Evaluator::exponentiate_inplace(
        Ciphertext &encrypted, std::uint64_t exponent, const RelinKeys &relin_keys, MemoryPoolHandle pool) const
    {
        (void)pool;
        if (exponent == 0)
        {
            throw std::invalid_argument("exponentiate_inplace: exponent is zero");
        }
        Ciphertext base = encrypted;
        Ciphertext result;
        bool have_result = false;
        std::uint64_t e = exponent;
        while (e != 0)
        {
            if ((e & 1) != 0)
            {
                if (!have_result)
                {
                    result = base;
                    have_result = true;
                }
                else
                {
                    Ciphertext product;
                    detail::CipherOps::multiply(context_, product, result, base, "exponentiate_inplace");
                    Ciphertext next;
                    detail::CipherOps::relin(context_, next, product, "exponentiate_inplace");
                    result = std::move(next);
                }
            }
            e >>= 1;
            if (e != 0)
            {
                Ciphertext sq;
                detail::CipherOps::multiply(context_, sq, base, base, "exponentiate_inplace");
                Ciphertext next;
                detail::CipherOps::relin(context_, next, sq, "exponentiate_inplace");
                base = std::move(next);
            }
        }
        (void)relin_keys;
        encrypted = std::move(result);
    }

    void Evaluator::add_plain_inplace(Ciphertext &encrypted, const Plaintext &plain) const
    {
        Ciphertext result;
        detail::CipherOps::add_plain(context_, result, encrypted, plain, false, "add_plain_inplace");
        encrypted = std::move(result);
    }

    void Evaluator::add_plain(const Ciphertext &encrypted, const Plaintext &plain, Ciphertext &destination) const
    {
        Ciphertext result;
        detail::CipherOps::add_plain(context_, result, encrypted, plain, false, "add_plain");
        destination = std::move(result);
    }

    void Evaluator::sub_plain_inplace(Ciphertext &encrypted, const Plaintext &plain) const
    {
        Ciphertext result;
        detail::CipherOps::add_plain(context_, result, encrypted, plain, true, "sub_plain_inplace");
        encrypted = std::move(result);
    }

    void Evaluator::sub_plain(const Ciphertext &encrypted, const Plaintext &plain, Ciphertext &destination) const
    {
        Ciphertext result;
        detail::CipherOps::add_plain(context_, result, encrypted, plain, true, "sub_plain");
        destination = std::move(result);
    }

    void Evaluator::multiply_plain_inplace(Ciphertext &encrypted, const Plaintext &plain, MemoryPoolHandle pool) const
    {
        (void)pool;
        Ciphertext result;
        detail::CipherOps::mul_plain(context_, result, encrypted, plain, "multiply_plain_inplace");
        encrypted = std::move(result);
    }

    void Evaluator::multiply_plain(
        const Ciphertext &encrypted, const Plaintext &plain, Ciphertext &destination, MemoryPoolHandle pool) const
    {
        (void)pool;
        Ciphertext result;
        detail::CipherOps::mul_plain(context_, result, encrypted, plain, "multiply_plain");
        destination = std::move(result);
    }

    void Evaluator::transform_to_ntt_inplace(Plaintext &plain, parms_id_type parms_id, MemoryPoolHandle pool) const
    {
        (void)pool;
        if (!plain.handle_ && plain.parms_id_.q_limbs != 0 && parms_id.q_limbs > plain.parms_id_.q_limbs)
        {
            throw std::invalid_argument("transform_to_ntt_inplace: parms_id is above the plaintext level");
        }
        if (parms_id.q_limbs != 0)
        {
            plain.parms_id_ = parms_id;
        }
        plain.is_ntt_form_ = true;
    }

    void Evaluator::transform_to_ntt_inplace(Ciphertext &encrypted) const
    {
        detail::CipherOps::require_valid(encrypted, "transform_to_ntt_inplace");
        // clib ciphertexts are internally always in NTT form; this only
        // updates the shim bookkeeping flag.
        encrypted.is_ntt_form_ = true;
    }

    void Evaluator::transform_from_ntt_inplace(Ciphertext &encrypted_ntt) const
    {
        detail::CipherOps::require_valid(encrypted_ntt, "transform_from_ntt_inplace");
        // Bookkeeping only: the clib backend keeps ciphertexts in NTT form
        // internally regardless of this flag.
        encrypted_ntt.is_ntt_form_ = false;
    }

    void Evaluator::apply_galois_inplace(
        Ciphertext &encrypted, std::uint32_t galois_elt, const GaloisKeys &galois_keys, MemoryPoolHandle pool) const
    {
        (void)encrypted;
        (void)galois_elt;
        (void)galois_keys;
        (void)pool;
        throw std::runtime_error(
            "apply_galois: the clib backend exposes rotations only; use rotate_vector instead");
    }

    void Evaluator::rotate_rows_inplace(
        Ciphertext &encrypted, int steps, const GaloisKeys &galois_keys, MemoryPoolHandle pool) const
    {
        (void)encrypted;
        (void)steps;
        (void)galois_keys;
        (void)pool;
        throw std::logic_error("rotate_rows_inplace: unsupported scheme (row rotation requires BFV)");
    }

    void Evaluator::rotate_columns_inplace(
        Ciphertext &encrypted, const GaloisKeys &galois_keys, MemoryPoolHandle pool) const
    {
        (void)encrypted;
        (void)galois_keys;
        (void)pool;
        throw std::logic_error("rotate_columns_inplace: unsupported scheme (column rotation requires BFV)");
    }

    void Evaluator::rotate_vector_inplace(
        Ciphertext &encrypted, int steps, const GaloisKeys &galois_keys, MemoryPoolHandle pool) const
    {
        (void)galois_keys;
        (void)pool;
        Ciphertext result;
        detail::CipherOps::rotate(context_, result, encrypted, steps, "rotate_vector_inplace");
        encrypted = std::move(result);
    }

    void Evaluator::complex_conjugate_inplace(
        Ciphertext &encrypted, const GaloisKeys &galois_keys, MemoryPoolHandle pool) const
    {
        (void)galois_keys;
        (void)pool;
        Ciphertext result;
        detail::CipherOps::conjugate(context_, result, encrypted, "complex_conjugate_inplace");
        encrypted = std::move(result);
    }

    void Evaluator::add_const_inplace(Ciphertext &encrypted, double value) const
    {
        detail::CipherOps::require_valid(encrypted, "add_const_inplace");
        if (detail::CipherOps::is_message(encrypted))
        {
            const std::complex<double> addend(value, 0.0);
            for (auto &v : encrypted.shadow_)
            {
                v += addend;
            }
            return;
        }
        // clib backend: encode the broadcast constant at the ciphertext level
        // (clib::Encode_plain via the backend session) and add it.
        Plaintext constant;
        constant.values_.assign(context_.slot_count(), std::complex<double>(value, 0.0));
        constant.parms_id_ = encrypted.parms_id_;
        constant.is_ntt_form_ = true;
        Ciphertext result;
        detail::CipherOps::add_plain(context_, result, encrypted, constant, false, "add_const_inplace");
        encrypted = std::move(result);
    }

    void Evaluator::multiply_const_inplace(Ciphertext &encrypted, double value) const
    {
        detail::CipherOps::require_valid(encrypted, "multiply_const_inplace");
        if (detail::CipherOps::is_message(encrypted))
        {
            const double old_scale = encrypted.scale_;
            const std::complex<double> factor(value, 0.0);
            for (auto &v : encrypted.shadow_)
            {
                v *= factor;
            }
            // Mirrors the modified SEAL: the constant is encoded at the
            // ciphertext scale, so the product carries scale^2.
            encrypted.scale_ = old_scale * old_scale;
            encrypted.scale_degree_ += 1;
            return;
        }
        if (std::floor(value) == value && std::fabs(value) <= 2147483647.0)
        {
            // Note: clib::Mul_Integer preserves the scale metadata, whereas
            // the modified SEAL's multiply_const produces a squared scale.
            Ciphertext result;
            detail::CipherOps::mul_integer(
                context_, result, encrypted, static_cast<std::int32_t>(value), "multiply_const_inplace");
            encrypted = std::move(result);
            return;
        }
        // clib backend: encode the broadcast constant at the ciphertext level
        // (clib::Encode_plain via the backend session) and multiply by it.
        Plaintext constant;
        constant.values_.assign(context_.slot_count(), std::complex<double>(value, 0.0));
        constant.parms_id_ = encrypted.parms_id_;
        constant.is_ntt_form_ = true;
        Ciphertext result;
        detail::CipherOps::mul_plain(context_, result, encrypted, constant, "multiply_const_inplace");
        encrypted = std::move(result);
    }

    void Evaluator::multiply_vector_message_inplace(
        Ciphertext &encrypted, const std::vector<std::complex<double>> &value) const
    {
        detail::CipherOps::require_valid(encrypted, "multiply_vector_inplace");
        if (detail::CipherOps::is_message(encrypted))
        {
            std::vector<std::complex<double>> values =
                detail::CipherOps::elementwise(encrypted.shadow_, value, 2, "multiply_vector_inplace");
            const double old_scale = encrypted.scale_;
            encrypted.shadow_ = std::move(values);
            encrypted.scale_ = old_scale * old_scale;
            encrypted.scale_degree_ += 1;
            return;
        }
        // clib backend: encode the vector at the ciphertext level. The SEAL
        // semantics broadcast a size-1 vector and zero-pad shorter vectors.
        Plaintext constant;
        if (value.size() == 1)
        {
            constant.values_.assign(context_.slot_count(), value[0]);
        }
        else
        {
            constant.values_ = value;
            constant.values_.resize(context_.slot_count(), std::complex<double>(0.0, 0.0));
        }
        constant.parms_id_ = encrypted.parms_id_;
        constant.is_ntt_form_ = true;
        Ciphertext result;
        detail::CipherOps::mul_plain(context_, result, encrypted, constant, "multiply_vector_inplace");
        encrypted = std::move(result);
    }

    void Evaluator::add_inplace_reduced_error(Ciphertext &encrypted1, const Ciphertext &encrypted2) const
    {
        detail::CipherOps::require_valid(encrypted1, "add_inplace_reduced_error");
        detail::CipherOps::require_valid(encrypted2, "add_inplace_reduced_error");
        detail::CipherOps::require_same_mode(encrypted1, encrypted2, "add_inplace_reduced_error");
        Ciphertext xadj, yadj;
        const Ciphertext *xp = nullptr, *yp = nullptr;
        detail::CipherOps::align_levels(
            context_, encrypted1, encrypted2, xadj, yadj, xp, yp, "add_inplace_reduced_error");
        Ciphertext result;
        detail::CipherOps::add(context_, result, *xp, *yp, false, "add_inplace_reduced_error");
        // As in the modified SEAL, the destination adopts encrypted2's scale.
        result.scale_ = yp->scale_;
        encrypted1 = std::move(result);
    }

    void Evaluator::sub_inplace_reduced_error(Ciphertext &encrypted1, const Ciphertext &encrypted2) const
    {
        detail::CipherOps::require_valid(encrypted1, "sub_inplace_reduced_error");
        detail::CipherOps::require_valid(encrypted2, "sub_inplace_reduced_error");
        detail::CipherOps::require_same_mode(encrypted1, encrypted2, "sub_inplace_reduced_error");
        Ciphertext xadj, yadj;
        const Ciphertext *xp = nullptr, *yp = nullptr;
        detail::CipherOps::align_levels(
            context_, encrypted1, encrypted2, xadj, yadj, xp, yp, "sub_inplace_reduced_error");
        Ciphertext result;
        detail::CipherOps::add(context_, result, *xp, *yp, true, "sub_inplace_reduced_error");
        result.scale_ = yp->scale_;
        encrypted1 = std::move(result);
    }

    void Evaluator::multiply_inplace_reduced_error(
        Ciphertext &encrypted1, const Ciphertext &encrypted2, const RelinKeys &relin_keys) const
    {
        (void)relin_keys;
        detail::CipherOps::require_valid(encrypted1, "multiply_inplace_reduced_error");
        detail::CipherOps::require_valid(encrypted2, "multiply_inplace_reduced_error");
        detail::CipherOps::require_same_mode(encrypted1, encrypted2, "multiply_inplace_reduced_error");
        Ciphertext xadj, yadj;
        const Ciphertext *xp = nullptr, *yp = nullptr;
        detail::CipherOps::align_levels(
            context_, encrypted1, encrypted2, xadj, yadj, xp, yp, "multiply_inplace_reduced_error");
        Ciphertext product;
        detail::CipherOps::multiply(context_, product, *xp, *yp, "multiply_inplace_reduced_error");
        Ciphertext result;
        detail::CipherOps::relin(context_, result, product, "multiply_inplace_reduced_error");
        encrypted1 = std::move(result);
    }

    void Evaluator::bootstrap_inplace(Ciphertext &encrypted) const
    {
        Ciphertext result;
        detail::CipherOps::bootstrap(context_, result, encrypted, "bootstrap_inplace");
        encrypted = std::move(result);
    }

} // namespace seal
