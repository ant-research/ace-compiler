#!/usr/bin/env python3
"""Prepare per-context scalar encodings with ANT; never used during evaluation."""
import argparse
import os
from pathlib import Path
import subprocess

def prepare(native, collector, material, private, work):
    native,collector,material,private,work=map(lambda p:Path(p).resolve(),(native,collector,material,private,work))
    work.mkdir(parents=True,exist_ok=True)
    (material/'scalars').mkdir(exist_ok=True)
    env={**os.environ,'OMP_NUM_THREADS':'32','OMP_WAIT_POLICY':'PASSIVE',
         'CLIB_ANT_NATIVE':str(native),'CLIB_ANT_PRIVATE':str(private)}
    lines=(material/'param.txt').read_text().splitlines()
    n=int(next(s.split()[1] for s in lines if s.startswith('[Degree:]')))
    scales=[float(v) for s in lines if s.startswith('[scale:]') for v in s.split()[1:]]
    q=[float(s.split(',')[1]) for s in lines if s.startswith('[q:]')]
    requests=[]
    for level in range(4,len(q)+1,2):
        for target in range(2,level,2):
            scale=scales[level//2-1];u=scales[target//2];v=scales[target//2-1]
            requests.extend([(u*u/scale/scale,scale,level),
                             (v*v/(scale*scale)*(q[level-1]*q[level-2])/scale,scale,level)])
    req=work/'modswitch_requests.txt'
    req.write_text(''.join(f'{v:.17g} {s:.17g} {l}\n' for v,s,l in requests))
    subprocess.run([native,'scalars',private,req,material/'scalars'],env=env,check=True)
    values=work/'collector.values';values.write_text('0\n'*(n//2))
    ciphertext=work/'collector.ct'
    subprocess.run([native,'encrypt',private,values,'2',ciphertext],env=env,check=True)
    with (work/'catalogue.log').open('w') as log:
        subprocess.run([collector,material,ciphertext,work/'output'],env=env,stdout=log,stderr=subprocess.STDOUT,check=True)
    return len(list((material/'scalars').glob('*.txt')))

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--build',type=Path,required=True);p.add_argument('--material',type=Path,required=True)
    a=p.parse_args()
    from materials import seal
    print('ANT scalar records:',prepare(a.build/'ant-native',a.build/'prepare-scalars',a.material,a.material,a.build/'catalogue'))
    seal(a.material)
