#!/usr/bin/env python3
"""Parse FHE compiler logs and generate bootstrap summary table.

For each model, the table shows:
  - Number of bootstraps
  - Bootstrap time (sum of BOOTSTRAP Elapse, excluding sub total)
  - Main graph time (MAIN_GRAPH Elapse)
  - Ratio of bootstrap time to main graph time

Usage:
    python3 generate_bootstrap_table.py
    python3 generate_bootstrap_table.py -d machine181/509_model_data
    python3 generate_bootstrap_table.py --latex
"""

import argparse
import glob
import os
import re

from generate_tables import model_name_from_path, display_name, OPT_SUFFIX_TO_DIR

def bootstrap_display_name(name, is_x2):
    """Return display name with explicit -H/-L suffix.

    The base display_name() only adds -H for models with _pre/_train suffixes.
    Models like 'lenet', 'vgg11_cifar10' lack those
    suffixes but are still the high-degree polynomial variant (-H).
    This wrapper ensures every model gets the appropriate -H or -L tag.
    """
    dname = display_name(name)
    if is_x2:
        # x2 models already get -L from display_name
        return dname
    # Non-x2 models: add -H if not already present
    if not dname.endswith("-H"):
        dname += "-H"
    return dname


def parse_bootstrap_info(filepath):
    """Parse a single log file, return dict with bootstrap and main_graph info."""
    with open(filepath) as f:
        content = f.read()

    if "RTLib functions" not in content:
        return None

    lines = content.splitlines()
    result = {
        "bootstrap_count": 0,
        "bootstrap_time": 0.0,
        "main_graph_time": 0.0,
    }

    for line in lines:
        # MAIN_GRAPH time (first line with count=1)
        m = re.match(r'^MAIN_GRAPH\s+1\s+([\d.]+)\s+sec', line)
        if m:
            result["main_graph_time"] = float(m.group(1))
            continue

        # BOOTSTRAP time (line with numeric count, not "sub total")
        m = re.match(r'^\s+BOOTSTRAP\s+(\d+)\s+([\d.]+)\s+sec', line)
        if m:
            result["bootstrap_count"] = int(m.group(1))
            result["bootstrap_time"] = float(m.group(2))
            continue

    return result


def load_bootstrap_data(base_dir=None, data_dir=None, opt_suffix="base"):
    """Load and parse all log files, return dict keyed by model name.

    Args:
        base_dir: root directory containing machine* folders
        data_dir: path to data directory relative to base_dir
                  (e.g. "machine181/509_model_data")
        opt_suffix: optimization suffix to load (default: "base")
    """
    if base_dir is None:
        base_dir = os.path.dirname(os.path.abspath(__file__))

    if data_dir:
        pattern = os.path.join(base_dir, data_dir, f"*.{opt_suffix}.log")
    else:
        pattern = os.path.join(base_dir, f"*.{opt_suffix}.log")
    log_files = sorted(glob.glob(pattern))

    models = {}
    for f in log_files:
        name = model_name_from_path(f)
        data = parse_bootstrap_info(f)
        if data is None:
            print(f"[SKIP] {name} — no RTLib data")
            continue
        models[name] = data

    return models


def split_groups(models):
    """Split models into non-x2 and x2 groups."""
    x2 = {k: v for k, v in models.items() if "_x2" in k}
    non_x2 = {k: v for k, v in models.items() if "_x2" not in k}
    return non_x2, x2


def print_table(title, rows, headers):
    """Print a formatted text table."""
    print(f"\n{'=' * 110}")
    print(f"  {title}")
    print(f"{'=' * 110}")
    widths = [max(len(str(row[i])) for row in [headers] + rows) for i in range(len(headers))]
    fmt = "  ".join(f"{{:<{w}}}" for w in widths)
    print(fmt.format(*headers))
    print("  ".join("-" * w for w in widths))
    for row in rows:
        print(fmt.format(*row))


def print_latex_table(title, models, label, is_x2):
    """Print a LaTeX-formatted bootstrap table using booktabs style."""
    print(f"\\begin{{table}}[t]")
    print(f"  \\caption{{{title}}}")
    print(f"  \\label{{{label}}}")
    print(f"  \\centering\\scriptsize")
    print(f"  \\begin{{tabular}}{{@{{}}l r r r r@{{}}}}")
    print(f"    \\toprule")
    print(f"    \\textbf{{Model}} & \\textbf{{\\#BS}} & \\textbf{{BS Time (s)}} & "
          f"\\textbf{{Graph Time (s)}} & \\textbf{{BS/Graph}} \\\\")
    print(f"    \\midrule")

    for name in sorted(models.keys()):
        d = models[name]
        dname = bootstrap_display_name(name, is_x2)
        bs_count = d["bootstrap_count"]
        bs_time = d["bootstrap_time"]
        mg_time = d["main_graph_time"]
        if mg_time > 0:
            ratio = bs_time / mg_time
            ratio_str = f"{ratio:.2%}"
        else:
            ratio_str = "---"
        bs_time_str = f"{bs_time:.2f}" if bs_time > 0 else "0"
        mg_time_str = f"{mg_time:.2f}" if mg_time > 0 else "0"
        print(f"    {dname} & {bs_count} & {bs_time_str} & {mg_time_str} & {ratio_str} \\\\")

    print(f"    \\bottomrule")
    print(f"  \\end{{tabular}}")
    print(f"\\end{{table}}")


def main():
    parser = argparse.ArgumentParser(
        description="Generate bootstrap summary table from FHE compiler logs")
    parser.add_argument("-d", "--data-dir",
                        help="Path to data directory relative to script dir "
                             "(e.g. machine181/509_model_data); "
                             "default: search *.log in script directory")
    parser.add_argument("-o", "--opt-suffix", default="base",
                        help="Optimization suffix to load (default: base). "
                             "Options: " + ", ".join(sorted(OPT_SUFFIX_TO_DIR.keys())))
    parser.add_argument("-l", "--latex", action="store_true",
                        help="Also print LaTeX-formatted table")
    args = parser.parse_args()

    base_dir = os.path.dirname(os.path.abspath(__file__))
    data_dir = args.data_dir or None
    models = load_bootstrap_data(base_dir, data_dir, args.opt_suffix)

    if not models:
        print("No model data found.")
        return

    non_x2, x2 = split_groups(models)

    headers = [
        "Model",
        "#Bootstraps",
        "BS Time (s)",
        "Graph Time (s)",
        "BS/Graph",
    ]

    for group_name, group, is_x2 in [
        ("Non-x2 Models (Higher-order Polynomial ReLU)", non_x2, False),
        ("x2 Models (x2 Approximation ReLU)", x2, True),
    ]:
        if not group:
            continue
        rows = []
        for name in sorted(group.keys()):
            d = group[name]
            bs_count = d["bootstrap_count"]
            bs_time = d["bootstrap_time"]
            mg_time = d["main_graph_time"]
            if mg_time > 0:
                ratio = bs_time / mg_time
                ratio_str = f"{ratio:.2%}"
            else:
                ratio_str = "N/A"
            bs_time_str = f"{bs_time:.2f}" if bs_time > 0 else "0.00"
            mg_time_str = f"{mg_time:.2f}" if mg_time > 0 else "0.00"
            rows.append([
                bootstrap_display_name(name, is_x2),
                str(bs_count),
                bs_time_str,
                mg_time_str,
                ratio_str,
            ])
        print_table(group_name, rows, headers)

    if args.latex:
        for group_name, group, is_x2 in [
            ("Bootstrap Summary — Non-x2 Models", non_x2, False),
            ("Bootstrap Summary — x2 Models", x2, True),
        ]:
            if not group:
                continue
            label = "tab:bootstrap-nonx2" if not is_x2 else "tab:bootstrap-x2"
            print()
            print_latex_table(group_name, group, label, is_x2)


if __name__ == "__main__":
    main()
