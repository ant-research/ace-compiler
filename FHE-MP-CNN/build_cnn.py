import os
import subprocess

dir = f'/app/FHE-MP-CNN'
cnn = f'{dir}/FHE-MP-CNN'

# source code & patch
clone_cnn=f'git clone -b main-3.6.6 https://github.com/snu-ccl/FHE-MP-CNN.git {cnn}'
patch_001=f'{dir}/0001-capture-keymem-support-resnet32_cifar10.patch'
patch=f'git apply {patch_001}'

# build command
build_seal=f'cmake -S {cnn}/cnn_ckks/cpu-ckks/single-key/seal-modified-3.6.6 -B {dir}/build_seal -DCMAKE_BUILD_TYPE=Release; \
            cmake --build {dir}/build_seal -- -j; \
            cmake --install {dir}/build_seal'
build_cnn=f'cmake -S {cnn}/cnn_ckks -B {cnn}/cnn_ckks/build_cnn -DCNN=ON -DCMAKE_BUILD_TYPE=Release; \
            cmake --build {cnn}/cnn_ckks/build_cnn -- -j'

# clone FHE-MP-CNN
print("\nclone FHE-MP-CNN ...")
if not os.path.exists(cnn):
    ret = os.system(clone_cnn)
    assert(ret == 0)

    print("\npatch FHE-MP-CNN ...")
    print("  0001 : capture keymem and support resnet32_cifar100 ...")
    p = subprocess.Popen(patch, cwd=cnn, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    p.wait()

# build seal
print("\nbuild seal-3.6.6 ...")
ret = os.system(build_seal)
assert(ret == 0)

# build cnn
print("\nbuild cnn_ckks ...")
ret = os.system(build_cnn)
assert(ret == 0)

# test