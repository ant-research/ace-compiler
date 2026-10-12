"""Independent NumPy CHW network and Chebyshev polynomial reference."""
from pathlib import Path
import numpy as np
from numpy.polynomial import chebyshev as ch
DATA = Path(__file__).resolve().parents[1] / 'data'


def polynomials():
    raw = np.loadtxt(DATA/'result/d13.txt')
    trees = [([-1,8,4,4,0,0,0,2,-1,-1,-1,-1,-1,-1,0,0],15),
             ([-1,8,4,4,0,0,0,2,-1,-1,-1,-1,-1,-1,0,0],15),
             ([-1,16,8,8,0,0,0,0],27)]
    offset, result = 0, []
    for stage,(tree,degree) in enumerate(trees):
        degrees={1:degree}; leaves={}
        for j in range(2,len(tree)):
            degrees[j]=tree[j//2]-1 if j%2==0 else degrees[j//2]-tree[j//2]
        for j in range(1,len(tree)):
            if tree[j]!=0: continue
            size=degrees[j]+1
            c=raw[offset:offset+size].copy(); offset+=size
            c[::2]=0 # oddbaby evaluates odd Chebyshev terms only
            leaves[j]=c
        def expand(j):
            if tree[j]==0: return leaves[j]
            t=np.zeros(tree[j]+1); t[-1]=1
            return ch.chebadd(expand(2*j),ch.chebmul(t,expand(2*j+1)))
        result.append(expand(1)/[2,1.7,2][stage])
    if offset!=len(raw): raise ValueError('unexpected d13 coefficient count')
    return result


def relu(x):
    y=x.copy()
    for c in polynomials(): y=ch.chebval(y,c)
    return x*(.5+y)


def conv(x,w,stride=1):
    pad=w.shape[2]//2
    patches=np.lib.stride_tricks.sliding_window_view(np.pad(x,((0,0),(pad,pad),(pad,pad))),w.shape[2:],axis=(1,2))[:,::stride,::stride]
    return np.einsum('cxyij,ocij->oxy',patches,w,optimize=True)


def network(model,sample,exact_relu=False):
    classes=10 if model==20 else 100
    bound=40. if classes==10 else 65.
    folder=DATA/'pretrained_parameters'/('resnet20_new' if classes==10 else 'resnet32_cifar100')
    test=DATA/('testFile' if classes==10 else 'testFile_cifar100')
    def rd(name): return np.loadtxt(folder/(name+'.txt')).reshape(-1)
    def layer(x,wp,bp,cout,stride=1,kernel=3):
        weight=rd(wp+'_weight').reshape(cout,x.shape[0],kernel,kernel)
        scale=rd(bp+'_weight')/np.sqrt(rd(bp+'_running_var')+1e-5)
        bias=(rd(bp+'_bias')-rd(bp+'_running_mean')*scale)/bound
        return conv(x,weight*scale[:,None,None,None],stride)+bias[:,None,None]
    activation=(lambda x:np.maximum(x,0)) if exact_relu else relu
    x=np.loadtxt(test/'test_values.txt').reshape(3,3,32,32)[sample]/bound
    x=activation(layer(x,'conv1','bn1',16))
    for stage in range(1,4):
        cout=16*2**(stage-1)
        for block in range((model-2)//6):
            tag=f'layer{stage}_{block}'
            stride=2 if stage>1 and block==0 else 1
            skip=x
            y=activation(layer(x,tag+'_conv1',tag+'_bn1',cout,stride))
            y=layer(y,tag+'_conv2',tag+'_bn2',cout)
            if stride==2:
                if classes==10:
                    # Option A pads equally on both sides of the channel dimension.
                    channels=skip.shape[0]
                    skip=np.pad(skip[:,::2,::2],((channels//2,channels//2),(0,0),(0,0)))
                else:
                    skip=layer(skip,tag+'_shortcut_0',tag+'_shortcut_1',cout,2,1)
            x=activation(y+skip)
    avg=x.mean(axis=(1,2))*bound
    # Upstream matrix_multiplication_seal constructs b but never adds it.
    logits=rd('linear_weight').reshape(classes,64)@avg
    if not np.isfinite(logits).all(): raise ValueError('non-finite reference logits')
    return logits
