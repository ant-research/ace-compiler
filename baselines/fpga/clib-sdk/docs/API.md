# C++ API 说明

[返回使用说明](../README.md) · [手写应用](APPLICATION.md) · [ANT 工具](ANT.md)

包含 `clib.h`，使用 `clib` 命名空间。完整声明见 [include/clib.h](../include/clib.h)，本文解释各接口的使用条件。应用无需访问句柄内部结构。

## 0. 应用驱动与离线声明

新应用通过 Run_application 执行同一份计算函数。Makefile 传入内部参数，用户只需 `return Run_application(argc, argv, Main_graph);`。

| 调用 | 作用 |
| --- | --- |
| `Run_application(argc, argv, graph)` | 创建/关闭上下文，选择导出或计算模式；成功返回 0，错误打印原因并返回 1 |
| `Encode_plain(ctx, name, values, count, q)` | 导出明文需求，或在计算模式核对声明后返回对应的 ANT PLAIN |
| `Encrypt_input(ctx, name, values, count, q)` | 导出输入加密需求，或在计算模式返回对应的 ANT CIPHER |
| `Save_output(ctx, name, x, expected, count, tolerance)` | 声明并保存输出、明文参考与绝对误差阈值，供 make verify 解密比较 |

Encode_plain/Encrypt_input 的数值必须是恰好 N/2 个有限 double，Q 为 2 到上下文 Q 之间的偶数，初始 degree 为 1。调用立即读取并校验数值，不持有调用者的缓冲区。相同类型、相同名称的声明必须具有相同数值与层级。

Save_output 的名称对应 result/name.ct，允许子目录，但不能使用绝对路径或 `.`/`..` 路径段。每个输出名称在一轮中只声明一次。参考为 N/2 个有限实数，或用 nullptr/0 表示只保存和解密。阈值必须有限且非负。名字、内容和计算记录变更后重新 make。

导出时句柄只包含元数据；N、slots、Q、Scale_degree 可查询，INFO.scale 为 NaN。计算时执行真实算子，实际 scale 由 ANT 材料决定。Encode_plain/Encrypt_input 在计算模式只加载文件，不运行 ANT。应用的数据生成/打包代码在两种模式下都会执行。

Run_application 自动管理上下文并启用 profiling。graph 内使用 New/Free、算子和 Begin_stage/End_stage；不要再调用 Open_context、Close_context、Enable_profile，或用 Load_plain/Load_ciph 代替声明接口。失败后驱动清理仍存活的对象，不提交成功标记。正常返回前用户仍需释放全部对象，遗漏会报错。

下面各节的直接加载 API 继续用于旧应用。直接模式中的 Open_context 不自动启用 profiling。

## 1. 句柄与统一调用规则

| 类型 | 含义 | 创建方式 |
| --- | --- | --- |
| `CONTEXT` | 一套已加载的参数、评估密钥和自举材料 | `Open_context` |
| `CIPHER` | 二分量密文 | `Load_ciph` 或 `New_ciph` |
| `CIPHER3` | 密文乘法产生的三分量密文 | `New_ciph3` |
| `PLAIN` | ANT 编码明文 | `Encode_plain` 或 `Load_plain` |
| `INFO` | `ring_degree`、`slots`、`q_limbs`、`scale` 四个字段 | `Context_info` / `Cipher_info` 的返回值 |

调用规则：

1. 同一进程一次只打开一个计算上下文，应用顺序调用 API；算子内部处理并行。
2. 运算对象必须属于当前上下文。输入必须已经加载或计算完成；`New_ciph` 只创建空输出，不能直接当作零密文输入。
3. 输出句柄先创建，并且与各输入句柄不同。已有输出句柄可以重复用作输出，其旧内容由库释放。
4. 加减不自动对齐层级，乘法不自动 rescale，密文乘法不自动重线性化。
5. 接口报错抛出 `std::runtime_error`，消息包含操作名称及原因。运算失败后不要读取该次输出；先处理错误或重新计算。

`Scale_degree` 返回 scale degree，常用值为 1 和 2。它不是环次数 N，也不是密文分量数。degree 1 输入相乘得到 degree 2；rescale 后回到 degree 1。

## 2. 上下文与对象管理

下表中的接口名称和参数顺序与公开头文件一致。

| 调用 | 返回/效果 |
| --- | --- |
| `Open_context(material_dir, output_dir)` | 返回 `CONTEXT`；第一参数是 `evaluation/`，第二参数是运行输出目录，必要时创建目录 |
| `Close_context(ctx)` | 关闭上下文并将调用者句柄清空；必须先释放全部对象 |
| `Context_info(ctx)` | 返回上下文的 N、slots、最大 Q 和对应 scale |
| `Cipher_info(ctx, x)` | 返回密文的 N、slots、当前 Q 和 scale |
| `Scale_degree(ctx, x)` | 返回密文 scale degree |
| `New_ciph(ctx)` / `New_ciph3(ctx)` | 返回空输出句柄 |
| `Load_ciph(ctx, path)` / `Load_plain(ctx, path)` | 从 `.ct` / `.pt` 创建并返回对象；校验参数、密钥身份或编码信息 |
| `Save_ciph(ctx, x, path)` | 保存密文；目标父目录应已存在 |
| `Free_ciph(x)` / `Free_ciph3(x)` / `Free_plain(p)` | 释放对象并清空传入的句柄变量；空句柄可重复释放 |
| `Free_ciph_array(values, count)` | 释放 CIPHER 数组中的对象并清空各元素；不释放数组本身 |

不能通过句柄赋值来复制密文数据。`b = a` 会让两个变量指向同一对象；需要独立副本时，先创建 b，再调用 `Copy_ciph(ctx, b, a)`。交换两个句柄可以使用 `std::swap`。

所有 `CIPHER3` 临时结果也必须释放。应用驱动的对象释放写法见 [新应用](../apps/new-app/main.cpp)。直接加载接口需用 try/catch 在失败时释放已创建的句柄，再关闭上下文。

## 3. 算子与层级变化

下表用 Q 表示输入的物理 Q limbs，d 表示 scale degree。所有输出都需与输入使用不同句柄。

| 调用 | 输入要求 | 输出 |
| --- | --- | --- |
| `Rotate_ciph(ctx, out, x, step)` | 非零旋转需对应评估密钥；正数左旋、负数右旋，按 slots 取模 | 相同 Q、d；step=0 相当于复制 |
| `Copy_ciph(ctx, out, x)` | 有效密文 | 独立副本，Q、d 不变 |
| `Zero_ciph(ctx, out, x)` | x 提供层级与 scale | 同层级的零值密文，Q、d 不变 |
| `Mul_Integer(ctx, out, x, value)` | value 为 int32 整数 | 各槽乘以该整数，Q、d 不变 |
| `Add_ciph(ctx, out, x, y)` / `Sub_ciph(ctx, out, x, y)` | 两输入相同 Q、d | Q、d 不变 |
| `Add_plain(ctx, out, x, p)` | 密文与明文同 Q，密文 d=1；明文由 ANT 在该层编码 | Q、d 不变 |
| `Mul_plain(ctx, out, x, p)` | 两输入同 Q，Q≥4，d=1 | 二分量密文，Q 不变，d=2 |
| `Mul_ciph(ctx, out3, x, y)` | 两输入同 Q，Q≥4，d=1 | 三分量密文 `CIPHER3`，Q 不变，d=2 |
| `Relin(ctx, out, x3)` | 有效 `CIPHER3` | 二分量密文，Q、d 不变 |
| `Rescale_ciph(ctx, out, x)` | Q≥4，d=2 | Q−2，d=1 |
| `Modswitch(ctx, out, x, target_q)` | 目标为不大于输入 Q 的偶数；d=1 时目标≥2，d=2 时目标≥4 | 目标 Q，d 不变；数值近似保持 |

`Mul_Integer` 只接受整数。乘以 0.5 等实数时，使用 ANT 编码的 `PLAIN` 和 `Mul_plain`，然后按计算安排 rescale。

两个常用计算片段如下。这里假设 ctx、输入和输出句柄都已按前文创建，输入均为 Q8、degree 1。

```cpp
// 密文 × 明文：product、result 是 CIPHER，weight 是 Q8 PLAIN。
Mul_plain(ctx, product, x, weight);  // Q8, degree 2
Rescale_ciph(ctx, result, product); // Q6, degree 1
```

```cpp
// 密文 × 密文：product3 是 CIPHER3，product2、result 是 CIPHER。
Mul_ciph(ctx, product3, x, y);      // Q8, degree 2，三分量
Relin(ctx, product2, product3);    // Q8, degree 2，二分量
Rescale_ciph(ctx, result, product2); // Q6, degree 1
```

对多个同层级乘积求和时，可以先在 degree 2 累加，最后统一 rescale。模板中的 MVM 和卷积示例采用这种安排。

## 4. Bootstrap

```cpp
uint32_t max_q = Max_bootstrap_q_limbs(ctx);
Bootstrap(ctx, out, in);         // 输出当前上下文允许的最大 Q
Bootstrap(ctx, out, in, target); // 输出指定的 Q
```

输入必须为 **Q2、degree 1**。`target` 必须是 2 到 `max_q` 之间的偶数。输出是 degree 1 的二分量密文，近似保留原槽位数值。

| 上下文 Q | 当前最大输出 Q | 示例用途 |
| ---: | ---: | --- |
| 38 | 8 | 基础 MVM/卷积后刷新 |
| 62 | 32 | 随包 ReLU 多项式及后续卷积 |

当前实现先完整执行自举，再降到指定目标；较小 target 不会减少前面的自举计算。应用通过 `Max_bootstrap_q_limbs` 查询能力。

若输入尚未到 Q2，先根据后续计算安排处理层级；例如 degree 1 的 Q4 密文可使用不同输出句柄 `Modswitch(ctx, q2, x, 2)`，再送入 Bootstrap。degree 2 密文先显式 rescale 回 degree 1。

## 5. 性能统计

在成功打开上下文后启用统计：

```cpp
Enable_profile(ctx);
Begin_stage(ctx, "conv1");
// 此处放本阶段的 API 调用。
End_stage(ctx);
Write_profile(ctx, "result");
```

| 调用 | 说明 |
| --- | --- |
| `Enable_profile(ctx)` | 启用统计并开始端到端计时；记录之前 Open_context 的独立耗时 |
| `Begin_stage(ctx, name)` | 开始命名阶段；阶段不能嵌套 |
| `End_stage(ctx)` | 结束当前阶段 |
| `Write_profile(ctx, output_dir)` | 输出 stages.csv、breakdown.csv、runtime.csv |

统计单位为秒，API 累计只计成功调用。阶段计时覆盖标记之间的全部工作；`end_to_end_without_open` 从 Enable_profile 开始，因此不包含上下文加载。查看 `runtime.csv` 可确认观察到的最大 OpenMP 团队大小，不能把它当成整个程序的平均 CPU 利用率。
