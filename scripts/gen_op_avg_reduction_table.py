#!/usr/bin/env python3
"""Generate a summary table of average operator-level improvements for each
optimization, comparing baseline (ANT-ACE) vs. individual optimizations.

Metrics:
  HPAO-MU:  reduction in poly.modup calls  (PRECOMP count)
  HPAO-MD:  reduction in poly.moddown calls (MOD_DOWN count)
  HPAO-FM:  reduction in poly.encode + eligible poly.mul execution time
            (PT_ENCODE + PT_GET + HW_MUL + MULP_FAST vs PT_ENCODE + HW_MUL)
  HPAO-LM:  reduction in poly.mul/poly.add time (DOT_PROD + BS_DOT_PROD time)

Usage:
    python3 generate_op_summary_table.py -d machine181/509_model_data
    python3 generate_op_summary_table.py -d machine181/509_model_data --latex
"""

import argparse
import glob
import math
import os
import re

from generate_tables import model_name_from_path, OPT_SUFFIX_TO_DIR

# Column order: FM before LM (consistent with other scripts)
TABLE_OPT_KEYS = ["moduphoist", "moddownhoist", "fast_multiply", "lmr"]

OPT_LABELS = {
    "conv_fast": "ANT-ACE",
    "moduphoist": "HPAO-MU",
    "moddownhoist": "HPAO-MD",
    "fast_multiply": "HPAO-FM",
    "lmr": "HPAO-LM",
    "all": "HPAO-ALL",
}

LATEX_HEADERS = {
    "moduphoist": r"\HPAOMU",
    "moddownhoist": r"\HPAOMD",
    "fast_multiply": r"\HPAOFM",
    "lmr": r"\HPAOLM",
}



def parse_rtlib(filepath):
    """Parse RTLib functions table from a log file.

    Returns a dict with operation counts and times, or None if no RTLib data.
    """
    with open(filepath) as f:
        content = f.read()

    if "RTLib functions" not in content:
        return None

    result = {
        "precomp_count": 0, "precomp_time": 0.0,
        "moddown_count": 0, "moddown_time": 0.0,
        "bs_precomp_count": 0, "bs_precomp_time": 0.0,
        "bs_moddown_count": 0, "bs_moddown_time": 0.0,
        "pt_encode_time": 0.0,
        "pt_encode_count": 0,
        "pt_get_time": 0.0,
        "pt_get_count": 0,
        "hw_mul_time": 0.0,
        "hw_mul_count": 0,
        "mulp_fast_time": 0.0,
        "mulp_fast_count": 0,
        "dotprod_time": 0.0,
        "bs_dotprod_time": 0.0,
    }

    lines = content.splitlines()
    in_total_section = False
    for line in lines:
        if "Total (non-bs + bs)" in line:
            in_total_section = True
            continue

        # Non-bootstrap PRECOMP (1-space indent)
        m = re.match(r'^ PRECOMP\s+(\d+)\s+([\d.]+)\s+sec', line)
        if m and not in_total_section:
            result["precomp_count"] = int(m.group(1))
            result["precomp_time"] = float(m.group(2))
            continue

        # Non-bootstrap MOD_DOWN (1-space indent)
        m = re.match(r'^ MOD_DOWN\s+(\d+)\s+([\d.]+)\s+sec', line)
        if m and not in_total_section:
            result["moddown_count"] = int(m.group(1))
            result["moddown_time"] = float(m.group(2))
            continue

        # Bootstrap BS_PRECOMP
        m = re.match(r'^\s+BS_PRECOMP\s+(\d+)\s+([\d.]+)\s+sec', line)
        if m and not in_total_section:
            result["bs_precomp_count"] = int(m.group(1))
            result["bs_precomp_time"] = float(m.group(2))
            continue

        # Bootstrap BS_MOD_DOWN
        m = re.match(r'^\s+BS_MOD_DOWN\s+(\d+)\s+([\d.]+)\s+sec', line)
        if m and not in_total_section:
            result["bs_moddown_count"] = int(m.group(1))
            result["bs_moddown_time"] = float(m.group(2))
            continue

        # PT_ENCODE
        m = re.match(r'^ PT_ENCODE\s+(\d+)\s+([\d.]+)\s+sec', line)
        if m:
            result["pt_encode_count"] = int(m.group(1))
            result["pt_encode_time"] = float(m.group(2))
            continue

        # PT_GET (appears when FM is enabled)
        m = re.match(r'^ PT_GET\s+(\d+)\s+([\d.]+)\s+sec', line)
        if m:
            result["pt_get_count"] = int(m.group(1))
            result["pt_get_time"] = float(m.group(2))
            continue

        # HW_MUL (non-bootstrap plain multiply)
        m = re.match(r'^ HW_MUL\s+(\d+)\s+([\d.]+)\s+sec', line)
        if m:
            result["hw_mul_count"] = int(m.group(1))
            result["hw_mul_time"] = float(m.group(2))
            continue

        # MULP_FAST (fast multiply, appears when FM is enabled)
        m = re.match(r'^ MULP_FAST\s+(\d+)\s+([\d.]+)\s+sec', line)
        if m:
            result["mulp_fast_count"] = int(m.group(1))
            result["mulp_fast_time"] = float(m.group(2))
            continue

        # DOT_PROD / FAST_DOT_PROD (non-bootstrap)
        m = re.match(r'^ (?:DOT_PROD|FAST_DOT_PROD)\s+(\d+)\s+([\d.]+)\s+sec', line)
        if m and not in_total_section:
            result["dotprod_time"] += float(m.group(2))
            continue

        # BS_DOT_PROD
        m = re.match(r'^\s+BS_DOT_PROD\s+(\d+)\s+([\d.]+)\s+sec', line)
        if m:
            result["bs_dotprod_time"] += float(m.group(2))
            continue

    # Total counts = non-bs + bs
    result["total_precomp_count"] = (result["precomp_count"]
                                     + result["bs_precomp_count"])
    result["total_moddown_count"] = (result["moddown_count"]
                                     + result["bs_moddown_count"])

    return result


def load_data(base_dir, data_dir=None):
    """Load RTLib data for all (model, optimization) pairs.

    Returns:
        models: sorted list of model names
        data: {opt_suffix: {model_name: rtlib_dict}}
    """
    all_suffixes = ["base", "modup", "moddown", "fm_cte", "lmr"]
    data = {s: {} for s in all_suffixes}
    all_models = set()

    for suffix in all_suffixes:
        if data_dir:
            pattern = os.path.join(base_dir, data_dir, f"*.{suffix}.log")
        else:
            pattern = os.path.join(base_dir, f"*.{suffix}.log")
        for fpath in sorted(glob.glob(pattern)):
            name = model_name_from_path(fpath)
            rtlib = parse_rtlib(fpath)
            if rtlib is not None:
                data[suffix][name] = rtlib
                all_models.add(name)

    return sorted(all_models), data


def compute_reductions(models, data):
    """Compute per-model reductions for each optimization metric.

    Returns a list of (opt_label, effect_desc, reductions) tuples where
    reductions is a list of percentage values (one per model).
    """
    rows = []

    # HPAO-MU: reduction in poly.modup calls (non-BS PRECOMP count)
    reductions = []
    for m in models:
        base = data["base"].get(m)
        opt = data["modup"].get(m)
        if base and opt and base["precomp_count"] > 0:
            red = 1.0 - opt["precomp_count"] / base["precomp_count"]
            reductions.append(red * 100)
    rows.append(("HPAO-MU", r"\texttt{poly.modup} calls", reductions))

    # HPAO-MD: reduction in poly.moddown calls (non-BS MOD_DOWN count)
    reductions = []
    for m in models:
        base = data["base"].get(m)
        opt = data["moddown"].get(m)
        if base and opt and base["moddown_count"] > 0:
            red = 1.0 - opt["moddown_count"] / base["moddown_count"]
            reductions.append(red * 100)
    rows.append(("HPAO-MD", r"\texttt{poly.moddown} calls", reductions))

    # HPAO-FM (merged): reduction in poly.encode/poly.mul execution time
    # Baseline: PT_ENCODE + HW_MUL time;
    # FM: PT_ENCODE + PT_GET + HW_MUL + MULP_FAST time
    reductions = []
    for m in models:
        base = data["base"].get(m)
        opt = data["fm_cte"].get(m)
        if base and opt:
            base_total = base["pt_encode_time"] + base["hw_mul_time"]
            opt_total = (opt["pt_encode_time"] + opt["pt_get_time"]
                         + opt["hw_mul_time"] + opt["mulp_fast_time"])
            if base_total > 0:
                red = 1.0 - opt_total / base_total
                reductions.append(red * 100)
    rows.append(("HPAO-FM",
                 r"\texttt{poly.encode} + eligible \texttt{poly.mul} "
                 r"execution time",
                 reductions))

    # HPAO-LM: reduction in poly.mul/poly.add time (DOT_PROD + BS_DOT_PROD)
    reductions = []
    for m in models:
        base = data["base"].get(m)
        opt = data["lmr"].get(m)
        if base and opt:
            base_total = base["dotprod_time"] + base["bs_dotprod_time"]
            opt_total = opt["dotprod_time"] + opt["bs_dotprod_time"]
            if base_total > 0:
                red = 1.0 - opt_total / base_total
                reductions.append(red * 100)
    rows.append(("HPAO-LM", r"\texttt{poly.mul}/\texttt{poly.add} execution time",
                 reductions))

    return rows


PLAIN_EFFECTS = {
    r"\texttt{poly.modup} calls": "poly.modup calls",
    r"\texttt{poly.moddown} calls": "poly.moddown calls",
    (r"\texttt{poly.encode} + eligible \texttt{poly.mul} "
     r"execution time"):
        "poly.encode + eligible poly.mul execution time",
    r"\texttt{poly.mul}/\texttt{poly.add} execution time": "poly.mul/poly.add execution time",
}


def arithmetic_mean(vals):
    """Compute arithmetic mean of a list of numbers."""
    return sum(vals) / len(vals)


def geometric_mean(vals):
    """Compute geometric mean of a list of positive numbers."""
    return math.exp(sum(math.log(v) for v in vals) / len(vals))


def fmt_pct(val):
    """Format a percentage value."""
    return f"{val:.2f}%"


def print_plain_table(rows):
    """Print a plain-text summary table with both arithmetic and geometric means."""
    headers = ["Opt.", "Measured Effect", "Arith. Mean", "Geom. Mean"]
    widths = [8, 50, 12, 12]
    fmt = "  ".join(f"{{:<{w}}}" for w in widths)
    print(fmt.format(*headers))
    print("  ".join("-" * w for w in widths))
    for opt, effect, reductions in rows:
        plain_effect = PLAIN_EFFECTS.get(effect, effect)
        if reductions:
            amean = arithmetic_mean(reductions)
            # Geometric mean of reductions: convert back from percentage,
            # compute gmean of (1 - reduction/100), then convert to percentage
            ratios = [(100 - r) / 100 for r in reductions]
            gmean = (1.0 - geometric_mean(ratios)) * 100
            print(fmt.format(opt, plain_effect, fmt_pct(amean), fmt_pct(gmean)))
        else:
            print(fmt.format(opt, plain_effect, "N/A", "N/A"))


def print_latex_table(rows):
    """Print a LaTeX-formatted summary table with both arithmetic and geometric means."""
    lines = [
        r"\begin{table}[t]",
        r"    \centering",
        r"    \caption{Average operator-level improvements across all evaluated "
        r"model variants (7 models $\times$ 2 polynomial ReLU settings). "
        r"For \HPAOLM, the reported result includes both compiler- and "
        r"bootstrap-level lazy-reduction optimizations.}",
        r"    \label{tab:op-summary}",
        r"    \small",
        r"    \setlength{\tabcolsep}{1pt}",
        r"    \begin{tabular}{l l cc}",
        r"        \toprule",
        r"        \textbf{Opt.} & \textbf{Measured Effect} "
        r"& \textbf{Arith. Mean} & \textbf{Geom. Mean} \\",
        r"        \midrule",
    ]

    for opt, effect, reductions in rows:
        if reductions:
            amean = arithmetic_mean(reductions)
            ratios = [(100 - r) / 100 for r in reductions]
            gmean = (1.0 - geometric_mean(ratios)) * 100
            lines.append(
                f"        {LATEX_HEADERS.get(_opt_key(opt), opt)} & "
                f"{effect} & {amean:.2f}\\% & {gmean:.2f}\\% \\\\"
            )
        else:
            lines.append(
                f"        {LATEX_HEADERS.get(_opt_key(opt), opt)} & "
                f"{effect} & -- & -- \\\\"
            )

    lines.append(r"        \bottomrule")
    lines.append(r"    \end{tabular}")
    lines.append(r"\end{table}")

    print("\n".join(lines))


def _opt_key(label):
    """Map OPT_LABELS value back to the key for LATEX_HEADERS lookup."""
    for k, v in OPT_LABELS.items():
        if v == label:
            return k
    return label


def main():
    parser = argparse.ArgumentParser(
        description="Generate operator-level improvement summary table")
    parser.add_argument("-d", "--data-dir",
                        help="Path to data directory relative to script dir "
                             "(e.g. machine181/509_model_data)")
    parser.add_argument("-l", "--latex", action="store_true",
                        help="Print LaTeX-formatted table instead of plain text")
    args = parser.parse_args()

    base_dir = os.path.dirname(os.path.abspath(__file__))
    data_dir = args.data_dir or None

    models, data = load_data(base_dir, data_dir)
    if not models:
        print("No model data found.")
        return

    rows = compute_reductions(models, data)

    if args.latex:
        print_latex_table(rows)
    else:
        print_plain_table(rows)


if __name__ == "__main__":
    main()