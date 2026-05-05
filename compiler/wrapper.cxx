#include <cstdint>
#include <cstddef>

extern "C" {


//! @brief Gets the major version number of the library
//! @return Major version number
int32_t Get_library_major_version() {
    return 1;
}

/**
 * @brief 获取库的次版本号  
 * @return 次版本号
 */
int32_t Get_library_minor_version() {
    return 0;
}

/**
 * @brief 获取库的修订版本号
 * @return 修订版本号
 */
int32_t Get_library_patch_version() {
    return 0;
}

/**
 * @brief 获取完整的版本字符串
 * @return 版本字符串（格式：MAJOR.MINOR.PATCH）
 */
const char* Get_library_version_string() {
    return "1.0.0";
}

/**
 * @brief 获取库的构建时间戳
 * @return ISO 8601 格式的构建时间
 */
const char* Get_library_build_timestamp() {
    return __DATE__ " " __TIME__;
}

/**
 * @brief 获取库的描述信息
 * @return 库的描述字符串
 */
const char* Get_library_description() {
    return "Wrapper library for multiple static archives";
}

/**
 * @brief 获取库的编译器信息
 * @return 编译器信息字符串
 */
const char* Get_library_compiler_info() {
#if defined(__clang__)
    return "Clang " __clang_version__;
#elif defined(__GNUC__)
    return "GCC " __VERSION__;
#elif defined(_MSC_VER)
    return "MSVC " _MSC_FULL_VER_STR;
#else
    return "Unknown compiler";
#endif
}

/**
 * @brief 获取库的构建类型
 * @return "Debug" 或 "Release"
 */
const char* Get_library_build_type() {
#ifdef NDEBUG
    return "Release";
#else
    return "Debug";
#endif
}

/**
 * @brief 健康检查函数 - 验证库是否正确加载
 * @return 总是返回 1（表示健康）
 */
int32_t Library_health_check() {
    return 1;
}

/**
 * @brief 获取库支持的功能标志
 * @return 功能标志位掩码
 */
uint64_t Get_library_feature_flags() {
    // 可以根据实际功能设置不同的位
    uint64_t flags = 0;
    // flags |= (1ULL << 0); // Feature A enabled
    // flags |= (1ULL << 1); // Feature B enabled
    return flags;
}

// 声明外部函数
int Test_fhe_cmplr3(int argc, char* argv[]);

// 这行代码强制链接器包含 Test_fhe_cmplr3
// 创建一个实际调用的函数（不是只是取地址）
__attribute__((constructor))
static void ensure_test_fhe_cmplr3_linked() {
    // 实际调用（即使参数是 dummy）
    volatile int result = Test_fhe_cmplr3(0, nullptr);
    (void)result;
}

} // extern "C"

// extern "C" __attribute__((visibility("default"))) 
// int Test_fhe_cmplr3(int argc, char* argv[]);