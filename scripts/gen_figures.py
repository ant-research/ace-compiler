#!/usr/bin/env python3
#
# Generate trend figures (figures_7.X.png) from the tables in testing/report.md.
# Parses the markdown tables directly — no .log parsing — so the figures stay
# in sync with the curated report content.
#
# Usage:
#   python3 scripts/gen_figures.py                    # all sections, testing/
#   python3 scripts/gen_figures.py -o testing -t 7.4 # single section
import os
import re
import argparse
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

# Pick a CJK-capable font available on macOS / Linux so chart titles and labels
# render Chinese instead of tofu boxes.
for _f in ('PingFang SC', 'Heiti SC', 'Arial Unicode MS', 'Noto Sans CJK SC',
           'Noto Sans CJK JP', 'WenQuanYi Zen Hei', 'Source Han Sans SC'):
    try:
        matplotlib.font_manager.findfont(_f, fallback_to_default=False)
        plt.rcParams['font.sans-serif'] = [_f]
        break
    except Exception:
        continue
plt.rcParams['axes.unicode_minus'] = False


REPORT_MD = os.path.join(os.path.dirname(__file__), '..', 'testing', 'report.md')


def parse_md_tables(path):
    """Return {section_header: {table_title: [rows]}} from a markdown file.

    Sections are keyed by the nearest preceding ## heading. Each table is
    keyed by the nearest preceding ### or #### heading. Prose lines between
    a heading and its table preserve the title; prose after a table
    finalises it so the next table (even without an explicit heading) gets
    a fresh key.
    """
    text = open(path, encoding='utf-8').read()
    current_section = None
    current_title = None
    saw_table = False  # True once we've seen any | row for the current title
    tables = {}
    untitled = 0
    for line in text.splitlines():
        if line.startswith('## ') and not line.startswith('### '):
            current_section = line.lstrip('# ').strip()
            current_title = None
            saw_table = False
            tables.setdefault(current_section, {})
        elif line.startswith('### ') and not line.startswith('#### '):
            current_title = line.lstrip('# ').strip()
            saw_table = False
        elif line.startswith('#### '):
            current_title = line.lstrip('# ').strip()
            saw_table = False
        elif line.startswith('|'):
            if current_section is None:
                continue
            cells = [c.strip() for c in line.strip('|').split('|')]
            if all(set(c) <= set('-: ') for c in cells):  # separator row
                continue
            if current_title is None:
                untitled += 1
                current_title = '_untitled_%d' % untitled
            tables[current_section].setdefault(current_title, []).append(cells)
            saw_table = True
        elif line.strip() == '':
            continue
        else:
            # non-blank prose line: only finalise the table if we've already
            # started one. Prose between a heading and its table is preserved.
            if saw_table:
                current_title = None
                saw_table = False
    return tables


def clean(s):
    """Strip markdown bold/code/links, keep the numeric/text payload."""
    s = re.sub(r'`([^`]*)`', r'\1', s)
    s = re.sub(r'\*\*([^*]*)\*\*', r'\1', s)
    s = re.sub(r'\[([^\]]*)\]\([^)]*\)', r'\1', s)
    return s.strip()


def to_num(s):
    """Parse a cell like '0.48', '354.51', '85.00%', '2997.94 s' to float."""
    s = clean(s)
    if s in ('', '-', '—', 'PASS', 'N/A'):
        return None
    m = re.search(r'-?\d+\.?\d*', s.replace(',', ''))
    return float(m.group()) if m else None


def find_table(tables, section_substr, title_substr=None):
    """Find the first table under a section matching section_substr (and
    optionally a title containing title_substr)."""
    for sec, tbls in tables.items():
        if section_substr in sec:
            if title_substr is None:
                for title, rows in tbls.items():
                    if rows:
                        return rows
            else:
                for title, rows in tbls.items():
                    if title_substr in title and rows:
                        return rows
    return None


def bar_save(out, labels, series, names, title, ylabel, log_y=False, ylim=None):
    fig, ax = plt.subplots(figsize=(10, 5.5))
    x = list(range(len(labels)))
    w = 0.8 / max(1, len(series))
    for i, s in enumerate(series):
        s = [v if v is not None else float('nan') for v in s]
        ax.bar([xi + i * w for xi in x], s, width=w, label=names[i])
    ax.set_xticks([xi + w * (len(series) - 1) / 2 for xi in x])
    ax.set_xticklabels(labels, rotation=20, ha='right')
    ax.set_ylabel(ylabel)
    ax.set_title(title)
    if log_y:
        ax.set_yscale('log')
    if ylim is not None:
        ax.set_ylim(ylim)
    ax.legend()
    ax.grid(axis='y', alpha=0.3)
    fig.tight_layout()
    fig.savefig(out, dpi=120)
    plt.close(fig)
    print('wrote', out)


def bar_line_save(out, labels, bars, bar_name, line, line_name, title, barylabel, lineylabel):
    """Bar + secondary line (e.g. inference time + speedup)."""
    bars = [v if v is not None else float('nan') for v in bars]
    line = [v if v is not None else float('nan') for v in line]
    fig, ax1 = plt.subplots(figsize=(10, 5.5))
    x = list(range(len(labels)))
    ax1.bar(x, bars, width=0.55, color='#4c72b0', label=bar_name)
    ax1.set_xticks(x)
    ax1.set_xticklabels(labels, rotation=20, ha='right')
    ax1.set_ylabel(barylabel)
    ax1.grid(axis='y', alpha=0.3)
    ax2 = ax1.twinx()
    ax2.plot(x, line, 'o-', color='#dd8452', label=line_name, linewidth=2, markersize=8)
    ax2.set_ylabel(lineylabel)
    lines1, labels1 = ax1.get_legend_handles_labels()
    lines2, labels2 = ax2.get_legend_handles_labels()
    ax1.legend(lines1 + lines2, labels1 + labels2, loc='upper left')
    plt.title(title)
    fig.tight_layout()
    fig.savefig(out, dpi=120)
    plt.close(fig)
    print('wrote', out)


# --- 7.1 basic ops: CPU vs GPU compile time + exec time ---------------------

def gen_7_1(tables, out_dir):
    cpu_rows = find_table(tables, '7.1', 'CPU')
    gpu_rows = find_table(tables, '7.1', 'GPU')
    if not cpu_rows or not gpu_rows:
        print('skip 7.1: missing CPU or GPU table')
        return
    cpu, gpu = cpu_rows[1:], gpu_rows[1:]  # drop header
    # rows: [编号, 算子, 输入形状, 编译时间(s), 执行时间(ms), 执行结果]
    labels = ['%s %s' % (clean(r[1]), clean(r[2])) for r in cpu]
    cpu_compile = [to_num(r[3]) for r in cpu]
    gpu_compile = [to_num(r[3]) for r in gpu]
    cpu_exec = [to_num(r[4]) for r in cpu]
    gpu_exec = [to_num(r[4]) for r in gpu]

    bar_save(os.path.join(out_dir, 'figures_7.1_compile.png'),
             labels,
             [cpu_compile, gpu_compile],
             ['CPU', 'GPU'],
             '7.1 基础算子编译时间 (CPU vs GPU)',
             '编译时间 (s, 对数刻度)', log_y=True)
    bar_save(os.path.join(out_dir, 'figures_7.1_exec.png'),
             labels,
             [cpu_exec, gpu_exec],
             ['CPU', 'GPU'],
             '7.1 基础算子执行时间 (CPU vs GPU)',
             '执行时间 (ms, 对数刻度)', log_y=True)


# --- 7.2 model compile: CPU end-to-end time; GPU compile + total -------------

def gen_7_2(tables, out_dir):
    cpu_rows = find_table(tables, '7.2', 'CPU')
    gpu_rows = find_table(tables, '7.2', 'GPU')
    if cpu_rows:
        cpu = cpu_rows[1:]
        # [编号, 模型, 数据集, 端到端耗时, 最大绝对误差, 执行结果]
        labels = [clean(r[1]) for r in cpu]
        times = [to_num(r[3]) for r in cpu]
        bar_save(os.path.join(out_dir, 'figures_7.2_cpu.png'),
                 labels, [times], ['ANT-ACE CPU'],
                 '7.2 模型端到端编译执行耗时 (CPU)',
                 '端到端耗时 (s)')
    if gpu_rows:
        gpu = gpu_rows[1:]
        # [编号, 模型, 数据集, 编译时间, 总耗时, 最大绝对误差, 执行结果]
        labels = [clean(r[1]) for r in gpu]
        compile_t = [to_num(r[3]) for r in gpu]
        total_t = [to_num(r[4]) for r in gpu]
        bar_save(os.path.join(out_dir, 'figures_7.2_gpu.png'),
                 labels, [compile_t, total_t], ['编译时间', '总耗时'],
                 '7.2 模型编译与总耗时 (GPU, Phantom)',
                 '耗时 (s)')


# --- 7.3 accuracy: encrypted vs plaintext ------------------------------------

def gen_7_3(tables, out_dir):
    cpu_rows = find_table(tables, '7.3', 'CPU')
    gpu_rows = find_table(tables, '7.3', 'GPU')
    if cpu_rows:
        cpu = cpu_rows[1:]
        # [编号, 模型, 数据集, 加密准确率, 非加密准确率, 差异, 基线文件]
        labels = [clean(r[1]) for r in cpu]
        enc = [to_num(r[3]) for r in cpu]
        plain = [to_num(r[4]) for r in cpu]
        bar_save(os.path.join(out_dir, 'figures_7.3_cpu.png'),
                 labels, [enc, plain], ['加密准确率', '明文准确率'],
                 '7.3 模型加密/明文准确率对比 (CPU, 100 张 CIFAR-10)',
                 'Top-1 准确率 (%)', ylim=(0, 100))
    if gpu_rows:
        gpu = gpu_rows[1:]
        labels = [clean(r[1]) for r in gpu]
        enc = [to_num(r[3]) for r in gpu]
        plain = [to_num(r[4]) for r in gpu]
        bar_save(os.path.join(out_dir, 'figures_7.3_gpu.png'),
                 labels, [enc, plain], ['加密准确率', '明文准确率'],
                 '7.3 模型加密/明文准确率对比 (GPU, 100 张图片)',
                 'Top-1 准确率 (%)', ylim=(0, 100))


# --- 7.4 CPU perf: FHE-MP-CNN vs ANT-ACE + speedup ---------------------------

def gen_7_4(tables, out_dir):
    rows = find_table(tables, '7.4', 'ResNet')
    if not rows:
        print('skip 7.4: table not found')
        return
    data = rows[1:]
    # [编号, 模型, FHE-MP-CNN, ANT-ACE, 加速比]
    labels = [clean(r[1]) for r in data]
    fhe = [to_num(r[2]) for r in data]
    ace = [to_num(r[3]) for r in data]
    speedup = [to_num(r[4]) for r in data]
    bar_save(os.path.join(out_dir, 'figures_7.4_time.png'),
             labels, [fhe, ace], ['FHE-MP-CNN', 'ANT-ACE'],
             '7.4 ResNet 系列 FHE 推理耗时 (CPU)',
             '推理耗时 (s)')
    bar_line_save(os.path.join(out_dir, 'figures_7.4_speedup.png'),
                  labels, fhe, 'FHE-MP-CNN 耗时', speedup, '加速比 (FHE-MP-CNN ÷ ANT-ACE)',
                  '7.4 ResNet 系列: ANT-ACE 推理耗时与加速比', '推理耗时 (s)', '加速比 (×)')


# --- 7.5 FPGA: compiler vs handwritten + speedup -----------------------------

def gen_7_5(tables, out_dir):
    sec = None
    for s in tables:
        if s.startswith('7.5'):
            sec = s
            break
    if not sec:
        print('skip 7.5: section not found')
        return
    # The comparison table has header row with '手写 FHE-MP-CNN'; the
    # compiler-only table has only 'FPGA 实测耗时'.
    cmp_rows = None
    for title, rows in tables[sec].items():
        if not rows:
            continue
        header = ' '.join(rows[0])
        if '手写' in header and '编译器生成' in header:
            cmp_rows = rows
            break
    if not cmp_rows:
        print('skip 7.5: comparison table not found')
        return
    # rows[0] is header; data rows are rows[1:]
    data = cmp_rows[1:]
    # [模型, 编译器生成代码, 手写 FHE-MP-CNN, 加速比]
    labels = [clean(r[0]) for r in data]
    comp = [to_num(r[1]) for r in data]
    hand = [to_num(r[2]) for r in data]
    speedup = [to_num(r[3]) for r in data]
    bar_save(os.path.join(out_dir, 'figures_7.5_time.png'),
             labels, [comp, hand], ['编译器生成代码', '手写 FHE-MP-CNN'],
             '7.5 FPGA: 编译器生成代码 vs 手写 FHE-MP-CNN 耗时',
             '实测耗时 (s)')
    bar_line_save(os.path.join(out_dir, 'figures_7.5_speedup.png'),
                  labels, comp, '编译器生成代码耗时', speedup, '加速比 (手写 ÷ 编译器)',
                  '7.5 FPGA: 编译器生成代码耗时与加速比', '实测耗时 (s)', '加速比 (×)')


def main():
    p = argparse.ArgumentParser(description='Generate figures from testing/report.md tables')
    p.add_argument('-r', '--report', default=REPORT_MD, help='path to report.md')
    p.add_argument('-o', '--out_dir', default=os.path.join(os.path.dirname(__file__), '..', 'testing'),
                   help='output dir for figures')
    p.add_argument('-t', '--test', default='all', choices=['all', '7.1', '7.2', '7.3', '7.4', '7.5'])
    a = p.parse_args()
    os.makedirs(a.out_dir, exist_ok=True)
    tables = parse_md_tables(a.report)
    if a.test in ('all', '7.1'): gen_7_1(tables, a.out_dir)
    if a.test in ('all', '7.2'): gen_7_2(tables, a.out_dir)
    if a.test in ('all', '7.3'): gen_7_3(tables, a.out_dir)
    if a.test in ('all', '7.4'): gen_7_4(tables, a.out_dir)
    if a.test in ('all', '7.5'): gen_7_5(tables, a.out_dir)


if __name__ == '__main__':
    main()
