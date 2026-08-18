#!/usr/bin/env python3
"""Parse FHE compiler logs and generate modup/moddown summary tables."""

import glob
import os
import re


def parse_log(filepath):
    """Parse a single log file, return dict with modup/moddown counts and times."""
    with open(filepath) as f:
        content = f.read()

    if "RTLib functions" not in content:
        return None

    lines = content.splitlines()
    result = {
        "precomp_count": 0, "precomp_time": 0.0,
        "moddown_count": 0, "moddown_time": 0.0,
        "bs_precomp_count": 0, "bs_precomp_time": 0.0,
        "bs_moddown_count": 0, "bs_moddown_time": 0.0,
        "total_precomp_count": 0, "total_precomp_time": 0.0,
        "total_moddown_count": 0, "total_moddown_time": 0.0,
        "main_graph_time": 0.0,
        "bootstrap_time": 0.0,
        "dotprod_time": 0.0,
        "bs_dotprod_time": 0.0,
        "pt_encode_time": 0.0,
    }

    in_total_section = False
    for line in lines:
        # MAIN_GRAPH time (first line with count=1)
        m = re.match(r'^MAIN_GRAPH\s+1\s+([\d.]+)\s+sec', line)
        if m:
            result["main_graph_time"] = float(m.group(1))
            continue

        # BOOTSTRAP time (line with numeric count, not sub total)
        m = re.match(r'^\s+BOOTSTRAP\s+(\d+)\s+([\d.]+)\s+sec', line)
        if m:
            result["bootstrap_time"] = float(m.group(2))
            continue

        if "Total (non-bs + bs)" in line:
            in_total_section = True
            continue

        if in_total_section:
            m = re.match(r'^PRECOMP\s+(\d+)\s+([\d.]+)\s+sec', line)
            if m:
                result["total_precomp_count"] = int(m.group(1))
                result["total_precomp_time"] = float(m.group(2))
                continue
            m = re.match(r'^MOD_DOWN\s+(\d+)\s+([\d.]+)\s+sec', line)
            if m:
                result["total_moddown_count"] = int(m.group(1))
                result["total_moddown_time"] = float(m.group(2))
                continue
        else:
            m = re.match(r'^ PRECOMP\s+(\d+)\s+([\d.]+)\s+sec', line)
            if m:
                result["precomp_count"] = int(m.group(1))
                result["precomp_time"] = float(m.group(2))
                continue
            m = re.match(r'^ MOD_DOWN\s+(\d+)\s+([\d.]+)\s+sec', line)
            if m:
                result["moddown_count"] = int(m.group(1))
                result["moddown_time"] = float(m.group(2))
                continue
            m = re.match(r'^\s+BS_PRECOMP\s+(\d+)\s+([\d.]+)\s+sec', line)
            if m:
                result["bs_precomp_count"] = int(m.group(1))
                result["bs_precomp_time"] = float(m.group(2))
                continue
            m = re.match(r'^\s+BS_MOD_DOWN\s+(\d+)\s+([\d.]+)\s+sec', line)
            if m:
                result["bs_moddown_count"] = int(m.group(1))
                result["bs_moddown_time"] = float(m.group(2))
                continue
            # Non-bootstrap DOT_PROD (1-space indent)
            m = re.match(r'^ DOT_PROD\s+(\d+)\s+([\d.]+)\s+sec', line)
            if m:
                result["dotprod_time"] = float(m.group(2))
                continue
            # Non-bootstrap FAST_DOT_PROD (appears when lmr is enabled)
            m = re.match(r'^ FAST_DOT_PROD\s+(\d+)\s+([\d.]+)\s+sec', line)
            if m:
                result["dotprod_time"] = float(m.group(2))
                continue
            # Bootstrap BS_DOT_PROD
            m = re.match(r'^\s+BS_DOT_PROD\s+(\d+)\s+([\d.]+)\s+sec', line)
            if m:
                result["bs_dotprod_time"] = float(m.group(2))
                continue
            # PT_ENCODE (non-bootstrap only)
            m = re.match(r'^ PT_ENCODE\s+(\d+)\s+([\d.]+)\s+sec', line)
            if m:
                result["pt_encode_time"] = float(m.group(2))
                continue

    return result


# Opt suffixes used in flat-file naming
OPT_SUFFIXES = {"base", "modup", "moddown", "lmr", "fm_cte", "all"}

# Mapping from opt suffix to opt_dir name
OPT_SUFFIX_TO_DIR = {
    "base": "conv_fast",
    "modup": "moduphoist",
    "moddown": "moddownhoist",
    "lmr": "lmr",
    "fm_cte": "fast_multiply",
    "all": "all",
}


def model_name_from_path(filepath):
    basename = os.path.basename(filepath)
    # Format: model_name.opt_suffix.log (e.g. resnet20_cifar10_pre.base.log)
    m = re.match(r'^(.+)\.(' + '|'.join(OPT_SUFFIXES) + r')\.log$', basename)
    if m:
        return m.group(1)
    # Fallback: remove .log
    return basename.replace(".log", "")


def display_name(name):
    """Convert internal model name to display label.

    Model names with _x2 suffix get -L, those with _pre/_train get -H.
    Names without these suffixes (e.g. micro-benchmarks) are returned as-is.
    """
    is_x2 = "_x2" in name
    # Check if this looks like a model with H/L variants
    has_hl_suffix = is_x2 or re.search(r'_(?:pre|train)$', name)

    label = re.sub(r'_cifar10', '', name)
    if is_x2:
        label = re.sub(r'_x2$', '', label)
        label += "-L"
    elif has_hl_suffix:
        label = re.sub(r'_(?:pre|train)$', '', label)
        label += "-H"

    label = re.sub(r'resnet', 'ResNet', label)
    label = re.sub(r'vgg', 'VGG', label)
    label = re.sub(r'lenet', 'LeNet', label)
    return label


def load_all_models(base_dir=None, opt_dir="conv_fast", data_dir=None):
    """Load and parse all log files, return dict keyed by model name.

    Args:
        base_dir: root directory containing machine* folders
        opt_dir: optimization directory to load from (default: conv_fast baseline)
        data_dir: path to data directory relative to base_dir
                  (e.g. "machine66/507_model_data"); if None, search base_dir/*.log
    """
    if base_dir is None:
        base_dir = os.path.dirname(os.path.abspath(__file__))

    # Find the opt suffix that maps to the requested opt_dir
    target_suffix = None
    for suffix, odir in OPT_SUFFIX_TO_DIR.items():
        if odir == opt_dir:
            target_suffix = suffix
            break

    if target_suffix is None:
        return {}

    if data_dir:
        pattern = os.path.join(base_dir, data_dir, f"*.{target_suffix}.log")
    else:
        pattern = os.path.join(base_dir, f"*.{target_suffix}.log")
    log_files = sorted(glob.glob(pattern))

    models = {}
    for f in log_files:
        name = model_name_from_path(f)
        data = parse_log(f)
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
    print(f"\n{'=' * 100}")
    print(f"  {title}")
    print(f"{'=' * 100}")
    widths = [max(len(str(row[i])) for row in [headers] + rows) for i in range(len(headers))]
    fmt = "  ".join(f"{{:<{w}}}" for w in widths)
    print(fmt.format(*headers))
    print("  ".join("-" * w for w in widths))
    for row in rows:
        print(fmt.format(*row))


def main():
    models = load_all_models()
    non_x2, x2 = split_groups(models)

    headers = [
        "Model",
        "Non-BS ModUp", "Non-BS ModDown",
        "BS ModUp", "BS ModDown",
        "Total ModUp", "Total ModDown",
    ]

    for group_name, group in [
        ("Non-x2 Models (Higher-order Polynomial Approximation ReLU)", non_x2),
        ("x2 Models (x2 Approximation ReLU)", x2),
    ]:
        if not group:
            continue
        rows = []
        for name in sorted(group.keys()):
            d = group[name]
            rows.append([
                display_name(name),
                d["precomp_count"], d["moddown_count"],
                d["bs_precomp_count"], d["bs_moddown_count"],
                d["total_precomp_count"], d["total_moddown_count"],
            ])
        print_table(group_name, rows, headers)


if __name__ == "__main__":
    main()
