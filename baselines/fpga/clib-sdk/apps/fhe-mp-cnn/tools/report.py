#!/usr/bin/env python3
"""Report actual ANT-decrypted logits separately from the SEAL shadow."""
import argparse
import json
import shlex
import numpy as np
from reference import DATA, network
p=argparse.ArgumentParser()
p.add_argument('--work',type=__import__('pathlib').Path,required=True)
p.add_argument('--output',type=__import__('pathlib').Path,required=True)
p.add_argument('--model',type=int,choices=[20,32],required=True)
p.add_argument('--sample',type=int,choices=[0,1,2],required=True)
a=p.parse_args()
classes=10 if a.model==20 else 100
name=f'fc_logits_{a.sample}'
actual=np.array(json.loads((a.output/(name+'.json')).read_text()))[:classes]
record=next(shlex.split(line) for line in (a.work/'export/encode.inc').read_text().splitlines() if line.startswith('OUTPUT ') and shlex.split(line)[1]==name)
shadow=np.loadtxt(a.work/'export'/record[5]).reshape(-1)[:classes]
reference=network(a.model,a.sample)
label=int(np.loadtxt(DATA/('testFile' if classes==10 else 'testFile_cifar100')/'test_label.txt')[a.sample])
report=dict(model=a.model,sample=a.sample,label=label,decrypted_top1=int(actual.argmax()),
            reference_top1=int(reference.argmax()),shadow_top1=int(shadow.argmax()),
            cipher_vs_shadow_max_abs=float(np.max(np.abs(actual-shadow))),
            cipher_vs_independent_max_abs=float(np.max(np.abs(actual-reference))),
            shadow_vs_independent_max_abs=float(np.max(np.abs(shadow-reference))),
            logits=actual.tolist(),independent_logits=reference.tolist())
(a.output/'classification.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps(report,indent=2))
if report['shadow_vs_independent_max_abs']>1e-6:
    raise SystemExit('independent network reference differs from packed graph')
if report['cipher_vs_independent_max_abs']>2.0:
    raise SystemExit('actual logits exceed the agreed 2.0 tolerance')
