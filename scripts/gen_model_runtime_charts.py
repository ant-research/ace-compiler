#!/usr/bin/env python3
"""Generate runtime bar charts for H and L model variants.

Produces two PDF charts:
  - runtime_H.pdf : models with high-degree polynomial ReLU
  - runtime_L.pdf : models with x^2 approximation ReLU

Each chart shows grouped bars per model: Baseline + 4 individual
optimisations + All.

Usage:
    python3 generate_runtime_charts.py
    python3 generate_runtime_charts.py -d machine66/507_model_data
"""

import argparse
import os
import re

import numpy as np
import matplotlib
import matplotlib.pyplot as plt

from gen_model_compile_runtime_tables import (
    load_opt_data,
    compute_speedup,
    _base_model_name,
    _paired_model_order,
    OPT_DIRS,
    OPT_LABELS,
)
from generate_tables import display_name

# Academic paper style
matplotlib.rcParams.update({
    "font.family": "sans-serif",
    "font.sans-serif": ["DejaVu Sans", "Helvetica", "Arial"],
    "font.weight": "bold",
    "font.size": 9,
    "axes.linewidth": 0.8,
    "xtick.major.width": 0.8,
    "ytick.major.width": 0.8,
})

OPT_COLORS = {
    "conv_fast": "#4472c4",
    "moduphoist": "#5b9bd5",
    "moddownhoist": "#70ad47",
    "lmr": "#ffc000",
    "fast_multiply": "#ed7d31",
    "all": "#c0392b",
}


class _CompressBelowOne(matplotlib.scale.ScaleBase):
    """Custom scale that compresses [0, 1) into a small band while keeping
    [1, ∞) linear.  This makes the below-baseline region visually small."""

    name = "compress_below_one"

    def __init__(self, axis, compress_ratio=0.12):
        super().__init__(axis)
        self.compress_ratio = compress_ratio  # fraction of axis for [0,1)

    def get_transform(self):
        return self._Transform(self.compress_ratio)

    class _Transform(matplotlib.transforms.Transform):
        input_dims = output_dims = 1

        def __init__(self, compress_ratio):
            super().__init__()
            self.compress_ratio = compress_ratio

        def transform_non_affine(self, values):
            out = np.empty_like(values, dtype=float)
            below = values < 1.0
            above = ~below
            # [0,1) → [0, compress_ratio)
            out[below] = values[below] * self.compress_ratio
            # [1,∞) → [compress_ratio, ∞)  (linear, offset)
            out[above] = self.compress_ratio + (values[above] - 1.0)
            return out

        def inverted(self):
            return _CompressBelowOne._InvertedTransform(self.compress_ratio)

    class _InvertedTransform(matplotlib.transforms.Transform):
        input_dims = output_dims = 1

        def __init__(self, compress_ratio):
            super().__init__()
            self.compress_ratio = compress_ratio

        def transform_non_affine(self, values):
            out = np.empty_like(values, dtype=float)
            below = values < self.compress_ratio
            above = ~below
            out[below] = values[below] / self.compress_ratio
            out[above] = 1.0 + (values[above] - self.compress_ratio)
            return out

        def inverted(self):
            return _CompressBelowOne._Transform(self.compress_ratio)

    def set_default_locators_and_formatters(self, axis):
        axis.set_major_locator(matplotlib.ticker.AutoLocator())
        axis.set_major_formatter(matplotlib.ticker.ScalarFormatter())


matplotlib.scale.register_scale(_CompressBelowOne)


def _max_speedup(model_keys, speedups):
    """Return the maximum speedup value across all models and opts."""
    vals = []
    for opt in OPT_DIRS:
        for m in model_keys:
            v = speedups[opt].get(m)
            if v is not None:
                vals.append(v)
    return max(vals) if vals else 1.0


def _draw_grouped_bars(ax, model_keys, speedups, y_max=None):
    """Draw grouped vertical bars for each model showing speedup over baseline."""
    opt_keys = OPT_DIRS  # Baseline + 4 opts + All
    opt_labels = [OPT_LABELS[o] for o in opt_keys]
    labels = []
    for m in model_keys:
        dname = display_name(m)
        if not dname.endswith(("-H", "-L")):
            dname += "-H" if "_x2" not in m else "-L"
        labels.append(dname)

    n = len(labels)
    n_opts = len(opt_keys)
    x = np.arange(n)
    bar_width = 0.8 / n_opts

    for i, opt in enumerate(opt_keys):
        vals = [speedups[opt].get(m, 0) for m in model_keys]
        offset = (i - n_opts / 2 + 0.5) * bar_width
        ax.bar(x + offset, vals, bar_width,
               label=opt_labels[i], color=OPT_COLORS[opt],
               edgecolor="white", linewidth=0.3)

    ax.set_xticks(x)
    ax.set_xticklabels(labels, fontsize=12, fontweight="bold", rotation=15, ha="right")
    ax.tick_params(axis='y', labelsize=12)
    for label in ax.get_yticklabels():
        label.set_fontweight("bold")
    ax.set_ylabel("Speedups", fontsize=14, fontweight="bold")
    ax.axhline(y=1.0, color='red', linewidth=1.2, linestyle=':', zorder=0)
    ax.set_yscale("compress_below_one")
    ax.set_ylim(bottom=0.8)
    if y_max is not None:
        ax.set_ylim(bottom=0.8, top=y_max)
    # Set explicit y-axis ticks with 1 decimal place
    ticks = [t for t in np.arange(1.0, y_max + 0.01, 0.5)]
    ticks.insert(0, 0.5)
    ticks.append(y_max)
    ax.yaxis.set_major_locator(matplotlib.ticker.FixedLocator(ticks))
    ax.yaxis.set_major_formatter(matplotlib.ticker.FormatStrFormatter('%.1f'))
    ax.margins(x=0.02)


def main():
    parser = argparse.ArgumentParser(
        description="Generate runtime bar charts for H and L model variants")
    parser.add_argument("-d", "--data-dir",
                        help="Path to data directory relative to script dir")
    args = parser.parse_args()

    base_dir = os.path.dirname(os.path.abspath(__file__))
    data_dir = args.data_dir or None
    _, _, runtimes = load_opt_data(base_dir, data_dir)
    speedups = compute_speedup(runtimes)

    # Get all model names that have runtime data
    all_models = sorted(m for m in runtimes.get("conv_fast", {})
                        if runtimes["conv_fast"].get(m) is not None)

    # Split into H and L
    h_models = [m for m in all_models if "_x2" not in m]
    l_models = [m for m in all_models if "_x2" in m]

    output_dir = os.path.join(base_dir, data_dir) if data_dir else base_dir

    # Compute shared y-axis max across both H and L, rounded up to a nice tick
    all_chart_models = h_models + l_models
    raw_max = _max_speedup(all_chart_models, speedups)
    # 10% headroom above max to avoid bar-legend overlap
    y_max = raw_max * 1.10

    if h_models:
        n = len(h_models)
        fig_h = max(2.5, n * 0.35 + 0.8)
        fig, ax = plt.subplots(1, 1, figsize=(9, fig_h))
        _draw_grouped_bars(ax, h_models, speedups, y_max=y_max)
        ax.legend(loc="upper center", bbox_to_anchor=(0.5, 1.02),
                  fontsize=10, ncol=6,
                  frameon=False,
                  columnspacing=0.8, handletextpad=0.3, handlelength=1.0,
                  borderpad=0.3,
                  prop={"weight": "bold"})
        fig.subplots_adjust(left=0.10, right=0.97, top=0.88, bottom=0.07)
        output_pdf = os.path.join(output_dir, "runtime_H.pdf")
        fig.savefig(output_pdf, format="pdf", bbox_inches="tight", pad_inches=0.01)
        plt.close(fig)
        print(f"Saved: {output_pdf}")

    if l_models:
        n = len(l_models)
        fig_h = max(2.5, n * 0.35 + 0.8)
        fig, ax = plt.subplots(1, 1, figsize=(9, fig_h))
        _draw_grouped_bars(ax, l_models, speedups, y_max=y_max)
        ax.legend(loc="upper center", bbox_to_anchor=(0.5, 1.02),
                  fontsize=10, ncol=6,
                  frameon=False,
                  columnspacing=0.8, handletextpad=0.3, handlelength=1.0,
                  borderpad=0.3,
                  prop={"weight": "bold"})
        fig.subplots_adjust(left=0.10, right=0.97, top=0.85, bottom=0.07)
        output_pdf = os.path.join(output_dir, "runtime_L.pdf")
        fig.savefig(output_pdf, format="pdf", bbox_inches="tight", pad_inches=0.01)
        plt.close(fig)
        print(f"Saved: {output_pdf}")


if __name__ == "__main__":
    main()