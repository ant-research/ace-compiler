#!/usr/bin/env python3
"""gen_ref.py — numpy ResNet-20 (CIFAR-10) forward for clib-sdk resnet20_eva (route B).

Reproduces the NUMERICAL chain of the SEAL ckks program (cnn_seal.cpp /
infer_seal.cpp:ResNet_cifar10_seal_sparse) so clib cipher outputs can be checked
block-by-block against plaintext references.

clib block packing (route B, N=8192, slots=4096/cipher):
  block_channels = 4096 // (H*W); cipher k holds channels [k*blk:(k+1)*blk]
  slot j -> channel=j//(H*W), h=(j%(H*W))//W, w=j%W

SEAL scaling (must match exactly, else label flips):
  - input:  img /= B         (B=40; brings conv_out into ReLU poly interval [-1,1])
  - layer0: fW = conv_w * (bn_w/sqrt(var+eps)); fb = bn_b - bn_w*mean/sqrt(var+eps);
           then fW/=B, fb/=B  => out0 = (conv_w*bn_w/sqrt)/B . conv(img/B) + (bn_b - bn_w*mean/sqrt)/B
  - stage blocks: weight folded with bn_w/sqrt (NO /B); the BN `g` term:
           g = (mean*bn_w/sqrt - bn_b)/B ; stage_bn_out = (bn_w/sqrt . conv) - g
           i.e. effective bias term = (bn_b - mean*bn_w/sqrt)/B , and conv term stays (bn_w/sqrt)*conv(x)
           (x is ReLU output, already ~[-1,1]); the /B only touches the bias/g term.
  - shortcut identity (stage1, no downsample): out += input
  - shortcut downsample (stage2/3 first block): out += np.pad(input[:, ::2, ::2],
                                              ((0, co-ci), (0,0), (0,0)))
  - avgpool: feat.mean(axis=(1,2))  -> [c]
  - FC:     linear_weight[10,64] @ x[64]  (bias added per SEAL; the dead-code path
            is treated as the intended add — verified against argmax below)

Exports per block (packed stage-out layout) for clib Save_output comparison:
  stage1 (16ch 32x32, blk=4): layer0 already; + per block s{stag}_conv1/relu1/conv2/relu2
  stage2 (32ch 16x16, blk=16): 2 cipher; refs packed.
  stage3 (64ch 8x8,   blk=64): 1 cipher.
Final: avgpool.out, logits (size 10).
"""
import numpy as np
from pathlib import Path

P = Path("/app/baselines/fpga/pretrained_parameters/resnet20_new")
TEST = Path("/app/baselines/fpga/testFile")
OUT = Path("/app/baselines/fpga/clib-sdk/apps/resnet20_eva/data")
OUT.mkdir(parents=True, exist_ok=True)

B = 40.0
EPS = 1e-5


def rd(n):
    return np.fromstring((P / n).read_text(), sep=" ", dtype=float)


def conv3(x, W, stride=1):
    """x:[ci,h,w] W:[co,ci,3,3] -> [co,oh,ow] pad1."""
    co, ci, kh, kw = W.shape
    h, w = x.shape[1:]
    xp = np.pad(x, ((0, 0), (1, 1), (1, 1)))
    oh = (h + 2 - 3) // stride + 1
    ow = (w + 2 - 3) // stride + 1
    out = np.zeros((co, oh, ow))
    for o in range(co):
        for y in range(oh):
            ys = y * stride
            for xx in range(ow):
                xs = xx * stride
                out[o, y, xx] = (xp[:, ys:ys + 3, xs:xs + 3] * W[o]).sum()
    return out


def fold(w, mean, var, bw, bm):
    """SEAL BN-fold: fW = w * bw/sqrt(var+eps); fb = bm - bw*mean/sqrt(var+eps).
    NO /B here (caller decides per layer)."""
    s = bw / np.sqrt(var + EPS)
    fW = w * s[:, None, None, None]
    fb = bm - bw * mean / np.sqrt(var + EPS)
    return fW, fb


def relu(x):
    return np.maximum(x, 0.0)


# ---------------- stage geometry ----------------
# block repacking: out_g index -> (cipher ob, co_rel): co_rel = out_g % blk; ob = out_g // blk
GEOM = {  # stage(j): (C, H, W, blk, nblk)
    0: (16, 32, 32, 4, 4),    # stage1
    1: (32, 16, 16, 16, 2),   # stage2
    2: (64, 8, 8, 64, 1),     # stage3
}


def pack(feat, name):
    """feat[C,H,W] -> cipher-major 1D (clib layout). blk=4096//(H*W)."""
    C, H, W = feat.shape
    blk = 4096 // (H * W)
    nblk = (C + blk - 1) // blk
    slots = blk * H * W
    arr = np.zeros(nblk * slots)
    for k in range(nblk):
        for j in range(slots):
            ch = j // (H * W); h = (j % (H * W)) // W; w = j % W
            gch = k * blk + ch
            if gch < C:
                arr[k * slots + j] = feat[gch, h, w]
    (OUT / name).write_text(" ".join(repr(float(x)) for x in arr))


def main():
    img = np.fromstring((TEST / "test_values.txt").read_text(), sep=" ").reshape(-1, 3, 32, 32)[0]
    label = int(np.fromstring((TEST / "test_label.txt").read_text(), sep=" ")[0])
    print(f"image0 label={label}")
    img = img / B

    # ---- layer0: conv(3->16)+bn+relu (layer0 has special /B fold, as in gen_ref) ----
    fW0, fb0 = fold(rd("conv1_weight.txt").reshape(16, 3, 3, 3),
                    rd("bn1_running_mean.txt"), rd("bn1_running_var.txt"),
                    rd("bn1_weight.txt"), rd("bn1_bias.txt"))
    fW0 = fW0 / B; fb0 = fb0 / B
    conv0 = conv3(img, fW0, 1) + fb0[:, None, None]      # [16,32,32], ReLU-pre-input
    relu0 = relu(conv0)
    print(f"layer0 conv0 [{conv0.min():.3f},{conv0.max():.3f}] relu0 [{relu0.min():.3f},{relu0.max():.3f}]")

    # pack + export layer0 (already produced by old gen_ref, re-exported for self-containment)
    pack(img, "input.img")
    (OUT / "layer0.conv.weight").write_text(" ".join(repr(float(x)) for x in fW0.ravel()))
    (OUT / "layer0.conv.bias").write_text(" ".join(repr(float(x)) for x in fb0.ravel()))
    pack(conv0, "layer0.conv.ref")
    pack(relu0, "layer0.relu.ref")

    # ---- stages 1..3, each 3 blocks ----
    # Per block k in stage j: conv1 (stride st) + bn1 + relu ; conv2 (stride1) + bn2 ;
    #   shortcut (identity if no downsample, else pad-downsample) ; add ; relu.
    # Conv weight fold: fW = w*bn_w/sqrt(var+eps)  (NO /B); effective bias = (bn_b - bn_w*mean/sqrt)/B
    feat = relu0  # stage1 input [16,32,32]
    for j in range(3):
        C, H, W, blk, nblk = GEOM[j]
        for k in range(3):
            st = 2 if (j >= 1 and k == 0) else 1
            # conv1
            co1 = C
            ci1 = {0: 16, 1: (16 if k == 0 else 32), 2: (32 if k == 0 else 64)}[j]
            W1 = rd(f"layer{j+1}_{k}_conv1_weight.txt").reshape(co1, ci1, 3, 3)
            fW1, fb1 = fold(W1,
                            rd(f"layer{j+1}_{k}_bn1_running_mean.txt"),
                            rd(f"layer{j+1}_{k}_bn1_running_var.txt"),
                            rd(f"layer{j+1}_{k}_bn1_weight.txt"),
                            rd(f"layer{j+1}_{k}_bn1_bias.txt"))
            # stage fold: weight stays fW1 (=w*bn_w/sqrt); bias = fb1/B
            conv1 = conv3(feat, fW1, st) + (fb1 / B)[:, None, None]   # stage_bn1 = conv1 (g term folded)
            relu1 = relu(conv1)
            # conv2
            W2 = rd(f"layer{j+1}_{k}_conv2_weight.txt").reshape(C, C, 3, 3)
            fW2, fb2 = fold(W2,
                            rd(f"layer{j+1}_{k}_bn2_running_mean.txt"),
                            rd(f"layer{j+1}_{k}_bn2_running_var.txt"),
                            rd(f"layer{j+1}_{k}_bn2_weight.txt"),
                            rd(f"layer{j+1}_{k}_bn2_bias.txt"))
            conv2 = conv3(relu1, fW2, 1) + (fb2 / B)[:, None, None]
            # shortcut
            if st == 2:
                sub = feat[:, ::2, ::2]                       # [ci, H/2, W/2]
                sc = np.zeros((C, sub.shape[1], sub.shape[2]))
                sc[:ci1] = sub                                   # zero-pad channels ci1->C
            else:
                sc = feat
            added = conv2 + sc
            out = relu(added)
            print(f"stage{j+1} blk{k} ci1={ci1} co={C} st={st} conv1[{conv1.min():.3f},{conv1.max():.3f}] "
                  f"conv2[{conv2.min():.3f},{conv2.max():.3f}] out[{out.min():.3f},{out.max():.3f}]")
            # export refs (packed stage-out layout)
            tag = f"s{j+1}_{k}"
            pack(conv1, f"{tag}.conv1.ref")
            pack(relu1, f"{tag}.relu1.ref")
            pack(conv2, f"{tag}.conv2.ref")
            pack(out,   f"{tag}.relu2.ref")
            # fused conv weights/bias for clib (weight=fW no /B; bias=fb/B)
            (OUT / f"{tag}.conv1.weight").write_text(" ".join(repr(float(x)) for x in fW1.ravel()))
            (OUT / f"{tag}.conv1.bias").write_text(" ".join(repr(float(x)) for x in (fb1 / B).ravel()))
            (OUT / f"{tag}.conv2.weight").write_text(" ".join(repr(float(x)) for x in fW2.ravel()))
            (OUT / f"{tag}.conv2.bias").write_text(" ".join(repr(float(x)) for x in (fb2 / B).ravel()))
            feat = out

    # ---- avgpool + FC ----
    avg = feat.mean(axis=(1, 2))   # [64]
    pack(np.expand_dims(np.expand_dims(avg, 1), 1), "avgpool.ref")  # treat as [64,1,1] in stage3 layout
    lw = rd("linear_weight.txt").reshape(10, 64)
    lb = rd("linear_bias.txt")
    logits = lw @ avg + lb
    (OUT / "logits.ref").write_text(" ".join(repr(float(x)) for x in logits))
    print(f"logits={np.round(logits,4)}")
    print(f"argmax={int(np.argmax(logits))}  (label={label})  match={int(np.argmax(logits))==label}")
    print("exported to", OUT)


if __name__ == "__main__":
    main()
