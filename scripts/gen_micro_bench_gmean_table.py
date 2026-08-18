#!/usr/bin/env python3
"""Generate LaTeX table of geometric-mean speedups over ANT-ACE on microbenchmarks.

Usage:
    python3 generate_micro_bench_gmean_table.py -d machine181/509_micro_bench_data
"""

import argparse
import math
import os

from gen_model_compile_runtime_tables import (
    load_opt_data,
    load_micro_bench_categories,
    compute_speedup,
    OPT_LABELS,
)

# Column order: FM before LM (swapped vs OPT_DIRS)
TABLE_OPT_KEYS = ["moduphoist", "moddownhoist", "fast_multiply", "lmr", "all"]

CATEGORY_ORDER = {"conv": 0, "avgpool": 1, "gemm": 2}
CATEGORY_LABELS = {"conv": "Conv", "avgpool": "AvgPool", "gemm": "GEMM"}

# LaTeX command names for column headers
LATEX_HEADERS = {
    "moduphoist": r"\HPAOMU",
    "moddownhoist": r"\HPAOMD",
    "fast_multiply": r"\HPAOFM",
    "lmr": r"\HPAOLM",
    "all": r"\ALL",
}


def arithmetic_mean(vals):
    """Compute arithmetic mean of a list of numbers."""
    return sum(vals) / len(vals)


def geometric_mean(vals):
    """Compute geometric mean of a list of positive numbers."""
    return math.exp(sum(math.log(v) for v in vals) / len(vals))


def compute_category_means(speedups, bench_models, categories):
    """Compute arithmetic- and geometric-mean speedup per (category, optimization).

    Returns:
        cat_ameans: {category: {opt_key: arithmetic_mean}}
        cat_gmeans: {category: {opt_key: geometric_mean}}
        all_bench_keys: list of all benchmark names with valid baseline data
    """
    # Group benchmarks by category
    cat_groups = {}
    for m in bench_models:
        cat = categories.get(m, "other")
        if cat in CATEGORY_ORDER:
            cat_groups.setdefault(cat, []).append(m)

    cat_ameans = {}
    cat_gmeans = {}
    for cat in sorted(cat_groups, key=lambda c: CATEGORY_ORDER[c]):
        cat_ameans[cat] = {}
        cat_gmeans[cat] = {}
        for opt in TABLE_OPT_KEYS:
            vals = [speedups[opt].get(m) for m in cat_groups[cat]]
            vals = [v for v in vals if v is not None and v > 0]
            cat_ameans[cat][opt] = arithmetic_mean(vals) if vals else None
            cat_gmeans[cat][opt] = geometric_mean(vals) if vals else None

    # All benchmark keys for Overall row
    all_bench_keys = []
    for cat in sorted(cat_groups, key=lambda c: CATEGORY_ORDER[c]):
        all_bench_keys.extend(cat_groups[cat])

    return cat_ameans, cat_gmeans, all_bench_keys


def format_val(val, latex=False):
    """Format a speedup value."""
    if val is None:
        return "--"
    if latex:
        return f"{val:.2f}$\\times$"
    return f"{val:.2f}×"


def print_plain_table(cat_ameans, cat_gmeans, overall_ameans, overall_gmeans):
    """Print a plain-text table to stdout with both arithmetic and geometric means."""
    # Geometric mean table
    print("=== Geometric Mean ===")
    headers = ["Type"] + [OPT_LABELS[o] for o in TABLE_OPT_KEYS]
    rows = [headers]
    for cat in sorted(cat_gmeans, key=lambda c: CATEGORY_ORDER[c]):
        label = CATEGORY_LABELS[cat]
        vals = [format_val(cat_gmeans[cat].get(o)) for o in TABLE_OPT_KEYS]
        rows.append([label] + vals)
    rows.append(["Overall"] + [format_val(overall_gmeans.get(o))
                                for o in TABLE_OPT_KEYS])

    col_widths = [max(len(r[i]) for r in rows) for i in range(len(headers))]
    fmt = "  ".join(f"{{:>{w}}}" for w in col_widths)
    for i, row in enumerate(rows):
        print(fmt.format(*row))
        if i == 0 or i == len(rows) - 2:
            print(fmt.format(*["-" * w for w in col_widths]))

    # Arithmetic mean table
    print("\n=== Arithmetic Mean ===")
    rows = [headers]
    for cat in sorted(cat_ameans, key=lambda c: CATEGORY_ORDER[c]):
        label = CATEGORY_LABELS[cat]
        vals = [format_val(cat_ameans[cat].get(o)) for o in TABLE_OPT_KEYS]
        rows.append([label] + vals)
    rows.append(["Overall"] + [format_val(overall_ameans.get(o))
                                for o in TABLE_OPT_KEYS])

    col_widths = [max(len(r[i]) for r in rows) for i in range(len(headers))]
    fmt = "  ".join(f"{{:>{w}}}" for w in col_widths)
    for i, row in enumerate(rows):
        print(fmt.format(*row))
        if i == 0 or i == len(rows) - 2:
            print(fmt.format(*["-" * w for w in col_widths]))


def print_latex_table(cat_ameans, cat_gmeans, overall_ameans, overall_gmeans):
    """Print a LaTeX-formatted table to stdout with both arithmetic and geometric means."""
    n_opts = len(TABLE_OPT_KEYS)
    col_spec = "l" + "c" * n_opts

    # Geometric mean table
    lines = [
        r"\begin{table}[t]",
        r"    \centering",
        r"    \caption{Geometric-mean speedups over ANT-ACE on workload-derived microbenchmarks.}",
        r"    \label{tab:micro-gmean}",
        r"    \footnotesize",
        r"    \setlength{\tabcolsep}{3pt}",
        f"    \\begin{{tabular}}{{{col_spec}}}",
        r"        \toprule",
    ]

    headers = ["Type"] + [LATEX_HEADERS[o] for o in TABLE_OPT_KEYS]
    lines.append("        " + " & ".join(headers) + r" \\")
    lines.append(r"        \midrule")

    for cat in sorted(cat_gmeans, key=lambda c: CATEGORY_ORDER[c]):
        label = CATEGORY_LABELS[cat]
        vals = [format_val(cat_gmeans[cat].get(o), latex=True)
                for o in TABLE_OPT_KEYS]
        lines.append("        " + " & ".join([label] + vals) + r" \\")

    lines.append(r"        \midrule")
    overall_vals = [format_val(overall_gmeans.get(o), latex=True)
                    for o in TABLE_OPT_KEYS]
    lines.append("        " + " & ".join(["Overall"] + overall_vals) + r" \\")

    lines.append(r"        \bottomrule")
    lines.append(r"    \end{tabular}")
    lines.append(r"\end{table}")

    # Arithmetic mean table
    lines.append("")
    lines.append(r"\begin{table}[t]")
    lines.append(r"    \centering")
    lines.append(r"    \caption{Arithmetic-mean speedups over ANT-ACE on workload-derived microbenchmarks.}")
    lines.append(r"    \label{tab:micro-amean}")
    lines.append(r"    \footnotesize")
    lines.append(r"    \setlength{\tabcolsep}{3pt}")
    lines.append(f"    \\begin{{tabular}}{{{col_spec}}}")
    lines.append(r"        \toprule")

    lines.append("        " + " & ".join(headers) + r" \\")
    lines.append(r"        \midrule")

    for cat in sorted(cat_ameans, key=lambda c: CATEGORY_ORDER[c]):
        label = CATEGORY_LABELS[cat]
        vals = [format_val(cat_ameans[cat].get(o), latex=True)
                for o in TABLE_OPT_KEYS]
        lines.append("        " + " & ".join([label] + vals) + r" \\")

    lines.append(r"        \midrule")
    overall_vals = [format_val(overall_ameans.get(o), latex=True)
                    for o in TABLE_OPT_KEYS]
    lines.append("        " + " & ".join(["Overall"] + overall_vals) + r" \\")

    lines.append(r"        \bottomrule")
    lines.append(r"    \end{tabular}")
    lines.append(r"\end{table}")

    print("\n".join(lines))


def main():
    parser = argparse.ArgumentParser(
        description="Generate LaTeX table of geometric-mean speedups over ANT-ACE "
                    "on microbenchmarks")
    parser.add_argument("-d", "--data-dir",
                        help="Path to data directory relative to script dir "
                             "(e.g. machine181/509_micro_bench_data)")
    parser.add_argument("-l", "--latex", action="store_true",
                        help="Print LaTeX-formatted table instead of plain text")
    args = parser.parse_args()

    base_dir = os.path.dirname(os.path.abspath(__file__))
    data_dir = args.data_dir or None

    models, _, runtimes = load_opt_data(base_dir, data_dir)
    speedups = compute_speedup(runtimes)

    # Only keep benchmarks with baseline runtime data
    bench_models = [m for m in models
                    if runtimes.get("conv_fast", {}).get(m) is not None]
    if not bench_models:
        print("No benchmark data found.")
        return

    categories = load_micro_bench_categories(base_dir, data_dir)

    cat_ameans, cat_gmeans, all_bench_keys = compute_category_means(
        speedups, bench_models, categories)

    # Overall means
    overall_ameans = {}
    overall_gmeans = {}
    for opt in TABLE_OPT_KEYS:
        vals = [speedups[opt].get(m) for m in all_bench_keys]
        vals = [v for v in vals if v is not None and v > 0]
        overall_ameans[opt] = arithmetic_mean(vals) if vals else None
        overall_gmeans[opt] = geometric_mean(vals) if vals else None

    if args.latex:
        print_latex_table(cat_ameans, cat_gmeans, overall_ameans, overall_gmeans)
    else:
        print_plain_table(cat_ameans, cat_gmeans, overall_ameans, overall_gmeans)


if __name__ == "__main__":
    main()
