#!/usr/bin/env python3
"""Parse FHE compiler logs across optimization options and generate
compile-time / runtime comparison tables and a runtime bar chart.

Usage:
    python3 generate_opt_tables.py              # tables only
    python3 generate_opt_tables.py --chart       # tables + bar chart PDF
"""

import argparse
import glob
import os
import re
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
from generate_tables import model_name_from_path, display_name, OPT_SUFFIX_TO_DIR

# Academic paper style
matplotlib.rcParams.update({
    "font.family": "serif",
    "font.serif": ["Times New Roman", "Times", "DejaVu Serif"],
    "axes.linewidth": 0.8,
    "xtick.major.width": 0.8,
    "ytick.major.width": 0.8,
})

OPT_DIRS = ["conv_fast", "moduphoist", "moddownhoist", "fast_multiply", "lmr", "all"]
OPT_LABELS = {
    "conv_fast": "ANT-ACE",
    "moduphoist": "HPAO-MU",
    "moddownhoist": "HPAO-MD",
    "lmr": "HPAO-LM",
    "fast_multiply": "HPAO-FM",
    "all": "HPAO-ALL",
}


def parse_compile_time(filepath):
    """Extract compile time from 'COMPILE CMD TIME:Xs' line."""
    with open(filepath) as f:
        for line in f:
            m = re.match(r'^COMPILE CMD TIME[:\s]*([\d.]+)s', line)
            if m:
                return float(m.group(1))
    return None


def parse_runtime(filepath):
    """Extract MAIN_GRAPH runtime from 'MAIN_GRAPH  1  X sec' line."""
    with open(filepath) as f:
        for line in f:
            m = re.match(r'^MAIN_GRAPH\s+1\s+([\d.]+)\s+sec', line)
            if m:
                return float(m.group(1))
    return None


def load_opt_data(base_dir, data_dir=None):
    """Load compile time and runtime for every (model, optimization) pair.

    Args:
        base_dir: root directory (script's directory)
        data_dir: path to data directory relative to base_dir
                  (e.g. "machine66/508_micro_benchmark_data");
                  if None, search base_dir/*.log directly

    Returns:
        models: sorted list of model names
        compile_times: {opt: {model: time}}
        runtimes: {opt: {model: time}}
    """
    compile_times = {opt: {} for opt in OPT_DIRS}
    runtimes = {opt: {} for opt in OPT_DIRS}
    all_models = set()

    for opt in OPT_DIRS:
        suffix = None
        for s, odir in OPT_SUFFIX_TO_DIR.items():
            if odir == opt:
                suffix = s
                break
        if suffix is None:
            continue
        if data_dir:
            pattern = os.path.join(base_dir, data_dir, f"*.{suffix}.log")
        else:
            pattern = os.path.join(base_dir, f"*.{suffix}.log")
        for fpath in glob.glob(pattern):
            name = model_name_from_path(fpath)
            ct = parse_compile_time(fpath)
            rt = parse_runtime(fpath)
            if ct is not None:
                compile_times[opt][name] = ct
            if rt is not None:
                runtimes[opt][name] = rt
            all_models.add(name)

    models = sorted(all_models)
    return models, compile_times, runtimes


def _base_model_name(name):
    """Strip _x2, _pre, _train suffixes to get base model name."""
    return re.sub(r'_(x2|pre|train)$', '', name)


def load_micro_bench_categories(base_dir, data_dir=None):
    """Parse micro_bench file, return {benchmark_name: category}.

    When data_dir is given (e.g. "machine66/508_micro_benchmark_data"),
    look for micro_bench in the same parent directory
    (e.g. "machine66/micro_bench"). Otherwise look in base_dir directly.

    File format:
        conv:
        i16x32x32_ci16_co16_k3p1s1
        ...
        avgpool:
        i128x16x16_k2p0s2
        ...
    """
    categories = {}
    if data_dir:
        micro_bench_dir = os.path.dirname(data_dir)
        pattern = os.path.join(base_dir, micro_bench_dir, "micro_bench")
    else:
        pattern = os.path.join(base_dir, "micro_bench")
    for fpath in sorted(glob.glob(pattern)):
        current_cat = None
        with open(fpath) as f:
            for line in f:
                line = line.strip()
                if not line:
                    continue
                if line.endswith(":"):
                    current_cat = line[:-1]
                elif current_cat:
                    categories[line] = current_cat
    return categories


def _paired_model_order(models):
    """Return list of (base_name, h_key, l_key) grouping H/L together."""
    h_models = {_base_model_name(m): m for m in models if "_x2" not in m}
    l_models = {_base_model_name(m): m for m in models if "_x2" in m}
    bases = sorted(set(list(h_models.keys()) + list(l_models.keys())))
    return [(b, h_models.get(b), l_models.get(b)) for b in bases]


def _fmt_val(val):
    """Format a numeric value for display."""
    if val is None:
        return "—"
    return f"{val:.2f}"


def _fmt_pct_increase(val):
    """Format a ratio as percentage increase over baseline, e.g. 1.10 -> '+10.00%'."""
    if val is None:
        return "—"
    pct = (val - 1) * 100
    return f"{pct:+.2f}%"


def compute_speedup(runtimes, baseline_key="conv_fast"):
    """Convert absolute runtimes to speedup over baseline.

    Returns new dict {opt: {model: speedup}} where speedup = baseline / opt.
    Baseline column values are 1.00 (or None if missing).
    """
    speedups = {opt: {} for opt in runtimes}
    for opt in runtimes:
        for model, t in runtimes[opt].items():
            base = runtimes[baseline_key].get(model)
            if base and t and t > 0:
                speedups[opt][model] = base / t
            else:
                speedups[opt][model] = None
    return speedups


def _compute_averages(pairs, data, baseline_key="conv_fast", ratio_avg=False):
    """Compute arithmetic- and geometric-mean for -L, -H, and All groups.

    When ratio_avg=False (default): averages are computed directly from data values
    (e.g. for speedup tables where data is already baseline/opt).

    When ratio_avg=True: each value is first divided by its baseline value
    (opt/baseline), then averaged. This is used for compile time where we want
    the average overhead ratio (opt_time / baseline_time).

    Returns tuple: (ameans, gmeans) where each is a dict
    {"-L": [...], "-H": [...], "All": [...]} with one value per OPT_DIRS entry
    (None if no valid data).
    """
    import math

    groups = {"-L": [], "-H": []}
    for base, h_key, l_key in pairs:
        if l_key is not None:
            groups["-L"].append(l_key)
        if h_key is not None:
            groups["-H"].append(h_key)
    groups["All"] = groups["-L"] + groups["-H"]

    ameans = {}
    gmeans = {}
    for label, keys in groups.items():
        a_row = []
        g_row = []
        for opt in OPT_DIRS:
            if ratio_avg:
                # Compute opt/baseline ratio per model, then average
                ratios = []
                for k in keys:
                    v = data[opt].get(k)
                    base = data[baseline_key].get(k)
                    if v is not None and base is not None and base > 0:
                        ratios.append(v / base)
                if ratios:
                    a_row.append(sum(ratios) / len(ratios))
                    g_row.append(math.exp(sum(math.log(r) for r in ratios) / len(ratios)))
                else:
                    a_row.append(None)
                    g_row.append(None)
            else:
                vals = [data[opt].get(k) for k in keys]
                vals = [v for v in vals if v is not None and v > 0]
                if vals:
                    a_row.append(sum(vals) / len(vals))
                    g_row.append(math.exp(sum(math.log(v) for v in vals) / len(vals)))
                else:
                    a_row.append(None)
                    g_row.append(None)
        ameans[label] = a_row
        gmeans[label] = g_row
    return ameans, gmeans


def print_comparison_table(title, models, data, baseline_key="conv_fast",
                          flat=False, categories=None, show_avg=False,
                          ratio_avg=False):
    """Print a comparison table. If flat=True, list models without H/L pairing.
    categories: optional {name: category} dict for flat mode (adds Type column).
    show_avg: if True, append average rows for -L, -H, and All (non-flat only).
    """
    if flat:
        headers = ["Benchmark"]
        if categories:
            headers.append("Type")
        headers += [OPT_LABELS[o] for o in OPT_DIRS]
        rows = []
        for name in models:
            row = [display_name(name)]
            if categories:
                row.append(categories.get(name, ""))
            for opt in OPT_DIRS:
                row.append(_fmt_val(data[opt].get(name)))
            rows.append(row)
    else:
        pairs = _paired_model_order(models)
        headers = ["Model", "Type"] + [OPT_LABELS[o] for o in OPT_DIRS]
        rows = []
        # All -L models first
        for base, h_key, l_key in pairs:
            if l_key is None:
                continue
            dname = display_name(l_key).rsplit("-", 1)[0]
            row = [dname, "-L"]
            for opt in OPT_DIRS:
                row.append(_fmt_val(data[opt].get(l_key)))
            rows.append(row)
        # Then all -H models
        for base, h_key, l_key in pairs:
            if h_key is None:
                continue
            dname = display_name(h_key).rsplit("-", 1)[0]
            row = [dname, "-H"]
            for opt in OPT_DIRS:
                row.append(_fmt_val(data[opt].get(h_key)))
            rows.append(row)
        # Average rows
        if show_avg:
            ameans, gmeans = _compute_averages(pairs, data,
                                               baseline_key=baseline_key,
                                               ratio_avg=ratio_avg)
            fmt_fn = _fmt_pct_increase if ratio_avg else _fmt_val
            for label in ["-L", "-H", "All"]:
                row = [f"Avg({label})", ""]
                for v in ameans[label]:
                    row.append(fmt_fn(v))
                rows.append(row)
            for label in ["-L", "-H", "All"]:
                row = [f"GMean({label})", ""]
                for v in gmeans[label]:
                    row.append(fmt_fn(v))
                rows.append(row)

    print(f"\n{'=' * 120}")
    print(f"  {title}")
    print(f"{'=' * 120}")
    widths = [max(len(str(r[i])) for r in [headers] + rows) for i in range(len(headers))]
    fmt = "  ".join(f"{{:<{w}}}" for w in widths)
    print(fmt.format(*headers))
    print("  ".join("-" * w for w in widths))
    for row in rows:
        print(fmt.format(*row))


def print_latex_table(title, models, data, label, flat=False, categories=None,
                      ratio_avg=False):
    """Print a LaTeX-formatted table using booktabs style.
    If flat=True, list without H/L pairing.
    categories: optional {name: category} dict for flat mode.
    """
    n_cols = len(OPT_DIRS) + (2 if flat and categories else 1)
    col_spec = "@{}l " + "c" * (n_cols - 1) + "@{}"
    header_cells = [f"\\textbf{{{OPT_LABELS[o]}}}" for o in OPT_DIRS]

    if flat:
        if categories:
            header = " & ".join(["\\textbf{Benchmark}", "\\textbf{Type}"] + header_cells)
        else:
            header = " & ".join(["\\textbf{Benchmark}"] + header_cells)
    else:
        header = " & ".join(["\\textbf{Model}"] + header_cells)

    print(f"\\begin{{table}}[t]")
    print(f"  \\caption{{{title}}}")
    print(f"  \\label{{{label}}}")
    print(f"  \\centering\\scriptsize")
    print(f"  \\begin{{tabular}}{{{col_spec}}}")
    print(f"    \\toprule")
    print(f"    {header} \\\\")
    print(f"    \\midrule")

    if flat:
        for name in models:
            dname = display_name(name)
            vals = []
            if categories:
                vals.append(categories.get(name, ""))
            for opt in OPT_DIRS:
                v = data[opt].get(name)
                vals.append(f"{v:.2f}" if v is not None else "---")
            cells = " & ".join(vals)
            print(f"    {dname} & {cells} \\\\")
    else:
        pairs = _paired_model_order(models)
        # All -L models first
        for base, h_key, l_key in pairs:
            if l_key is None:
                continue
            dname = display_name(l_key)
            vals = []
            for opt in OPT_DIRS:
                v = data[opt].get(l_key)
                vals.append(f"{v:.2f}" if v is not None else "---")
            cells = " & ".join(vals)
            print(f"    {dname} & {cells} \\\\")
        print(f"    \\midrule")
        # Then all -H models
        for base, h_key, l_key in pairs:
            if h_key is None:
                continue
            dname = display_name(h_key)
            if not dname.endswith("-H"):
                dname += "-H"
            vals = []
            for opt in OPT_DIRS:
                v = data[opt].get(h_key)
                vals.append(f"{v:.2f}" if v is not None else "---")
            cells = " & ".join(vals)
            print(f"    {dname} & {cells} \\\\")
        # Average rows
        ameans, gmeans = _compute_averages(pairs, data, ratio_avg=ratio_avg)
        print(f"    \\midrule")
        for label in ["-L", "-H", "All"]:
            vals = []
            for v in ameans[label]:
                if v is None:
                    vals.append("---")
                elif ratio_avg:
                    pct = (v - 1) * 100
                    vals.append(f"{pct:+.2f}\\%")
                else:
                    vals.append(f"{v:.2f}")
            cells = " & ".join(vals)
            print(f"    Avg({label}) & {cells} \\\\")
        for label in ["-L", "-H", "All"]:
            vals = []
            for v in gmeans[label]:
                if v is None:
                    vals.append("---")
                elif ratio_avg:
                    pct = (v - 1) * 100
                    vals.append(f"{pct:+.2f}\\%")
                else:
                    vals.append(f"{v:.2f}")
            cells = " & ".join(vals)
            print(f"    GMean({label}) & {cells} \\\\")

    print(f"    \\bottomrule")
    print(f"  \\end{{tabular}}")
    print(f"\\end{{table}}")


def _draw_speedup_bars(ax, model_names, runtimes, categories=None):
    """Draw grouped horizontal bars showing speedup vs baseline."""
    # Skip baseline — only show optimizations that differ
    opt_keys = [o for o in OPT_DIRS if o != "conv_fast"]
    opt_labels = [OPT_LABELS[o] for o in opt_keys]
    labels = []
    for m in model_names:
        dname = display_name(m)
        if categories and categories.get(m):
            labels.append(f"[{categories[m]}] {dname}")
        else:
            labels.append(dname)
    n = len(labels)
    n_opts = len(opt_keys)
    x = np.arange(n)
    bar_width = 0.85 / n_opts
    colors = ["#1f4e79", "#7bafd4", "#2ca02c", "#c03d2f", "#e8825a"]

    for i, opt in enumerate(opt_keys):
        vals = []
        for m in model_names:
            base = runtimes["conv_fast"].get(m)
            v = runtimes[opt].get(m)
            if base and v and v > 0:
                vals.append(base / v)
            else:
                vals.append(0)
        y_pos = x + i * bar_width
        ax.barh(y_pos, vals, bar_width,
                label=opt_labels[i], color=colors[i],
                edgecolor="white", linewidth=0.3)

    ax.set_yticks(x + bar_width * (n_opts - 1) / 2)
    ax.set_yticklabels(labels, fontsize=6.5)
    ax.tick_params(axis='x', labelsize=6)
    ax.tick_params(axis='y', pad=1)
    ax.axvline(x=1.0, color='gray', linewidth=0.5, linestyle='--', zorder=0)
    ax.invert_yaxis()
    ax.margins(y=0.01)
    return opt_keys


def plot_runtime_charts(models, runtimes, base_dir, flat=False, categories=None,
                        data_dir=None):
    """Generate speedup chart. If flat=True, single plot; else H/L side-by-side."""
    if flat:
        bench_models = [m for m in models
                        if runtimes["conv_fast"].get(m) is not None]
        if not bench_models:
            return
        n = len(bench_models)
        fig_h = max(2.0, n * 0.22 + 0.5)
        fig, ax = plt.subplots(1, 1, figsize=(7, fig_h))
        _draw_speedup_bars(ax, bench_models, runtimes, categories)
        ax.set_title("Micro-benchmark Runtime Speedup", fontsize=7, pad=2)
        fig.subplots_adjust(left=0.12, right=0.98, top=0.95, bottom=0.06)
        fig.text(0.5, -0.04, "Speedup over baseline", ha="center", fontsize=7)
        handles, legend_labels = ax.get_legend_handles_labels()
        fig.legend(handles, legend_labels, loc="lower center",
                   bbox_to_anchor=(0.5, -0.12), fontsize=6, ncol=5,
                   framealpha=1, edgecolor="lightgray",
                   columnspacing=0.5, handletextpad=0.2, handlelength=1.0,
                   handleheight=0.8, borderpad=0.2)
        output_dir = os.path.join(base_dir, data_dir) if data_dir else base_dir
        output_pdf = os.path.join(output_dir, "micro_bench_speedup.pdf")
    else:
        h_models = [m for m in models if "_x2" not in m
                    and runtimes["conv_fast"].get(m) is not None]
        l_models = [m for m in models if "_x2" in m
                    and runtimes["conv_fast"].get(m) is not None]
        n_max = max(len(h_models), len(l_models))
        if n_max == 0:
            return
        fig_h = max(2.0, n_max * 0.22 + 0.5)
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(7, fig_h), sharey=False)
        _draw_speedup_bars(ax1, h_models, runtimes)
        ax1.set_title("(a) Models with high-degree polynomial ReLU", fontsize=7, pad=2)
        _draw_speedup_bars(ax2, l_models, runtimes)
        ax2.set_title("(b) Models with $x^2$ approximation ReLU", fontsize=7, pad=2)
        fig.subplots_adjust(left=0.12, right=0.98, top=0.95, bottom=0.06,
                            wspace=0.35)
        fig.text(0.5, -0.04, "Speedup over baseline", ha="center", fontsize=7)
        handles, legend_labels = ax1.get_legend_handles_labels()
        fig.legend(handles, legend_labels, loc="lower center",
                   bbox_to_anchor=(0.5, -0.12), fontsize=6, ncol=5,
                   framealpha=1, edgecolor="lightgray",
                   columnspacing=0.5, handletextpad=0.2, handlelength=1.0,
                   handleheight=0.8, borderpad=0.2)
        output_dir = os.path.join(base_dir, data_dir) if data_dir else base_dir
        output_pdf = os.path.join(output_dir, "runtime_speedup.pdf")

    fig.savefig(output_pdf, format="pdf", bbox_inches="tight", pad_inches=0.03)
    plt.close(fig)
    print(f"\nSaved: {output_pdf}")


def main():
    parser = argparse.ArgumentParser(
        description="Generate optimization comparison tables and charts")
    parser.add_argument("-d", "--data-dir",
                        help="Path to data directory relative to script dir "
                             "(e.g. machine66/508_micro_benchmark_data); "
                             "default: search *.log in script directory")
    parser.add_argument("-c", "--compile-time", action="store_true",
                        help="Show compile time comparison table")
    parser.add_argument("-r", "--runtime", action="store_true",
                        help="Show runtime comparison table (default unless -c is used alone)")
    parser.add_argument("-p", "--chart", action="store_true",
                        help="Also generate runtime bar chart PDF")
    parser.add_argument("-s", "--speedup", action="store_true",
                        help="Show runtime as speedup over baseline instead of seconds")
    parser.add_argument("-l", "--latex", action="store_true",
                        help="Also print LaTeX-formatted tables")
    args = parser.parse_args()

    base_dir = os.path.dirname(os.path.abspath(__file__))
    data_dir = args.data_dir or None
    models, compile_times, runtimes = load_opt_data(base_dir, data_dir)

    # Flat mode: no H/L pairing (micro-benchmarks have no _x2/_pre/_train suffixes)
    flat = not any(("_x2" in m or re.search(r'_(?:pre|train)$', m)) for m in models)

    # Load micro-benchmark categories if available
    categories = {}
    if flat:
        categories = load_micro_bench_categories(base_dir, data_dir)
        # Sort by (category, name) so benchmarks group by type
        cat_order = {"conv": 0, "avgpool": 1, "gemm": 2}
        models = sorted(models, key=lambda m: (cat_order.get(categories.get(m, ""), 99), m))

    show_runtime = args.runtime or not args.compile_time

    if args.compile_time:
        print_comparison_table(
            "Compile Time Comparison (seconds)",
            models, compile_times, flat=flat, categories=categories if flat else None,
            show_avg=not flat, ratio_avg=True)
        if args.latex:
            print_latex_table(
                "Compile Time", models, compile_times, "tab:comptime",
                flat=flat, categories=categories if flat else None,
                ratio_avg=True)

    if show_runtime:
        if args.speedup:
            speedups = compute_speedup(runtimes)
            print_comparison_table(
                "Runtime Speedup over Baseline (×)",
                models, speedups, flat=flat, categories=categories if flat else None,
                show_avg=not flat)
            if args.latex:
                print_latex_table(
                    "Runtime Speedup", models, speedups, "tab:speedup",
                    flat=flat, categories=categories if flat else None)
        else:
            print_comparison_table(
                "Runtime Comparison (seconds)",
                models, runtimes, flat=flat, categories=categories if flat else None)
            if args.latex:
                print_latex_table(
                    "Runtime", models, runtimes, "tab:runtime",
                    flat=flat, categories=categories if flat else None)

    if args.chart:
        plot_runtime_charts(models, runtimes, base_dir, flat=flat,
                            categories=categories if flat else None,
                            data_dir=data_dir)


if __name__ == "__main__":
    main()
