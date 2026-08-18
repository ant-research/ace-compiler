#!/usr/bin/env python3
"""Generate bar charts for micro-benchmark categories (Conv, AvgPool, GEMM).

By default produces one PDF per category.  With --combined, produces a
single PDF with three vertically stacked subplots sharing one legend,
suitable for a two-column paper figure*.

Usage:
    python3 generate_micro_bench_charts.py -d machine181/509_micro_bench_data
    python3 generate_micro_bench_charts.py -d machine181/509_micro_bench_data --combined
"""

import argparse
import os

import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.ticker

from gen_model_compile_runtime_tables import (
    load_opt_data,
    load_micro_bench_categories,
    compute_speedup,
    OPT_DIRS,
    OPT_LABELS,
)

# Academic paper style — consistent with generate_runtime_charts.py
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

# Display order and titles for categories
CATEGORY_ORDER = {"conv": 0, "avgpool": 1, "gemm": 2}
CATEGORY_TITLES = {
    "conv": "Convolution",
    "avgpool": "Average Pooling",
    "gemm": "GEMM",
}

# Short labels for micro-benchmarks (used in chart x-axis)
BENCH_LABELS = {
    # Convolution
    "i16x32x32_ci16_co16_k3p1s1": "C1",
    "i16x32x32_ci16_co32_k1p0s2": "C2",
    "i16x32x32_ci16_co32_k3p1s2": "C3",
    "i32x16x16_ci32_co32_k3p1s1": "C4",
    "i32x16x16_ci32_co64_k1p0s2": "C5",
    "i32x16x16_ci32_co64_k3p1s2": "C6",
    "i64x16x16_ci64_co128_k3p1s1": "C7",
    "i64x8x8_ci64_co64_k3p1s1": "C8",
    # Average Pooling
    "i128x16x16_k2p0s2": "P1",
    "i256x8x8_k2p0s2": "P2",
    "i512x4x4_k2p0s2": "P3",
    # GEMM
    "i1024_o4096": "G1",
    "i4096_o10": "G2",
    "i512_o10": "G3",
    "i64_o10": "G4",
}


class _CompressBelowOne(matplotlib.scale.ScaleBase):
    """Custom scale that compresses [0, 1) into a small band while keeping
    [1, ∞) linear.  This makes the below-baseline region visually small."""

    name = "compress_below_one"

    def __init__(self, axis, compress_ratio=0.12):
        super().__init__(axis)
        self.compress_ratio = compress_ratio

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
            out[below] = values[below] * self.compress_ratio
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


def _max_speedup_in(bench_keys, speedups):
    """Return the maximum speedup value for the given benchmark keys."""
    vals = []
    for opt in OPT_DIRS:
        for m in bench_keys:
            v = speedups[opt].get(m)
            if v is not None:
                vals.append(v)
    return max(vals) if vals else 1.0


def _draw_grouped_bars(ax, bench_keys, speedups, y_max=None,
                       show_ylabel=True, show_legend=False,
                       xtick_fontsize=12, ytick_fontsize=12,
                       ylabel_fontsize=14, bar_group_width=0.8):
    """Draw grouped vertical bars for benchmarks showing speedup over baseline.

    Font sizes default to the same values used in generate_runtime_charts.py.

    Args:
        show_ylabel: if False, hide the y-axis label (for shared-y subplots)
        show_legend: if True, draw the legend on this axes
        xtick_fontsize: font size for x-axis tick labels
        ytick_fontsize: font size for y-axis tick labels
        ylabel_fontsize: font size for the y-axis label
        bar_group_width: total width of a group of bars (per x-tick)
    """
    opt_keys = OPT_DIRS
    opt_labels = [OPT_LABELS[o] for o in opt_keys]
    # Use short labels (C1, P1, G1, etc.) on x-axis; fall back to full name
    labels = [BENCH_LABELS.get(m, m) for m in bench_keys]

    n = len(labels)
    n_opts = len(opt_keys)
    x = np.arange(n)
    bar_width = bar_group_width / n_opts

    for i, opt in enumerate(opt_keys):
        vals = [speedups[opt].get(m, 0) for m in bench_keys]
        offset = (i - n_opts / 2 + 0.5) * bar_width
        ax.bar(x + offset, vals, bar_width,
               label=opt_labels[i], color=OPT_COLORS[opt],
               edgecolor="white", linewidth=0.3)

    ax.set_xticks(x)
    ax.set_xticklabels(labels, fontsize=xtick_fontsize, fontweight="bold",
                       rotation=0, ha="center")
    ax.tick_params(axis='y', labelsize=ytick_fontsize)
    for label in ax.get_yticklabels():
        label.set_fontweight("bold")
    if show_ylabel:
        ax.set_ylabel("Speedups", fontsize=ylabel_fontsize, fontweight="bold")
    ax.axhline(y=1.0, color='red', linewidth=1.0, linestyle=':', zorder=0)
    # Use linear scale from 0 — micro-benchmarks have wide speedup ranges,
    # so the compress_below_one scale would make baseline bars invisible.
    ax.set_ylim(bottom=0, top=y_max)
    # Explicit y-axis ticks with integer steps
    step = 1 if y_max <= 5 else 2
    ticks = list(range(0, int(y_max) + step, step))
    if ticks[-1] < y_max:
        ticks.append(int(y_max) + step)
    ax.yaxis.set_major_locator(matplotlib.ticker.FixedLocator(ticks))
    ax.yaxis.set_major_formatter(matplotlib.ticker.FormatStrFormatter('%d'))
    ax.margins(x=0.02)

    if show_legend:
        ax.legend(loc="upper center", bbox_to_anchor=(0.5, 1.02),
                  fontsize=10, ncol=6,
                  frameon=False,
                  columnspacing=0.8, handletextpad=0.3, handlelength=1.0,
                  borderpad=0.3,
                  prop={"weight": "bold"})


def plot_separate(cat_groups, speedups, output_dir):
    """Generate one PDF per category."""
    # Compute unified figure height based on the largest category
    n_max = max(len(keys) for keys in cat_groups.values()) if cat_groups else 1
    fig_h = max(2.0, n_max * 0.25 + 1.0)

    for cat in sorted(cat_groups.keys(), key=lambda c: CATEGORY_ORDER.get(c, 99)):
        bench_keys = cat_groups[cat]
        title = CATEGORY_TITLES.get(cat, cat.capitalize())

        raw_max = _max_speedup_in(bench_keys, speedups)
        y_max = raw_max * 1.10

        fig, ax = plt.subplots(1, 1, figsize=(7, fig_h))
        _draw_grouped_bars(ax, bench_keys, speedups, y_max=y_max,
                           show_ylabel=True, show_legend=True)
        ax.set_title(title, fontsize=12, fontweight="bold", pad=8)
        fig.subplots_adjust(left=0.12, right=0.97, top=0.85, bottom=0.15)

        output_pdf = os.path.join(output_dir, f"micro_bench_{cat}.pdf")
        fig.savefig(output_pdf, format="pdf", bbox_inches="tight", pad_inches=0.01)
        plt.close(fig)
        print(f"Saved: {output_pdf}")


def plot_combined(cat_groups, speedups, output_dir, fullwidth=False,
                  description=None):
    """Generate a single PDF with a 2-row layout:

        Row 1: (a) Convolution   (spans full width)
        Row 2: (b) AvgPool | (c) GEMM  (side by side)

    This layout saves vertical space compared to 3 vertically stacked
    subplots, which is important for two-column papers.

    Args:
        fullwidth: unused (kept for CLI compatibility); the 2-row layout
                   always uses full two-column width (~7").
        description: optional figure caption rendered at the top.
    """
    import matplotlib.gridspec as gridspec

    cat_list = sorted(cat_groups.keys(), key=lambda c: CATEGORY_ORDER.get(c, 99))
    if not cat_list:
        return

    # Compute per-category y_max
    cat_ymax = {}
    for cat in cat_list:
        raw_max = _max_speedup_in(cat_groups[cat], speedups)
        cat_ymax[cat] = raw_max * 1.10

    # --- Layout parameters ---
    # Font sizes scaled down from generate_runtime_charts.py defaults
    # (xtick=12, ytick=12, ylabel=14, legend=10) to fit the compact layout
    col_width = 7.0          # full two-column width
    title_font = 9
    tick_font_x = 9
    tick_font_y = 9
    ylabel_font = 10
    legend_font = 8

    # Row 1 (Conv): height proportional to 8 benchmarks
    # Row 2 (AvgPool + GEMM): height proportional to max(3, 4) benchmarks
    n_conv = len(cat_groups.get("conv", []))
    n_row2 = max(
        len(cat_groups.get("avgpool", [])),
        len(cat_groups.get("gemm", [])),
        1,
    )
    row1_h = max(1.0, n_conv * 0.12 + 0.5)
    row2_h = max(0.8, n_row2 * 0.14 + 0.4)
    total_h = row1_h + row2_h + 0.55 + 0.20  # extra for legend + hspace gap

    if description:
        total_h += 0.35  # room for caption

    fig = plt.figure(figsize=(col_width, total_h))
    grid_top = 0.82 if description else 0.95
    gs = gridspec.GridSpec(2, 2, figure=fig, hspace=0.60, wspace=0.25,
                           left=0.08, right=0.98, top=grid_top, bottom=0.25)

    if description:
        fig.suptitle(description, x=0.02, y=0.98, ha="left",
                     fontsize=10, fontweight="normal")

    # (a) Convolution — spans both columns
    ax_conv = fig.add_subplot(gs[0, :])
    # (b) AvgPool — left column
    ax_avgpool = fig.add_subplot(gs[1, 0])
    # (c) GEMM — right column
    ax_gemm = fig.add_subplot(gs[1, 1])

    subplot_map = {
        "conv": (ax_conv, "(a)"),
        "avgpool": (ax_avgpool, "(b)"),
        "gemm": (ax_gemm, "(c)"),
    }

    for cat in cat_list:
        ax, label = subplot_map[cat]
        bench_keys = cat_groups[cat]
        y_max = cat_ymax[cat]
        title = f"{label} {CATEGORY_TITLES.get(cat, cat.capitalize())}"

        is_right = (cat == "gemm")
        _draw_grouped_bars(ax, bench_keys, speedups, y_max=y_max,
                           show_ylabel=(not is_right), show_legend=False,
                           xtick_fontsize=tick_font_x,
                           ytick_fontsize=tick_font_y,
                           ylabel_fontsize=ylabel_font,
                           bar_group_width=0.8)
        ax.set_title(title, fontsize=title_font, fontweight="bold", pad=3)

    # Shared legend at the bottom — placed inside the bottom margin
    handles, legend_labels = ax_conv.get_legend_handles_labels()
    fig.legend(handles, legend_labels, loc="lower center",
               bbox_to_anchor=(0.5, 0.08), fontsize=legend_font,
               ncol=6, frameon=False,
               columnspacing=0.5, handletextpad=0.3,
               handlelength=1.0, handleheight=0.7, borderpad=0.2,
               labelspacing=0.2, prop={"weight": "bold"})

    output_pdf = os.path.join(output_dir, "micro_bench_combined.pdf")
    pad = 0.12 if description else 0.02
    fig.savefig(output_pdf, format="pdf", bbox_inches="tight", pad_inches=pad)
    plt.close(fig)
    # print(f"Saved: {output_pdf}")


def main():
    parser = argparse.ArgumentParser(
        description="Generate micro-benchmark bar charts by category")
    parser.add_argument("-d", "--data-dir",
                        help="Path to data directory relative to script dir "
                             "(e.g. machine181/509_micro_bench_data)")
    parser.add_argument("--combined", action="store_true",
                        help="Generate a single combined PDF with a 2-row "
                             "layout: (a) Conv on top, (b) AvgPool + "
                             "(c) GEMM side by side on bottom. Uses full "
                             "two-column width (~7 inches).")
    parser.add_argument("--fullwidth", action="store_true",
                        help="Alias for --combined (kept for compatibility)")
    args = parser.parse_args()

    base_dir = os.path.dirname(os.path.abspath(__file__))
    data_dir = args.data_dir or None
    models, _, runtimes = load_opt_data(base_dir, data_dir)
    speedups = compute_speedup(runtimes)

    # Only keep benchmarks that have baseline runtime data
    bench_models = [m for m in models
                    if runtimes.get("conv_fast", {}).get(m) is not None]
    if not bench_models:
        print("No benchmark data found.")
        return

    # Load category mapping
    categories = load_micro_bench_categories(base_dir, data_dir)

    # Group benchmarks by category
    cat_groups = {}
    for m in bench_models:
        cat = categories.get(m, "other")
        cat_groups.setdefault(cat, []).append(m)

    # Sort benchmarks within each category
    for cat in cat_groups:
        cat_groups[cat].sort()

    output_dir = os.path.join(base_dir, data_dir) if data_dir else base_dir

    if args.combined or args.fullwidth:
        plot_combined(cat_groups, speedups, output_dir)
    else:
        plot_separate(cat_groups, speedups, output_dir)


if __name__ == "__main__":
    main()
