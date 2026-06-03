#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

'''
imagenet_reader.py: Reference implementation for C++ ImageNet reader
Load images, convert to Tensor and do normalization, dump all intermediate
results to file to verify C++ ImageNet reader
'''

from PIL import Image
from torchvision import transforms

INPUT_IMAGE = Image.open("../../../test/ILSVRC2012_val_00046108.JPEG")

#
# Original PyTroch Preprocess Steps:
#
#preprocess = transforms.Compose([
#    transforms.RESIZE(256),
#    transforms.CenterCrop(224),
#    transforms.ToTensor(),
#    transforms.NORMALIZE(mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225]),
#])
#
#input_tensor = preprocess(INPUT_IMAGE)
#print (input_tensor)

#
# Split into small steps and dump content to file for validation
#
RESIZE = transforms.RESIZE(256)
CENTER_CROP = transforms.CenterCrop(224)
TO_TENSOR = transforms.ToTensor()
NORMALIZE = transforms.NORMALIZE(mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225])

ORIG_IMAGE = TO_TENSOR(INPUT_IMAGE)
ORIG_IMAGE.numpy().tofile('python.orig.txt', ' ', '%5.4f')

RESZ_IMAGE = TO_TENSOR(RESIZE(INPUT_IMAGE))
RESZ_IMAGE.numpy().tofile('python.resize.txt', ' ', '%5.4f')

CROP_IMAGE = TO_TENSOR(CENTER_CROP(RESIZE(INPUT_IMAGE)))
CROP_IMAGE.numpy().tofile('python.crop.txt', ' ', '%5.4f')

NORM_IMAGE = NORMALIZE(TO_TENSOR(CENTER_CROP(RESIZE(INPUT_IMAGE))))
NORM_IMAGE.numpy().tofile('python.norm.txt', ' ', '%5.4f')
