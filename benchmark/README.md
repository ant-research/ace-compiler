# ACE FHE Benchmark

ResNet FHE 性能基准测试，使用 pytest-benchmark 进行统计采样和历史基线对比。

## 运行方式

```bash
# 运行所有 benchmark
pytest benchmark/ --benchmark-only -v

# 运行并保存结果
pytest benchmark/ --benchmark-only --benchmark-autosave

# 与上次基线对比
pytest benchmark/ --benchmark-only --benchmark-compare=last

# 性能回退 >10% 则失败
pytest benchmark/ --benchmark-only --benchmark-autosave \
  --benchmark-compare=last --benchmark-compare-fail=mean:10%

# 指定 GPU
CUDA_VISIBLE_DEVICES=6 pytest benchmark/ --benchmark-only -v
```

## Benchmark 内容

| Benchmark | 说明 |
|-----------|------|
| `test_resnet20_compile_time` | ResNet-20 FHE 编译耗时 |
| `test_resnet20_inference_latency` | 单图推理延迟 |
| `test_resnet20_inference_throughput` | 推理吞吐量 |

## 编译缓存

编译产物缓存在 `.compile_cache/` 目录下，由 Driver 自动管理，避免重复编译。
设置 `ACE_FORCE_REBUILD=1` 或调用 `configure_cache(force_rebuild=True)` 强制重新编译。

## 数据依赖

CIFAR-10 数据需要位于以下路径之一：
- `/app/cifar/cifar-10-batches-py/test_batch`
- `benchmark/data/cifar10/cifar-10-batches-py/test_batch`

## 与功能测试的区别

- **功能测试**（编译验证、推理精度）在 `tests/test_regression/test_resnet_torch.py`，用 `@pytest.mark.slow` 标记
- **性能基准** 在 `benchmark/`，用 pytest-benchmark 做统计测量
- CI 快速测试：`pytest tests/ -m "not slow"`
- 完整验证：`pytest tests/ -m slow`