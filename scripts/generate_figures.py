#!/usr/bin/env python3
"""Generate all Evaluation-section tables and figures for the HPAO paper.

Produces (all PDF):
  fig6.pdf  — End-to-end speedups over ANT-ACE, LReLU models
  fig7.pdf  — End-to-end speedups over ANT-ACE, HReLU models
  fig8.pdf  — Micro-benchmark speedups (Conv/AvgPool/GEMM combined)
  tab4.pdf  — Bootstrap frequency and runtime share (merged L/H per model)
  tab5.pdf  — Geometric-mean speedups on workload-derived microbenchmarks
  tab6.pdf  — Microbenchmark abbreviations
  tab7.pdf  — Average operator-level improvements
  tab8.pdf  — Compilation times (HReLU models)

Usage:
    python3 generate_figures.py -d machine182/605_merged_data
    python3 generate_figures.py -d machine182/605_merged_data -o ae_result

The data directory may contain both model logs (e.g. resnet20_cifar10_pre.base.log)
and micro-benchmark logs (e.g. i16x32x32_ci16_co16_k3p1s1.base.log); entries are
split by name: micro-benchmarks start with 'i<digit>'.
"""

import argparse
import logging
import os
import re
from pathlib import Path

import matplotlib
import matplotlib.pyplot as plt
from matplotlib.text import Text
import numpy as np
import pandas as pd
from plottable import ColumnDefinition, Table

# ==============================================================================
# SECTION 1: DATA PARSING  (imported from existing scripts — unmodified)
# ==============================================================================

from generate_tables import display_name
from gen_model_compile_runtime_tables import (
    load_opt_data,
    compute_speedup,
    OPT_DIRS,
    OPT_LABELS,
)
from gen_model_runtime_charts import (
    _draw_grouped_bars,
    _max_speedup,
)
from gen_micro_bench_charts import (
    plot_combined as _plot_micro_combined,
    BENCH_LABELS,
)
from gen_micro_bench_gmean_table import (
    compute_category_means,
    geometric_mean,
    TABLE_OPT_KEYS,
    CATEGORY_LABELS,
    CATEGORY_ORDER,
)
from gen_bts_freq_rtshare_table import load_bootstrap_data
from gen_op_avg_reduction_table import load_data, compute_reductions

logging.basicConfig(level=logging.INFO,
                    format="%(asctime)s - %(levelname)s - %(message)s")

def _is_micro_bench(name):
    """Micro-benchmark names start with i<digit>, e.g. i16x32x32_ci16_co16_k3p1s1."""
    return re.match(r'^i\d', name) is not None


def _infer_category(name):
    """Infer micro-benchmark category from its name."""
    if "_ci" in name and "_co" in name:
        return "conv"
    if re.match(r'^i\d+_o\d+$', name):
        return "gemm"
    return "avgpool"


def _model_display(name):
    """Display name with explicit -H/-L suffix (models only)."""
    dname = display_name(name)
    if not dname.endswith(("-H", "-L")):
        dname += "-H" if "_x2" not in name else "-L"
    return dname


def _paper_model_name(name):
    """Paper-style base name: LeNet, ResNet-20, VGG-11 (no -H/-L suffix)."""
    dname = display_name(name)
    dname = re.sub(r'-(H|L)$', '', dname)
    dname = re.sub(r'([a-zA-Z])(\d+)$', r'\1-\2', dname)
    return dname


def _base_name(name):
    return re.sub(r'_(x2|pre|train)$', '', name)


def _model_sort_key(name):
    """Sort models in paper order: LeNet, ResNet*, VGG*."""
    order = {"lenet": 0, "resnet": 1, "vgg": 2}
    base = _base_name(name).lower()
    for prefix, rank in order.items():
        if base.startswith(prefix):
            m = re.search(r'(\d+)', base)
            return (rank, int(m.group(1)) if m else 0)
    return (99, 0)


# ==============================================================================
# SECTION 2: DATA PROCESSING
# ==============================================================================

def split_entries(models):
    """Split loaded model names into (model_entries, micro_bench_entries)."""
    model_entries = sorted(
        (m for m in models if not _is_micro_bench(m)),
        key=_model_sort_key)
    bench_entries = sorted(m for m in models if _is_micro_bench(m))
    return model_entries, bench_entries


def build_tab4(data_dir):
    """Bootstrap frequency + runtime share, merged L/H per base model."""
    data = load_bootstrap_data(opt_suffix="base", data_dir=data_dir)
    data = {k: v for k, v in data.items()
            if not _is_micro_bench(k)}

    # Pair H and L variants per base model
    bases = {}
    for name, d in data.items():
        base = _base_name(name)
        side = "L" if "_x2" in name else "H"
        bases.setdefault(base, {})[side] = d

    rows = {}
    for base in sorted(bases, key=_model_sort_key):
        pair = bases[base]
        cells = []
        for metric in ("calls", "share"):
            parts = []
            for side in ("L", "H"):
                d = pair.get(side)
                if d is None or d["main_graph_time"] <= 0:
                    parts.append("---")
                elif metric == "calls":
                    parts.append(str(d["bootstrap_count"]))
                else:
                    share = d["bootstrap_time"] / d["main_graph_time"] * 100
                    parts.append(f"{share:.2f}%")
            cells.append(" / ".join(parts))
        rows[_paper_model_name(base)] = cells

    df = pd.DataFrame.from_dict(
        rows, orient="index",
        columns=["Bootstrap Calls\n(LReLU / HReLU)",
                 "Bootstrap Runtime Share\n(LReLU / HReLU)"])
    df.index.name = "Model"
    return df


def build_tab5(runtimes, bench_entries):
    """Geometric-mean speedups per micro-benchmark category."""
    speedups = compute_speedup(runtimes)
    benches = [m for m in bench_entries
               if runtimes["conv_fast"].get(m) is not None]
    categories = {m: _infer_category(m) for m in benches}
    _, cat_gmeans, all_bench_keys = compute_category_means(
        speedups, benches, categories)

    overall = {}
    for opt in TABLE_OPT_KEYS:
        vals = [speedups[opt].get(m) for m in all_bench_keys]
        vals = [v for v in vals if v is not None and v > 0]
        overall[opt] = geometric_mean(vals) if vals else None

    rows = {}
    for cat in sorted(cat_gmeans, key=lambda c: CATEGORY_ORDER[c]):
        rows[CATEGORY_LABELS[cat]] = [
            f"{cat_gmeans[cat].get(o):.2f}×" if cat_gmeans[cat].get(o) else "---"
            for o in TABLE_OPT_KEYS]
    rows["Overall"] = [
        f"{overall.get(o):.2f}×" if overall.get(o) else "---"
        for o in TABLE_OPT_KEYS]

    df = pd.DataFrame.from_dict(
        rows, orient="index",
        columns=[OPT_LABELS[o] for o in TABLE_OPT_KEYS])
    df.index.name = "Type"
    return df


def build_tab6():
    """Microbenchmark abbreviation mapping (2-pair column layout)."""
    abbrev_order = ([f"C{i}" for i in range(1, 9)]
                    + [f"P{i}" for i in range(1, 4)]
                    + [f"G{i}" for i in range(1, 5)])
    full_by_abbrev = {v: k for k, v in BENCH_LABELS.items()}

    rows = {}
    for i in range(0, len(abbrev_order), 2):
        left = abbrev_order[i]
        right = abbrev_order[i + 1] if i + 1 < len(abbrev_order) else None
        key = f"row{i // 2}"
        rows[key] = [left, full_by_abbrev.get(left, ""),
                     right or "", full_by_abbrev.get(right, "") if right else ""]

    df = pd.DataFrame.from_dict(
        rows, orient="index",
        columns=["Abbrev.", "Microbenchmark", "Abbrev. ", "Microbenchmark "])
    df.index = [""] * len(df)  # blank index column
    df.index.name = " "
    return df


def build_tab7(base_dir, data_dir):
    """Average operator-level improvements per optimization."""
    models, data = load_data(base_dir, data_dir)
    models = [m for m in models if not _is_micro_bench(m)]
    rows_raw = compute_reductions(models, data)

    rows = []
    for opt, effect, reductions in rows_raw:
        effect = effect.replace("\\texttt{", "").replace("}", "")
        avg = sum(reductions) / len(reductions) if reductions else None
        rows.append({"Opt.": opt,
                     "Measured Effect": effect,
                     "Avg. Reduction": f"{avg:.2f}%" if avg is not None else "---"})

    df = pd.DataFrame(rows)
    df.index = [""] * len(df)  # suppressed index
    df.index.name = " "
    return df


def build_tab8(compile_times, model_entries):
    """Compilation times for HReLU models only."""
    h_models = [m for m in model_entries if "_x2" not in m]
    rows = {}
    for m in h_models:
        vals = []
        for opt in OPT_DIRS:
            v = compile_times[opt].get(m)
            vals.append(f"{v:.2f}" if v is not None else "---")
        rows[_model_display(m)] = vals

    df = pd.DataFrame.from_dict(
        rows, orient="index", columns=[OPT_LABELS[o] for o in OPT_DIRS])
    df.index.name = "Model"
    return df


# ==============================================================================
# SECTION 3: PLOTTING
# ==============================================================================

matplotlib.rcParams.update({
    "font.family": "sans-serif",
    "font.sans-serif": ["DejaVu Sans", "Helvetica", "Arial"],
})

DESCRIPTIONS = {
    "fig6": "Fig. 6. End-to-end speedups over the ANT-ACE baseline for seven DNN models using LReLU.",
    "fig7": "Fig. 7. End-to-end speedups over the ANT-ACE baseline for seven DNN models using HReLU.",
    "fig8": "Fig. 8. Runtime speedups over ANT-ACE on workload-derived convolution,\naverage-pooling, and GEMM microbenchmarks extracted from the evaluated DNNs.",
    "tab4": "Table 4. Bootstrap frequency and runtime share.",
    "tab5": "Table 5. Geometric-mean speedups over ANT-ACE on workload-derived\nmicrobenchmarks.",
    "tab6": "Table 6. Microbenchmark abbreviations used in Figure 8.",
    "tab7": "Table 7. Average operator-level improvements across all evaluated model\nvariants (7 models × 2 polynomial ReLU settings).",
    "tab8": "Table 8. Compilation times (s) for ANT-ACE, individual HPAO optimization\npasses, and HPAO-ALL on HReLU models.",
}


def plot_table(df, col_defs, filename, output_dir, description, figsize=(6, 4)):
    """Render a DataFrame as a styled PDF table with a caption above it."""
    fig, ax = plt.subplots(figsize=figsize)
    fig.suptitle(description, x=0.05, y=0.98, ha="left",
                 fontsize=10, fontweight="normal")
    Table(df, column_definitions=col_defs, ax=ax, index_col=None,
          textprops={"fontfamily": "Arial", "fontsize": 10},
          row_dividers=False, footer_divider=True)

    header_strings = set()
    for cd in col_defs:
        header_strings.add(cd.name)
        if cd.group:
            header_strings.add(cd.group)
    for artist in ax.get_children():
        if isinstance(artist, Text) and artist.get_text() in header_strings:
            artist.set_fontweight("bold")

    plt.tight_layout(rect=[0, 0, 1, 0.95])
    out = output_dir / filename
    plt.savefig(out, dpi=750, format="pdf", bbox_inches="tight")
    plt.close(fig)
    logging.info(f"Saved: {out}")


def plot_runtime_fig(models, speedups, y_max, filename, output_dir, description):
    """Generate one runtime speedup bar chart (Fig 6 or Fig 7)."""
    if not models:
        return
    n = len(models)
    fig, ax = plt.subplots(1, 1, figsize=(9, max(2.5, n * 0.35 + 0.8)))
    _draw_grouped_bars(ax, models, speedups, y_max=y_max)
    ax.legend(loc="upper center", bbox_to_anchor=(0.5, 1.02), fontsize=10,
              ncol=6, frameon=False, columnspacing=0.8, handletextpad=0.3,
              handlelength=1.0, borderpad=0.3, prop={"weight": "bold"})
    fig.suptitle(description, x=0.02, y=0.98, ha="left",
                 fontsize=10, fontweight="normal")
    fig.subplots_adjust(left=0.10, right=0.97, top=0.85, bottom=0.07)
    out = output_dir / filename
    fig.savefig(out, format="pdf", bbox_inches="tight", pad_inches=0.01)
    plt.close(fig)
    logging.info(f"Saved: {out}")


def generate_all_tables(tables, output_dir):
    if "tab4" in tables:
        plot_table(
            tables["tab4"],
            [ColumnDefinition(name="Model", textprops={"ha": "left"}, width=1.0),
             ColumnDefinition(name="Bootstrap Calls\n(LReLU / HReLU)",
                              textprops={"ha": "center"}, width=1.5),
             ColumnDefinition(name="Bootstrap Runtime Share\n(LReLU / HReLU)",
                              textprops={"ha": "center"}, width=1.8)],
            "tab4.pdf", output_dir, DESCRIPTIONS["tab4"], figsize=(7, 4))

    if "tab5" in tables:
        plot_table(
            tables["tab5"],
            [ColumnDefinition(name="Type", textprops={"ha": "left"}, width=0.9)]
            + [ColumnDefinition(name=OPT_LABELS[o], textprops={"ha": "center"},
                                width=0.9) for o in TABLE_OPT_KEYS],
            "tab5.pdf", output_dir, DESCRIPTIONS["tab5"])

    if "tab6" in tables:
        plot_table(
            tables["tab6"],
            [ColumnDefinition(name=" ", textprops={"ha": "left"}, width=0.1),
             ColumnDefinition(name="Abbrev.", textprops={"ha": "center"}, width=0.55),
             ColumnDefinition(name="Microbenchmark", textprops={"ha": "left"}, width=2.1),
             ColumnDefinition(name="Abbrev. ", textprops={"ha": "center"}, width=0.55),
             ColumnDefinition(name="Microbenchmark ", textprops={"ha": "left"}, width=2.1)],
            "tab6.pdf", output_dir, DESCRIPTIONS["tab6"], figsize=(7, 4))

    if "tab7" in tables:
        plot_table(
            tables["tab7"],
            [ColumnDefinition(name=" ", textprops={"ha": "left"}, width=0.1),
             ColumnDefinition(name="Opt.", textprops={"ha": "left"}, width=0.8),
             ColumnDefinition(name="Measured Effect", textprops={"ha": "left"}, width=2.2),
             ColumnDefinition(name="Avg. Reduction", textprops={"ha": "center"}, width=1.1)],
            "tab7.pdf", output_dir, DESCRIPTIONS["tab7"])

    if "tab8" in tables:
        plot_table(
            tables["tab8"],
            [ColumnDefinition(name="Model", textprops={"ha": "left"}, width=1.0)]
            + [ColumnDefinition(name=OPT_LABELS[o], textprops={"ha": "center"},
                                width=0.85) for o in OPT_DIRS],
            "tab8.pdf", output_dir, DESCRIPTIONS["tab8"])


def generate_all_figures(model_entries, bench_entries, runtimes, speedups,
                         output_dir):
    # Fig 6: LReLU (-L) models; Fig 7: HReLU (-H) models
    h_models = [m for m in model_entries if "_x2" not in m]
    l_models = [m for m in model_entries if "_x2" in m]
    y_max = _max_speedup(h_models + l_models, speedups) * 1.10
    plot_runtime_fig(l_models, speedups, y_max, "fig6.pdf", output_dir,
                     DESCRIPTIONS["fig6"])
    plot_runtime_fig(h_models, speedups, y_max, "fig7.pdf", output_dir,
                     DESCRIPTIONS["fig7"])

    # Fig 8: combined micro-benchmark chart
    if bench_entries:
        cat_groups = {}
        for m in bench_entries:
            cat_groups.setdefault(_infer_category(m), []).append(m)
        _plot_micro_combined(cat_groups, speedups, str(output_dir),
                             description=DESCRIPTIONS["fig8"])
        src = output_dir / "micro_bench_combined.pdf"
        dst = output_dir / "fig8.pdf"
        if src.exists():
            os.replace(src, dst)
            logging.info(f"Saved: {dst}")


# ==============================================================================
# SECTION 4: MAIN
# ==============================================================================

def main():
    parser = argparse.ArgumentParser(
        description="Generate all HPAO Evaluation tables and figures (PDF).")
    parser.add_argument("-d", "--data-dir", type=Path,
                        default=Path("/app/ae_result/hpao"),
                        help="Directory containing model and/or micro-benchmark "
                             "logs (default: /app/ae_result/hpao)")
    parser.add_argument("-o", "--output-dir", type=Path,
                        default=Path("/app/ae_result"),
                        help="Directory for generated PDFs "
                             "(default: /app/ae_result)")
    args = parser.parse_args()

    base_dir = os.path.dirname(os.path.abspath(__file__))
    output_dir = args.output_dir
    output_dir.mkdir(parents=True, exist_ok=True)
    logging.info(f"Output will be saved to: {output_dir}")

    # Load everything once (model logs + micro-bench logs may be mixed)
    models, compile_times, runtimes = load_opt_data(base_dir, args.data_dir)
    model_entries, bench_entries = split_entries(models)
    logging.info(f"Found {len(model_entries)} models, "
                 f"{len(bench_entries)} micro-benchmarks")

    speedups = compute_speedup(runtimes)

    tables = {}
    if model_entries:
        tables["tab4"] = build_tab4(args.data_dir)
        tables["tab7"] = build_tab7(base_dir, args.data_dir)
        tables["tab8"] = build_tab8(compile_times, model_entries)
    if bench_entries:
        tables["tab5"] = build_tab5(runtimes, bench_entries)
        tables["tab6"] = build_tab6()

    generate_all_figures(model_entries, bench_entries, runtimes, speedups,
                         output_dir)
    generate_all_tables(tables, output_dir)

    logging.info(f"All artifacts written to {output_dir}")


if __name__ == "__main__":
    main()
