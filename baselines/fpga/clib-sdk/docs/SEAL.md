# SEAL API 转接

这是 FHE-MP-CNN 使用的 SEAL 3.6 接口子集，计算后端仍是本 SDK 的 clib。
原生应用继续只包含 `clib.h`；已有 SEAL 写法可包含 `seal/seal.h`。
两种方式都通过同一份程序导出，再由 ANT 生成参数、密钥、明文和输入密文。

## 写法

```cpp
#include <clib.h>
#include <seal/seal.h>

static void Main_graph(clib::CONTEXT ctx) {
    seal::ClibSession session(ctx); // 在所有 SEAL 对象之前创建，退出时自动解除绑定
    // 创建 SEALContext、CKKSEncoder、Encryptor、Evaluator 等对象。
    // encode/encrypt 声明离线材料；Evaluator 调用实际 clib 算子。
    // 用 Ciphertext::save_output 声明输出，最后由 ANT 解密核对。
}
int main(int argc, char** argv) {
    return clib::Run_application(argc, argv, Main_graph);
}
```

链接顺序为 `libclib_seal.a libclib.a -fopenmp -pthread -lm`。
CMake 项目可设置 `SEAL_DIR=<SDK>/cmake/SEAL`，然后使用：

```cmake
find_package(SEAL 3.6 REQUIRED CONFIG)
target_link_libraries(app PRIVATE SEAL::seal)
```

## 接口边界

- 一个进程同时使用一个 clib context，应用 API 串行调用，底层算子使用 SDK 的 OpenMP 线程组。
- 一个逻辑 rescale 消耗两个物理 Q limbs。自举出口由 ANT context 决定，当前是总 Q 减 30。
- `CoeffModulus::Create` 只产生兼容层的层级/scale 描述，不生成实际素数。真实模数以 ANT 材料为准。
- 自举精度还取决于消息系数的幅度。当前 ANT EvalMod 使用正弦近似，较大的系数会失真；仅满足 Q 层级要求不足以保证精度。FHE-MP-CNN 在 ReLU 中途自举前归一化，之后用整数乘法恢复。
- 转接层维护独立于运行模式的 scale 记录，避免导出时未知的实际 scale 导致两次运行声明不同的明文。
- `Decryptor::decrypt` 返回内部明文参考（shadow），**不解密 clib 密文**。实际解密和精度结果来自 `make` 的 ANT 验证步骤。
- 未绑定 `ClibSession` 的接口调用可能执行明文模拟。因此单独运行 SEAL 程序的时间不能当作密文性能。
- 当前密文路径用于实数槽位；复数输入转给 clib 时只取实部，不支持保留非零虚部的复数运算。
- 此接口不是完整 Microsoft SEAL；未支持的运算会报错。底层 clib 指令头文件不随接口公开。

SDK 源码仓库用 `make seal` 单独构建转接库，不重建 clib 核心。
