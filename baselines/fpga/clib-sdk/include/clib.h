#ifndef CLIB_PUBLIC_H
#define CLIB_PUBLIC_H
#ifndef __cplusplus
#error "This SDK exposes a C++17 API. Compile the application with a C++ compiler."
#endif
#include <stdint.h>
#include <stddef.h>
// Forward declarations only: no implementation, layout or instruction headers.
struct clib_context;
struct clib_cipher;
struct clib_cipher3;
struct clib_plain;
namespace clib {
// Opaque handles. Definitions and error handling live in libclib.a.
struct INFO { uint32_t ring_degree, slots, q_limbs; double scale; };
using CONTEXT = clib_context*;
using CIPHER = clib_cipher*;
using CIPHER3 = clib_cipher3*;
using PLAIN = clib_plain*;
// C++ calls throw std::runtime_error with operation name and error detail.
// One active context, serial API calls; expensive kernels use a 32-thread team.
// Distinct output/input; positive rotation is left. Runtime N: 1024/8192/65536.
// Application driver: export the same graph, then load ANT artifacts at run time.
// Makefile supplies the internal arguments. No ANT process runs inside the graph.
int Run_application(int argc, char** argv, void (*graph)(CONTEXT));
PLAIN Encode_plain(CONTEXT ctx, const char* name, const double* values,
                   size_t count, uint32_t q_limbs);
CIPHER Encrypt_input(CONTEXT ctx, const char* name, const double* values,
                     size_t count, uint32_t q_limbs);
// expected=nullptr/count=0 saves without numerical comparison.
void Save_output(CONTEXT ctx, const char* name, CIPHER value,
                 const double* expected, size_t count, double abs_tolerance);
// In export mode INFO.scale is NaN; use Q and Scale_degree to schedule operations.
CONTEXT Open_context(const char* material_dir, const char* output_dir);
void Close_context(CONTEXT& ctx);
INFO Context_info(CONTEXT ctx);
INFO Cipher_info(CONTEXT ctx, CIPHER x);
uint32_t Scale_degree(CONTEXT ctx, CIPHER x);
CIPHER New_ciph(CONTEXT ctx);
CIPHER3 New_ciph3(CONTEXT ctx);
CIPHER Load_ciph(CONTEXT ctx, const char* path);
PLAIN Load_plain(CONTEXT ctx, const char* path);
void Save_ciph(CONTEXT ctx, CIPHER x, const char* path);
// Null-safe; on success these clear the caller's handle. Free before close.
void Free_ciph(CIPHER& x);
void Free_ciph3(CIPHER3& x);
void Free_plain(PLAIN& x);
void Free_ciph_array(CIPHER* values, size_t count);
// One operation per call: no implicit alignment, relinearization or rescale.
// Mul_ciph/Mul_plain: same level, degree-1 inputs, >=4 Q limbs; output degree 2.
// Relin preserves level/degree. Rescale_ciph consumes 2 Q limbs, degree 2 -> 1.
// Modswitch: even target <= input, >=2 limbs for degree 1 / >=4 for degree 2.
// Bootstrap: Q=2, degree 1 input. Output: even physical Q limbs in [2, max].
// A smaller target drops limbs after the full bootstrap; it does not skip work.
void Rotate_ciph(CONTEXT ctx, CIPHER out, CIPHER x, int32_t step);
void Mul_plain(CONTEXT ctx, CIPHER out, CIPHER x, PLAIN p);
void Add_plain(CONTEXT ctx, CIPHER out, CIPHER x, PLAIN p);
void Add_ciph(CONTEXT ctx, CIPHER out, CIPHER x, CIPHER y);
void Sub_ciph(CONTEXT ctx, CIPHER out, CIPHER x, CIPHER y);
void Mul_ciph(CONTEXT ctx, CIPHER3 out, CIPHER x, CIPHER y);
void Relin(CONTEXT ctx, CIPHER out, CIPHER3 x);
void Rescale_ciph(CONTEXT ctx, CIPHER out, CIPHER x);
void Modswitch(CONTEXT ctx, CIPHER out, CIPHER x, uint32_t q_limbs);
void Mul_Integer(CONTEXT ctx, CIPHER out, CIPHER x, int32_t value);
void Copy_ciph(CONTEXT ctx, CIPHER out, CIPHER x);
void Zero_ciph(CONTEXT ctx, CIPHER out, CIPHER x);
uint32_t Max_bootstrap_q_limbs(CONTEXT ctx);
void Bootstrap(CONTEXT ctx, CIPHER out, CIPHER x, uint32_t output_q_limbs);
// Default: maximum output Q limbs for the loaded context.
void Bootstrap(CONTEXT ctx, CIPHER out, CIPHER x);
// Optional profiling of C++ calls only. Enable immediately after open.
// Successful calls only; open is recorded from Open_context. Units: seconds.
// Stage wall time includes all work between Begin_stage and End_stage.
// End-to-end starts at Enable_profile; open is excluded. No nested stages.
void Enable_profile(CONTEXT ctx);
void Begin_stage(CONTEXT ctx, const char* name);
void End_stage(CONTEXT ctx);
void Write_profile(CONTEXT ctx, const char* output_dir);
} // namespace clib
#endif
