#!/usr/bin/env python3
#
# Auto-generate the testing report (csv + report.md + figure png) from the .log
# files produced by the test entry scripts. This is the "report generation"
# layer that closes the gap between raw .log output and the curated result
# data (data 7.X.csv / report.md / figures_7.X.png) seen under testing/.
#
# Inputs (auto-discovered by suffix):
#   7.1  cpu_cgo25.log   -- cgo25_ae.py -a   : "<model>: Accuracy=X%, Time=Y(s)"
#         perf_e.log     -- perf.py -e        : "<Model> Inference Time = X.XXXX(s)"
#   7.2  asplos25.log    -- resbm.py -n 10    : "<model>: Accuracy=X%, Time=Y(s)"
#   7.3  expert.log      -- resbm_expert      : "<Model>: Unencrypted Accuracy = X%"
#         asplos25.log   -- resbm.py -n 100   : "<model>: Accuracy=X%" (encrypted)
#   7.4  perf_gpu.log    -- perf_gpu.py -a    : "..._expert: Inference Time = X(s)"
#                                               "..._ace: ... Inference Time = X(s)"
#
# Outputs (out_dir, default testing/):
#   data 7.X.csv (BOM header, GB2312-table style matching the existing csv)
#   report.md     (markdown tables + figure embeds)
#   figures_7.X.png (matplotlib bar charts; matplotlib optional)
import os
import sys
import re
import glob
import argparse
import datetime

# 6 model rows for 7.1 / 7.4 (ResNet family)
RESNET_ROWS = [
    ('1', 'ResNet-20', 'CIFAR-10', 'resnet20_cifar10'),
    ('2', 'ResNet-32', 'CIFAR-10', 'resnet32_cifar10'),
    ('3', 'ResNet-32', 'CIFAR-100', 'resnet32_cifar100'),
    ('4', 'ResNet-44', 'CIFAR-10', 'resnet44_cifar10'),
    ('5', 'ResNet-56', 'CIFAR-10', 'resnet56_cifar10'),
    ('6', 'ResNet-110', 'CIFAR-10', 'resnet110_cifar10'),
]
# 4 model rows for 7.2 / 7.3 (generalization family)
GEN_ROWS = [
    ('1', 'AlexNet', 'CIFAR-10', 'alexnet_cifar10'),
    ('2', 'ResNet-20', 'CIFAR-10', 'resnet20_cifar10'),
    ('3', 'SqueezeNet', 'CIFAR-10', 'squeezenet_cifar10'),
    ('4', 'VGG16', 'CIFAR-10', 'vgg16_cifar10'),
]
MODEL_CAP = {  # caption name -> slug used in logs
    'AlexNet': 'alexnet_cifar10', 'ResNet-20': 'resnet20_cifar10',
    'SqueezeNet': 'squeezenet_cifar10', 'VGG16': 'vgg16_cifar10',
    'ResNet-32': 'resnet32_cifar10', 'ResNet-44': 'resnet44_cifar10',
    'ResNet-56': 'resnet56_cifar10', 'ResNet-110': 'resnet110_cifar10',
}


def parse_log(path):
    return open(path, encoding='utf-8', errors='replace').read() if os.path.isfile(path) else ''


def find_log(log_dir, *kw):
    """Return the first log under log_dir whose name contains all keywords."""
    for f in sorted(glob.glob(os.path.join(log_dir, '*.log'))):
        base = os.path.basename(f)
        if all(k in base for k in kw):
            return f
    return None


def find_log_prefix(log_dir, prefix, exclude=None):
    """Find the newest .log whose name starts with `prefix` (e.g. '7.4_'),
    optionally excluding files whose name contains any substring in `exclude`.
    Files matching the prefix unambiguously tie a log to one test item — the
    producer scripts now stamp tested-item into the filename:
      perf.py        -> 7.1_<ts>.log            (FHE-MP-CNN CPU expert)
      cgo25_ae.py    -> 7.1_<ts>.cgo25.log       (ANT-ACE CPU side)
      resbm.py -n 10 -> 7.2_<ts>.asplos25.log
      resbm.py -n 100-> 7.3_<ts>.asplos25.log
      resbm_expert   -> 7.3_<ts>_acc_<n>.log     (plaintext baseline)
      perf_gpu.py    -> 7.4_<ts>.log
      run_rtlib.py   -> 7.5_<ts>.rtlib.log
    Newest-first so the most recent run wins.
    """
    excl = exclude or ()
    if isinstance(excl, str):
        excl = (excl,)
    for f in sorted(glob.glob(os.path.join(log_dir, prefix + '*.log')), reverse=True):
        base = os.path.basename(f)
        if any(x in base for x in excl):
            continue
        return f
    return None


def extract_ace_time_acc(text):
    """From cgo25_ae.py / resbm.py logs, extract per-model (time_s, acc_pct).

    Two real-world shapes:
      resbm.py  : '<model>: Accuracy = X%, Time = Y(s), Memory = ...' (one line)
      cgo25_ae  : '<model>:' then next 'Exec: Time = Y(s), Memory=..., Accuracy = X%'
    Parse by '<model>:' blocks; within each, first 'Time = Y(s)' and first
    'Accuracy = X%' (Exec: line for cgo25, inline for resbm).
    """
    out = {}
    # Split on lines that look like '<slug>:' headers (model casing).
    blocks = re.split(r'(?m)^(\w+_cifar\w*):', text)
    # blocks = [pre, slug1, body1, slug2, body2, ...]
    for i in range(1, len(blocks) - 1, 2):
        slug = blocks[i].strip()
        body = blocks[i + 1]
        t = None
        acc = None
        m = re.search(r'Time\s*=\s*([\d.]+)\(s\)', body)
        if m: t = float(m.group(1))
        ma = re.search(r'Accuracy\s*=\s*([\d.]+)\s*%', body)
        if ma: acc = float(ma.group(1))
        if t is not None or acc is not None:
            out[slug] = (t, acc)
    return out


def extract_expert_acc(text):
    """From resbm_expert log: '<Model>: Unencrypted Accuracy = X%'."""
    out = {}
    pat = re.compile(r'^([A-Za-z0-9\-]+):\s*Unencrypted Accuracy\s*=\s*([\d.]+)\s*%', re.M)
    for m in pat.finditer(text):
        name, acc = m.group(1), float(m.group(2))
        out[name] = acc
    return out


def extract_inference_time(text):
    """From perf.py -e (FHE-MP-CNN CPU expert) log.

    perf.py -e writes: '<slug>_expert: Time = X.XX(s) TotalMemory = ...'
    i.e. 'Time = X(s)' with no 'Inference' prefix, and the slug carries an
    '_expert' suffix that must be stripped to match RESNET_ROWS slugs.
    """
    out = {}
    pat = re.compile(r'^(\w+)_expert:\s*.*?Time\s*=\s*([\d.]+)\(s\)', re.M)
    for m in pat.finditer(text):
        slug, t = m.group(1), float(m.group(2))
        out[slug] = t
    return out


def write_csv(path, title, header, rows):
    # utf-8-sig emits a single leading BOM; do NOT add a literal ﻿ too.
    with open(path, 'w', encoding='utf-8-sig') as f:
        f.write(title + ',' * (len(header) - 1) + '\n')
        f.write(','.join(header) + '\n')
        for r in rows:
            f.write(','.join(str(x) for x in r) + '\n')


def fig_bar(out_png, labels, series_list, series_names, title, ylabel, ylim=None):
    try:
        import matplotlib
        matplotlib.use('Agg')
        import matplotlib.pyplot as plt
    except ImportError:
        return False
    # Each series is a list aligned to labels; None/NaN entries are skipped.
    # Series must be python floats (use float('nan') for missing) so matplotlib
    # bar() skips them gracefully.
    import math
    pairs = [(n, s) for n, s in zip(series_names, series_list)
             if any(not (v is None or (isinstance(v, float) and math.isnan(v))) for v in s)]
    if not pairs:
        return False
    names, sers = zip(*pairs)
    fig, ax = plt.subplots(figsize=(8, 5))
    x = list(range(len(labels)))
    w = 0.8 / max(1, len(sers))
    for i, s in enumerate(sers):
        ax.bar([xi + i * w for xi in x], s, width=w, label=names[i])
    ax.set_xticks([xi + w * (len(sers) - 1) / 2 for xi in x])
    ax.set_xticklabels(labels, rotation=20, ha='right')
    ax.set_ylabel(ylabel)
    ax.set_title(title)
    if ylim is not None:
        ax.set_ylim(ylim)
    ax.legend()
    fig.tight_layout()
    fig.savefig(out_png, dpi=110)
    plt.close(fig)
    return True


def fig_bar_dual(out_png, labels, left_series, left_name, left_ylabel, left_ylim,
                 right_series, right_name, right_ylabel, right_ylim, title):
    """Side-by-side bar subplots for two metrics with different units.

    Used by 7.2 to show both inference time (s, left) and accuracy (%, right)
    in one figure so the generalization verdict (4 models x 10 images all
    100%) is visible alongside the per-model cost.
    """
    try:
        import matplotlib
        matplotlib.use('Agg')
        import matplotlib.pyplot as plt
    except ImportError:
        return False
    import math
    def _clean(s):
        return [v if not (v is None or (isinstance(v, float) and math.isnan(v))) else 0.0 for v in s]
    if not any(_clean(left_series)) and not any(_clean(right_series)):
        return False
    fig, (ax_l, ax_r) = plt.subplots(1, 2, figsize=(12, 5))
    x = list(range(len(labels)))
    # left: inference time
    ax_l.bar(x, _clean(left_series), width=0.6, label=left_name, color='#4c72b0')
    ax_l.set_xticks(x)
    ax_l.set_xticklabels(labels, rotation=20, ha='right')
    ax_l.set_ylabel(left_ylabel)
    if left_ylim is not None:
        ax_l.set_ylim(left_ylim)
    ax_l.legend()
    # right: accuracy
    ax_r.bar(x, _clean(right_series), width=0.6, label=right_name, color='#55a868')
    ax_r.set_xticks(x)
    ax_r.set_xticklabels(labels, rotation=20, ha='right')
    ax_r.set_ylabel(right_ylabel)
    if right_ylim is not None:
        ax_r.set_ylim(right_ylim)
    ax_r.legend()
    fig.suptitle(title)
    fig.tight_layout()
    fig.savefig(out_png, dpi=110)
    plt.close(fig)
    return True


def _to_float(s):
    """Parse '<num>(s)' / '<num>%' / '<num>' to float, else nan."""
    import math
    if s in ('-', '', None):
        return float('nan')
    s = s.rstrip('(s)').rstrip('%').rstrip('s)')  # tolerant strip
    try:
        return float(s)
    except ValueError:
        return float('nan')


def gen_7_1(log_dir, out_dir):
    # 7.1 has two logs: 7.1_<ts>.cgo25.log (Ant-ACE side, cgo25_ae.py) and
    # 7.1_<ts>.log (FHE-MP-CNN expert side, perf.py -e).
    cgo = parse_log(find_log(log_dir, '7.1_', 'cgo25')
                    or find_log(log_dir, 'cgo25') or '')
    perf = parse_log(find_log_prefix(log_dir, '7.1_', exclude='cgo25')
                    or find_log(log_dir, 'perf') or '')
    ace = extract_ace_time_acc(cgo)                  # ANT-ACE
    fhe = extract_inference_time(perf)              # FHE-MP-CNN
    rows = []
    for n, name, ds, slug in RESNET_ROWS:
        a = ace.get(slug, (None, None))[0]
        fc = fhe.get(slug)
        speed = ('%.9f' % (fc / a)) if (a and fc) else '-'
        rows.append([n, name, ds,
                     ('%.1f(s)' % a) if a else '-',
                     ('%.2f(s)' % fc) if fc else '-',
                     speed])
    avg = sum(float(r[5]) for r in rows if r[5] != '-') / max(1, sum(1 for r in rows if r[5] != '-'))
    rows.append(['Average', '-', '-', '-', '-', '%.9f' % avg])
    write_csv(os.path.join(out_dir, 'data 7.1.csv'),
              'FHE Inference Performance Test for Neural Networks',
              ['No.', 'Model', 'Dataset', 'ANT-ACE Inference Time', 'FHE-MP-CNN Inference Time', 'Speedup'],
              rows)
    fig_bar(os.path.join(out_dir, 'figures_7.1.png'),
            [r[1] for r in rows[:-1]],
            [[_to_float(r[3]) for r in rows[:-1]],
             [_to_float(r[4]) for r in rows[:-1]]],
            ['ANT-ACE', 'FHE-MP-CNN'], '7.1 Inference performance (s)', 'time (s)')
    return rows


def gen_7_2(log_dir, out_dir):
    # resbm.py -n 10 writes 7.2_<ts>.asplos25.log
    asp = parse_log(find_log_prefix(log_dir, '7.2_')
                    or find_log(log_dir, 'asplos25') or find_log(log_dir, 'acc') or '')
    ace = extract_ace_time_acc(asp)
    rows = []
    for n, name, ds, slug in GEN_ROWS:
        t, acc = ace.get(slug, (None, None))
        rows.append([n, name, ds,
                     ('%.2f(s)' % t) if t else '-',
                     ('%.2f%%' % acc) if acc is not None else '-'])
    write_csv(os.path.join(out_dir, 'data 7.2.csv'),
              'FHE Compiler Generalization Ability Test',
              ['No.', 'Model', 'Dataset', 'ANT-ACE Inference Time', 'Inference Accuracy (10 images)'],
              rows)
    fig_bar_dual(os.path.join(out_dir, 'figures_7.2.png'),
                 [r[1] for r in rows],
                 [_to_float(r[3]) for r in rows], 'ANT-ACE', 'time (s)', None,
                 [_to_float(r[4]) for r in rows], 'Accuracy', 'accuracy (%)', (0, 100),
                 '7.2 Generalization: inference time (left) + accuracy 10 images (right)')
    return rows


def gen_7_3(log_dir, out_dir):
    # 7.3 has two logs: 7.3_<ts>_acc_<n>.log (resbm_expert, plaintext baseline)
    # and 7.3_<ts>.asplos25.log (resbm.py -n 100, encrypted accuracy).
    expert = parse_log(find_log(log_dir, '7.3_', 'acc')
                      or find_log(log_dir, 'acc')
                      or find_log(log_dir, '10_00') or '')
    enc_log = parse_log(find_log(log_dir, '7.3_', 'asplos25')
                      or find_log(log_dir, 'asplos25') or '')
    plain = extract_expert_acc(expert)
    enc = extract_ace_time_acc(enc_log)
    rows = []
    for n, name, ds, slug in GEN_ROWS:
        e_acc = enc.get(slug, (None, None))[1]
        p_acc = plain.get(name)
        diff = ('%d%%' % abs(int(p_acc) - int(e_acc))) if (e_acc is not None and p_acc is not None) else '-'
        rows.append([n, name, ds,
                     ('%.1f%%' % e_acc) if e_acc is not None else '-',
                     ('%.1f%%' % p_acc) if p_acc is not None else '-',
                     diff])
    valid = [int(r[5].rstrip('%')) for r in rows if r[5] not in ('-',)]
    rows.append(['Average', '-', '-', '-', '-', '%.2f%%' % (sum(valid) / max(1, len(valid))) if valid else '-'])
    write_csv(os.path.join(out_dir, 'data 7.3.csv'),
              'Inference Accuracy Test',
              ['No.', 'Model', 'Dataset', 'ANT-ACE Encrypted Accuracy', 'Non-encrypted Accuracy', 'Accuracy Difference'],
              rows)
    fig_bar(os.path.join(out_dir, 'figures_7.3.png'),
            [r[1] for r in rows[:-1]],
            [[_to_float(r[3]) for r in rows[:-1]],
             [_to_float(r[4]) for r in rows[:-1]]],
            ['Encrypted', 'Non-encrypted'], '7.3 Inference accuracy (%)', 'accuracy (%)')
    return rows


def gen_7_4(log_dir, out_dir):
    # The 7.4 log is written by perf_gpu.py as 7.4_<ts>.log to cwd and contains
    # both "----- Expert -----" (phantom-fhe GPU) and "----- ACE -----" sections.
    # Two lines per model: "<slug>_expert: ... Inference Time = X(s)" and
    # "<slug>_ace: ... Inference Time = X(s)".

    def pick_74_log():
        # Prefix match first (newest with the 7.4_ stamp wins).
        p = find_log_prefix(log_dir, '7.4_')
        if p:
            return p
        # Legacy fallback: explicit name match by old convention.
        for kw in ('perf_gpu', 'H20', 'A100'):
            p = find_log(log_dir, kw)
            if p:
                return p
        # Last fallback: newest .log containing both Expert and ACE content.
        for f in sorted(glob.glob(os.path.join(log_dir, '*.log')), reverse=True):
            txt = parse_log(f)
            if '----- Expert -----' in txt and '----- ACE -----' in txt:
                return f
        return ''

    gpu = parse_log(pick_74_log())
    # Two passes: _expert (FHE-MP-CNN/phantom side) and _ace (ANT-ACE side)
    expert = {}  # slug -> seconds
    ace = {}     # slug -> seconds
    for m in re.finditer(r'^(\w+)_expert:\s*.*?Inference Time\s*=\s*([\d.]+)\(s\)', gpu, re.M):
        expert[m.group(1)] = float(m.group(2))
    for m in re.finditer(r'^(\w+)_ace:\s*.*?(?:RESULT.*?\.)?\s*Inference Time\s*=\s*([\d.]+)\(s\)', gpu, re.M):
        ace[m.group(1)] = float(m.group(2))
    rows = []
    for n, name, ds, slug in RESNET_ROWS:
        e = expert.get(slug)
        a = ace.get(slug)
        # report 7.4 table: FHE-MP-CNN (phantom) time first, then ANT-ACE, then 加速比 (FHE-MP-CNN/ANT-ACE).
        speed = ('%.9f' % (e / a)) if (a and e) else '-'
        rows.append([n, name, ds,
                    ('%.4f(s)' % e) if e is not None else '-',
                    ('%.4f(s)' % a) if a is not None else '-',
                    speed])
    write_csv(os.path.join(out_dir, 'data 7.4.csv'),
              'FHE Compilation System Performance Test for Hardware Accelerators',
              ['No.', 'Model', 'Dataset', 'FHE-MP-CNN Inference Time', 'ANT-ACE Inference Time', 'Speedup'],
              rows)
    fig_bar(os.path.join(out_dir, 'figures_7.4.png'),
            [r[1] for r in rows],
            [[_to_float(r[3]) for r in rows],
             [_to_float(r[4]) for r in rows]],
            ['FHE-MP-CNN(phantom)', 'ANT-ACE'], '7.4 GPU inference time (s)', 'time (s)')
    return rows


def write_report(out_dir, r71, r72, r73, r74):
    md = ['# Testing Report (auto-generated)\n']
    for sec, title, hdr, rows in [
        ('7.1', '7.1 推理性能(CPU)', ['编号', '模型', '集', 'ANT-ACE', 'FHE-MP-CNN', '加速比'], r71),
        ('7.2', '7.2 泛化能力', ['编号', '模型', '集', 'ANT-ACE', '准确率(10图)'], r72),
        ('7.3', '7.3 精度', ['编号', '模型', '集', '加密', '非加密', '差异'], r73),
        ('7.4', '7.4 GPU 性能', ['编号', '模型', '集', 'FHE-MP-CNN', 'ANT-ACE', '加速比'], r74),
    ]:
        md.append('## %s\n' % title)
        md.append('|' + '|'.join(hdr) + '|')
        md.append('|' + '|'.join(['---'] * len(hdr)) + '|')
        if rows:
            for r in rows:
                md.append('|' + '|'.join(str(x) for x in r) + '|')
        else:
            md.append('|n/a|（this test was not run/extracted yet）|...|...|...|...|')
        if os.path.isfile(os.path.join(out_dir, 'figures_%s.png' % sec)):
            md.append('\n![figures_%s](./figures_%s.png)\n' % (sec, sec))
        md.append('')
    with open(os.path.join(out_dir, 'report.md'), 'w', encoding='utf-8') as f:
        f.write('\n'.join(md))


def main():
    p = argparse.ArgumentParser(description='Generate testing report (csv+md+png) from .log files')
    p.add_argument('-L', '--log_dir', default='.', help='dir containing the .log files')
    p.add_argument('-o', '--out_dir', default='/app/testing', help='output dir for csv/md/png')
    p.add_argument('-t', '--test', default='all', choices=['all', '7.1', '7.2', '7.3', '7.4'])
    a = p.parse_args()
    os.makedirs(a.out_dir, exist_ok=True)
    r71 = r72 = r73 = r74 = None
    if a.test in ('all', '7.1'): r71 = gen_7_1(a.log_dir, a.out_dir)
    if a.test in ('all', '7.2'): r72 = gen_7_2(a.log_dir, a.out_dir)
    if a.test in ('all', '7.3'): r73 = gen_7_3(a.log_dir, a.out_dir)
    if a.test in ('all', '7.4'): r74 = gen_7_4(a.log_dir, a.out_dir)
    write_report(a.out_dir, r71, r72, r73, r74)
    print('report written to', a.out_dir)


if __name__ == '__main__':
    main()
