# Nightly Performance Guard

Runs `benchmark/resnet/` (the pytest-regressions baseline suite) on a GPU box
every night, archives the verdict, and fails the run when latency / memory /
accuracy regress beyond the committed tolerances.

The comparison logic is **not** in this directory — it lives in pytest via
`pytest-regressions`:

| Test file | Mechanism | Baselines | Tolerance |
|-----------|-----------|-----------|-----------|
| `benchmark/resnet/test_latency.py` | `num_regression` | `test_latency/*.csv` (a100/h20) | `rtol=0.2, atol=30ms` |
| `benchmark/resnet/test_memory.py` | `num_regression` | `test_memory/*.csv` | `rtol=0.1, atol=50` |
| `benchmark/resnet/test_accuracy.py` | `data_regression` | `test_accuracy/*.yml` | exact match |

This guard only **drives pytest read-only** and aggregates the result. The
repo's CI never runs this suite (internal ACI is CPU-only `-k "not cuda"`; GHA
builds wheels/images), so the nightly guard is the only thing exercising GPU
baselines.

```
systemd timer → nightly-bench.service (host, root)
                  └─ /opt/nightly-bench/run.sh
                      ├─ host preflight: GPU-idle check + drop_caches
                      ├─ git fetch/reset master
                      └─ docker run --gpus all ace/dev-cuda
                             └─ repo:scripts/perf-guard/orchestrator.sh
                                  ├─ build (CI_TOKEN dev-build.sh)
                                  ├─ LD_LIBRARY_PATH → ace/lib
                                  ├─ pytest benchmark/resnet/ --junitxml …
                                  ├─ summarize.py → history.jsonl
                                  └─ exit code = pass/fail
```

## ⚠️ Golden rules

1. **Never use `--regen-all`.** It overwrites the committed baselines and
   defeats the guard. Baselines are updated deliberately, by a human, on a
   known-good change — not by the nightly run.
2. **`drop_caches` must run on the host**, before the container starts. The
   container's `/proc/sys` is read-only.
3. **The GPU must be idle.** Competing CUDA processes fight for memory and can
   segfault the FHE runtime; the host preflight aborts if anything is running.
4. **Known risk (v1, not mitigated):** `TARGET_PARAMS` orders
   `ant-cpu → phantom-cuda → ace-cuda` and `benchmark/resnet/conftest.py` does
   **not** reorder CPU before CUDA (unlike `tests/regression/conftest.py`). If
   the CUDA-then-ant segment segfaults, split into two pytest processes
   (`-k "ant"` then `-k "not ant"`, merge the junit files).
5. **`test_memory` CSVs are not GPU-tagged** — a100/h20 share one baseline.
   A cross-GPU regression may surface as a memory failure.

## Repo-side files

- `orchestrator.sh` — in-container driver. Env knobs: `PERF_SKIP_BUILD=1`,
  `PERF_DRY=1`, `PERF_FAST=1`.
- `summarize.py` — parse `junit.xml` → one JSONL line in `history.jsonl`.

## Host deployment (one-time)

### 1. Clone + image

```bash
git clone <repo> /opt/ace-compiler
docker pull reg.docker.alibaba-inc.com/ace/dev-cuda:12.8.0-devel-ubuntu24.04
```

### 2. Host wrapper `/opt/nightly-bench/run.sh`

```bash
#!/bin/bash
set -euo pipefail

REPO=/opt/ace-compiler
IMAGE=reg.docker.alibaba-inc.com/ace/dev-cuda:12.8.0-devel-ubuntu24.04
HIST=/mnt/nvme0/perf_history/resnet

# --- host preflight: GPU must be idle, page cache dropped (container can't) ---
if nvidia-smi --query-compute-apps=pid --format=csv,noheader | grep -q .; then
  echo "GPU not idle; aborting" >&2; exit 2
fi
echo 3 > /proc/sys/vm/drop_caches   # requires root; cold-cache discipline

# --- sync code ---
cd "$REPO"
git fetch --tags
git checkout master
git reset --hard origin/master

RUN_ID="$(date -u +%Y%m%d_%H%M)"

# --- launch container ---
docker run --rm --gpus all --shm-size=64g \
  -v "$REPO":/work/ace-fhe/ace-compiler \
  -v /work/dataset:/work/dataset \
  -v /mnt/nvme0:/mnt/nvme0 \
  -e CI_TOKEN="$CI_TOKEN" \
  -e CUDA_VISIBLE_DEVICES=0 \
  -e ACE_DATA_DIR=/work/dataset \
  -e PERF_RUN_ID="$RUN_ID" \
  "$IMAGE" bash /work/ace-fhe/ace-compiler/scripts/perf-guard/orchestrator.sh
```

Notable flags: `--gpus all` (These tests do not use cuFile direct NVMe read, so
no `--device /dev/nvme*` is needed — that's the IO-bench path, not this one);
`--shm-size=64g` (FHE DataLoader/CUDA IPC, default 64M will hang); `-v
/work/dataset` (CIFAR; otherwise the container auto-downloads to `project/data`
on first run, requiring network).

### 3. CI token file `/etc/nightly-bench.env` (chmod 600)

```
CI_TOKEN=<your token>
```

### 4. systemd units

`/etc/systemd/system/nightly-bench.service`
```ini
[Unit]
Description=ACE FHE nightly ResNet perf guard
Requires=docker.service
After=docker.service network-online.target
OnFailure=nightly-bench-fail.service

[Service]
Type=oneshot
EnvironmentFile=/etc/nightly-bench.env
ExecStart=/opt/nightly-bench/run.sh
TimeoutStartSec=3h
Nice=10
IOSchedulingClass=idle
StandardOutput=journal
StandardError=journal
```

`/etc/systemd/system/nightly-bench.timer`
```ini
[Unit]
Description=Trigger ACE nightly perf guard

[Timer]
OnCalendar=*-*-* 03:17:00
Persistent=true
RandomizedDelaySec=300

[Install]
WantedBy=timers.target
```

`/etc/systemd/system/nightly-bench-fail.service`
```ini
[Unit]
Description=ACE nightly perf guard failure capture

[Service]
Type=oneshot
ExecStart=/bin/bash -c 'journalctl -u nightly-bench -n 500 > /mnt/nvme0/perf_history/resnet/last_fail.log'
```

### 5. Enable + first manual run

```bash
systemctl daemon-reload
systemctl enable --now nightly-bench.timer
systemctl list-timers | grep nightly-bench

# verify end-to-end now:
systemctl start nightly-bench.service
journalctl -u nightly-bench -f
```

## Verification of a run

```bash
RUN_ID=<latest>
ls /mnt/nvme0/perf_history/resnet/runs/$RUN_ID/   # junit.xml, pytest.log
tail -1 /mnt/nvme0/perf_history/resnet/history.jsonl | python3 -m json.tool
```

A `history.jsonl` line looks like:
```json
{"run_id":"20260708_0317","ts":"2026-07-08T03:17:00","git":"3794888","gpu_tag":"a100",
 "tests":184,"failures":5,"errors":0,"skipped":60,"passed":119,
 "failed":[{"name":"test_latency[resnet20-phantom-cuda]","file":"benchmark/resnet/test_latency.py","type":"failure","msg":"…"}],
 "pass":false}
```

On the **first** run, a full pass is expected — the baselines are already
committed. If everything fails, suspect the environment/data, not a regression.

## Updating baselines (deliberate, manual)

When a code change legitimately shifts baseline numbers:

```bash
# on the GPU box, after a known-good build:
pytest benchmark/resnet/ --regen-all      # rewrites the committed CSV/YML
git diff benchmark/resnet/               # review carefully
git commit -am "bench: update resnet baselines for <change>"
```

This is a human action on a real change — never automated by the nightly guard.
