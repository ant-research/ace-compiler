# ResNet Training

Training configuration and results for ResNet-CIFAR models.

## Training Scripts

| Script | Description |
|--------|-------------|
| `ace/model/train_resnet.py` | ResNet-20/32/44/56/110 on CIFAR-10/100 |

### Usage

```bash
# Train ResNet-20 on CIFAR-10 (default)
python -m ace.model.train_resnet --model 20 --epochs 200 --dataset cifar10

# Train ResNet-110 on CIFAR-10
python -m ace.model.train_resnet --model 110 --epochs 200 --dataset cifar10 --batch-size 128

# Train ResNet-32 on CIFAR-100
python -m ace.model.train_resnet --model 32 --epochs 200 --dataset cifar100 --batch-size 128
```

## Training Configuration

All ResNet-CIFAR models use the same training configuration:

| Parameter | Value | Note |
|-----------|-------|------|
| epochs | 200 | |
| batch_size | 128 | |
| lr | 0.1 | Initial learning rate |
| momentum | 0.9 | SGD momentum |
| weight_decay | 5e-4 | |
| lr_schedule | standard | 100ep->0.01, 150ep->0.001 |
| optimizer | SGD | |

### Normalization

| Dataset | mean | std |
|---------|------|-----|
| CIFAR-10 | (0.4914, 0.4822, 0.4465) | (0.2023, 0.1994, 0.2010) |
| CIFAR-100 | (0.5070, 0.4865, 0.4409) | (0.2673, 0.2564, 0.2761) |

Pretrained weights from [chenyaofo/pytorch-cifar-models](https://github.com/chenyaofo/pytorch-cifar-models).

### Data Augmentation

- RandomCrop(32, padding=4)
- RandomHorizontalFlip()

## Training Results

| Model | Dataset | Classes | Best Test Accuracy | Weight File |
|-------|---------|---------|--------------------|-------------|
| ResNet-20 | CIFAR-10 | 10 | 92.40% | resnet20_cifar10.pt |
| ResNet-32 | CIFAR-10 | 10 | 93.07% | resnet32_cifar10.pt |
| ResNet-44 | CIFAR-10 | 10 | 93.59% | resnet44_cifar10.pt |
| ResNet-56 | CIFAR-10 | 10 | 93.38% | resnet56_cifar10.pt |
| ResNet-110 | CIFAR-10 | 10 | 94.11% | resnet110_cifar10.pt |
| ResNet-32 | CIFAR-100 | 100 | 71.23% | resnet32_cifar100.pt |

Training log: `logs/train/train_20260516_173953.log`

## Weight Files

All pretrained weights are stored in `ace/model/resnet/weights/`:

```
weights/
├── resnet20_cifar10.pt
├── resnet32_cifar10.pt
├── resnet32_cifar100.pt
├── resnet44_cifar10.pt
├── resnet56_cifar10.pt
└── resnet110_cifar10.pt
```

Load weights in Python:

```python
from ace.model.resnet import create_pretrained_resnet

model = create_pretrained_resnet(n_layers=20, num_classes=10)
```

Or via ModelSpec:

```python
from ace.model.spec_resnet import RESNET20_CIFAR10

model = RESNET20_CIFAR10.create_model()
```