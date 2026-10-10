# ANT 工具：参数、密钥、编码与加解密

[返回使用说明](../README.md) · [手写应用](APPLICATION.md) · [API](API.md)

用户通常只执行 `make`，不需要直接操作本页的底层命令。计算程序读取 ANT 生成的文件，不调用 ANT，也不读取 private/。

## Makefile 自动调用的离线流程

| 步骤 | ANT 工具入口 | 输入 | 输出 |
| --- | --- | --- | --- |
| make context | application-context | work/export/encode.inc 中 N、Q 和旋转需求 | ANT evaluation/private 上下文，保存在 work/context/ |
| make encode | application-encode | 自动生成的需求与原始槽位值、ANT 上下文 | .pt/.ct、encoded.inc 身份记录，保存在 work/encoded/ |
| make verify | application-verify | 本轮成功输出、对应 private、导出的参考 | 解密 JSON、全部槽位误差和 PASS/FAIL |

make export 先运行同一份 main.cpp 的导出模式，不需要 ANT 参数、密钥或输入密文。ANT 编码在 context 准备之后执行，使用该上下文的实际模数和 scale。正常 clib 计算再加载这一套材料。

### encode.inc 是什么

它是 SDK 生成的版本化任务清单，第一行为 `CLIB_EXPORT 2`；其余记录包括 CONFIG、BINARY、OBJECT、ROTATION、OUTPUT。不是需要用户编写或编译的任意 C++ 源码，也不是编译器通用 tool/encode.inc 的原样副本。

OBJECT 记录对象类型、逻辑名称、物理 Q、degree、槽位数和顺序编号的数值文件位置。导出的数值是每槽一个 double 的精确往返文本。trace.txt 保存算子和逻辑值依赖；每次导出生成独立的运行标识。

编码工具把这些记录转换为 ANT batch 请求。对象名与实际文件之间的对应由 encoded.inc 管理，上下文标识也保存在其中，数值文件和计算记录直接比较。应用不再通过手写路径匹配两份独立脚本。

同一配置与旋转集合复用上下文；同一批数值及 Q 需求复用已编码对象。只改代码但未改变编码需求时，也可复用编码结果。修改参数或旋转集合时先准备新上下文，成功后替换本应用的旧上下文。工作目录只保留当前版本。

### 输入格式与验证

原始输入和权重必须是 N/2 个有限实数；Q 是物理 limbs，2 到上下文 Q 之间的偶数。应用负责补零、分块和空间边界掩码。工具检查清单与数值，再调用 ANT。运行时检查数值与声明、对象头部和上下文标识；格式错误、截断和不同上下文混用会报错，不检测任意内容修改。

输出比较使用 main.cpp 的 Save_output 参考，绝对误差 `max(abs(actual-expected)) <= tolerance` 为通过。没有参考的输出只解密，不宣称通过数值正确性验证。

下面手动命令中的 SDK 表示 SDK 的绝对路径。

## 手动工具接口（已有材料或直接加载应用）

以下命令供需要手动准备材料的开发者查阅。新应用不需要手写这里的 JSON，也不需要逐条执行这些命令。

## 1. prepare：创建一套应用上下文

```bash
cat > app.json <<'JSON'
{"N": 1024, "q_limbs": 38, "rotations": [1, 17]}
JSON

"$SDK/tools/clib-ant-tool" prepare --config app.json --out ant-work
```

**输入**：配置 JSON。**输出**：新的 `ant-work/` 目录。输出目录必须不存在；准备失败后重试也请使用新目录，以免把不完整材料当作有效上下文。

| 配置字段 | 含义 | 默认值与范围 |
| --- | --- | --- |
| `N` | 环次数；每个对象 slots=N/2 | 默认 1024；支持 1024、8192、65536 |
| `q_limbs` | 上下文物理 Q limbs 数量 | 默认 62；32～62 的偶数 |
| `rotations` | 应用要使用的槽位旋转步数 | 默认 `[]`；int32 整数列表 |

配置只接受这三个字段。`rotations` 中正数为左旋、负数为右旋，工具按 slots 取模并去重。自举所需旋转和共轭密钥自动加入，合计旋转数量须小于 512。

当前固定 DS=1、DNUM=1、H=80。ANT 使用 first_mod_size=60、scaling_mod_size=52 生成上下文；DS 会拆分逻辑模数，因此这两个位数不是单个物理 limb 的位数。实际 Q/P 模数、P 数量和 scale 以 ANT 生成材料为准，Q/P 各自不超过 62。

工具还会在离线阶段运行自举，收集该参数下 ANT 编码的自举标量。这属于材料准备时间。

| 输出路径 | 内容 | 谁使用 |
| --- | --- | --- |
| `evaluation/param.txt`、`scales.txt` | ANT 参数与各层 scale | `Open_context` |
| `evaluation/key/` | 重线性化、旋转和共轭评估密钥 | clib 算子 |
| `evaluation/bootstrap.txt`、`plain/`、`scalars/` | 自举元数据、FFT 明文、标量 | clib Bootstrap |
| `evaluation/identity.txt` | 上下文及密钥集合标识 | 加载检查 |
| `private/` | ANT encoder、encryptor、decryptor 及身份记录 | encode、encrypt、decrypt |
| `case.json` | N、slots、实际 Q/P、自举最大输出 Q、应用旋转和准备耗时 | 开发者查看配置结果 |
| `preparation/`、`ant_prepare.log` | 离线准备中间文件与日志 | 准备过程排查 |

**`prepare` 生成通用上下文，不生成你自己的输入密文和模型权重。** 这些由下面的 encode/encrypt 命令准备；新模板由 Makefile 和导出需求串起这些步骤。

## 2. encode / encrypt：准备权重和输入

数值文件使用普通文本：每个文件恰好包含 N/2 个有限实数，空格或换行均可。文件中不写维度、标题、逗号或注释。数据量不足时补零，多于 slots 时自行分块。

下面为上面的 N1024 上下文生成 512 个输入值和权重：

```bash
python3 - <<'PY'
import json
from pathlib import Path
work = Path('ant-work')
slots = json.loads((work / 'case.json').read_text())['slots']
data = work / 'data'
data.mkdir(exist_ok=True)
(data / 'input.values').write_text('\n'.join(str((j % 31 - 15) / 64) for j in range(slots)))
(data / 'weight.values').write_text('\n'.join(['0.5'] * slots))
PY

"$SDK/tools/clib-ant-tool" encode --private ant-work/private \
  --values ant-work/data/weight.values --q-limbs 4 --out ant-work/app/weight.pt

"$SDK/tools/clib-ant-tool" encrypt --private ant-work/private \
  --values ant-work/data/input.values --q-limbs 4 --out ant-work/app/input.ct
```

| 参数 | 说明 |
| --- | --- |
| `--private` | 与计算上下文对应的 ANT private 目录 |
| `--values` | 待编码或加密的实数文本文件 |
| `--q-limbs` | 对象的物理 Q limbs，必填；2 到上下文 Q 之间的偶数 |
| `--out` | 目标 `.pt` 或 `.ct` 文件；工具创建父目录 |

`encode` 输出编码明文，供 `Load_plain` 使用；`encrypt` 输出密文，供 `Load_ciph` 使用。二者均由 ANT 选择目标层级的 scale，初始 scale degree 为 1。

输入密文与乘法权重需要编码在相同 Q。偏置若在 rescale 后相加，应编码在 rescale 后的 Q。例如 Q4 乘法、rescale 到 Q2，再加偏置时，偏置应使用 `--q-limbs 2`。

原始 `.values` 文件只在准备/参考计算时使用。clib 应用读取 `.pt`、`.ct`，不读取原始浮点权重。

## 3. batch：一次准备多个对象

多密文输入、卷积权重和多项式系数建议使用 batch，一次加载 ANT private 材料处理多条任务。

任务文件每行格式为：

```text
encode 4 "/absolute/path/weight.values" "/absolute/path/weight.pt"
encrypt 4 "/absolute/path/input.values" "/absolute/path/input.ct"
encode 2 "/absolute/path/bias.values" "/absolute/path/bias.pt"
```

四列依次是 `encode` 或 `encrypt`、Q limbs、输入数值文件、输出对象文件。路径用双引号包裹；清单中不写表头或注释。推荐绝对路径；相对路径按执行命令时的工作目录解释，不是按清单所在目录解释。

```bash
"$SDK/tools/clib-ant-tool" batch --private ant-work/private --requests requests.txt
```

输出为每行指定的文件。batch 不执行模型转换、分块或层级规划；任务中的数值和 Q 由应用准备代码确定。

## 4. decrypt：读取计算结果

假设应用已经将结果保存到 `result/bootstrap.ct`：

```bash
"$SDK/tools/clib-ant-tool" decrypt --private ant-work/private \
  --input result/bootstrap.ct --out result/bootstrap.json
```

**输入**：结果密文和对应密钥集的 `private/`。**输出**：N/2 个实数组成的 JSON 数组。

改变密钥后，即使 N、Q 相同，也不能用另一套 `private/` 解密旧密文。验证应比较所有有效输出槽位，并为同态计算误差及激活函数近似误差分别设置合适容差。

## 5. 材料如何复用

- 相同上下文处理新输入：复用 `evaluation/` 和 `private/`，只重新 encrypt。
- 相同上下文更换权重：复用上下文，重新 encode 或 batch。
- 改变 N、Q 或新增缺少的旋转密钥：在新目录 prepare，再生成该目录对应的应用对象。
- 复制到计算机器：复制 `evaluation/`、应用使用的 `.ct` / `.pt` 和应用可执行文件。解密在持有对应 `private/` 的环境执行。

工具帮助：

```bash
"$SDK/tools/clib-ant-tool" --help
"$SDK/tools/clib-ant-tool" prepare --help
"$SDK/tools/clib-ant-tool" batch --help
```
