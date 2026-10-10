# 跑起来

在解压后的 SDK 目录中执行：

```bash
cd apps/new-app
make
```

首次运行会自动导出需求、准备 ANT 上下文、编码和加密、执行计算、解密验证。看到 `PASS mvm` 和 `PASS bootstrap` 就通过了。结果在 `result/`，时间单位为秒。

再次执行 `make` 会复用未改变的上下文和编码材料。

# 写自己的应用

只改两个文件：

- **main.cpp**：写输入和权重的槽位打包，以及旋转、乘法、自举等计算。`Encode_plain` 声明明文，`Encrypt_input` 声明输入，`Save_output` 指定输出和验证参考。
- **config.mk**：设置 N、Q 和原始数据位置。将应用复制到 SDK 外时，在这里把 `SDK_DIR` 改为 SDK 的实际路径。

改完执行 `make`。不用写 Python、JSON、编码清单，也不用设置环境变量或维护 param.hh。`work/export/encode.inc` 是 main.cpp 自动导出的文件，不要手改。

模板计算三个循环对角线的 MVM，然后 Bootstrap 到 Q6。输入和权重为 Q4，乘积累加后一次 rescale 到 Q2。换公式时，同步修改 main.cpp 中的明文参考。

需要分步运行时：

```bash
make export    # 导出 main.cpp 的明文、输入、旋转和计算需求
make context   # ANT 准备上下文
make encode    # ANT 编码和加密
make build     # 只编译
make run       # 计算
make verify    # 计算并解密验证
```

每一步自动补齐依赖。`make clean` 只删除编译产物；`make distclean` 还会删除本应用生成的密钥、材料和结果。

完整开发规则见 SDK 中的 `docs/APPLICATION.md`，函数条件见 `docs/API.md`。
