import os

print("install package ...")

# check pip package: yml, onnx
print("\ncheck pip package ...")
os.system("pip3 install -r requirements.txt")

# install cifar-10, cifar-100 dataset ...
cifar=f'/app/cifar'
dataset_cifar10 = f'wget -O- https://www.cs.toronto.edu/~kriz/cifar-10-binary.tar.gz | tar xzv -C {cifar} --strip-components=1'
dataset_cifar100 = f'wget -O- https://www.cs.toronto.edu/~kriz/cifar-100-binary.tar.gz | tar xzv -C {cifar} --strip-components=1'

print("\ncheck cifar dataset ...")
if not os.path.exists(cifar):
    print("\n install cifar dataset ...")
    os.system(f'mkdir -p {cifar}')
    os.system(dataset_cifar10)
    os.system(dataset_cifar100)
