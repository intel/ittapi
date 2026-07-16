#!/usr/bin/env python
#
# Copyright (C) 2026 Intel Corporation
# SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause
#

import argparse
import glob
import json
import os
import subprocess
import sys
import tempfile


# ITT API names expected in the plain-text log (mode 1).
EXPECTED_SYMBOLS = [
    "__itt_task_begin",
    "__itt_task_end",
    "__itt_metadata_add",
    "__itt_frame_begin_v3",
    "__itt_frame_end_v3",
    "__itt_region_begin",
    "__itt_region_end",
]

# Friendly "api" values expected in the JSON trace args (mode 2).
EXPECTED_JSON_APIS = [
    "task",
    "metadata",
    "frame",
    "region",
]


def run_exe(exe, lib, log_dir, gen_json):
    """Run the smoke test executable with the collector attached.

    gen_json selects the collector output mode: False -> plain-text log,
    True -> JSON trace (EXP_LIBITTNOTIFY_GEN_JSON=1).
    """
    env = os.environ.copy()
    env["INTEL_LIBITTNOTIFY64"] = lib
    env["INTEL_LIBITTNOTIFY_LOG_DIR"] = log_dir
    if gen_json:
        env["EXP_LIBITTNOTIFY_GEN_JSON"] = "1"

    result = subprocess.run([exe], env=env)
    if result.returncode != 0:
        print(f"ERROR: smoke test executable exited with code {result.returncode}")
        return False
    return True


def check_text_log(log_dir):
    """Scenario 1: plain-text log contains the expected ITT API call lines."""
    logs = glob.glob(os.path.join(log_dir, "libittnotify_refcol_*.log"))
    if not logs:
        print("ERROR: no .log file found in", log_dir)
        return False

    log_path = logs[0]
    print(f"Log file: {log_path}")

    with open(log_path, encoding="utf-8", errors="replace") as f:
        content = f.read()

    missing = [sym for sym in EXPECTED_SYMBOLS if sym not in content]
    if missing:
        for sym in missing:
            print(f"ERROR: '{sym}' not found in log")
        return False

    print("Text log scenario passed.")
    return True


def check_json_trace(log_dir):
    """Scenario 2: JSON trace is a valid Chrome Trace Event array with the
    expected task events and friendly api arg values."""
    traces = glob.glob(os.path.join(log_dir, "libittnotify_refcol_*.json"))
    if not traces:
        print("ERROR: no .json file found in", log_dir)
        return False

    trace_path = traces[0]
    print(f"Trace file: {trace_path}")

    try:
        with open(trace_path, encoding="utf-8", errors="replace") as f:
            events = json.load(f)
    except json.JSONDecodeError as exc:
        print(f"ERROR: JSON trace is not valid JSON: {exc}")
        return False

    if not isinstance(events, list) or not events:
        print("ERROR: JSON trace is empty or not an array")
        return False

    phases = {e.get("ph") for e in events}
    if "B" not in phases or "E" not in phases:
        print(f"ERROR: expected task begin/end (B/E) phases, got: {sorted(phases)}")
        return False

    seen_apis = {e.get("args", {}).get("api") for e in events}
    missing = [api for api in EXPECTED_JSON_APIS if api not in seen_apis]
    if missing:
        for api in missing:
            print(f"ERROR: api '{api}' not found in JSON trace args")
        return False

    print("JSON trace scenario passed.")
    return True


def main():
    parser = argparse.ArgumentParser(description="Run reference collector smoke test")
    parser.add_argument("--lib", required=True, help="Path to libittnotify_refcol shared library")
    parser.add_argument("--exe", required=True, help="Path to refcol_smoke_test executable")
    parser.add_argument("--log-dir", help="Base directory for log files (default: temp dir)")
    args = parser.parse_args()

    lib = os.path.abspath(args.lib)
    exe = os.path.abspath(args.exe)

    if not os.path.isfile(lib):
        print(f"ERROR: library not found: {lib}")
        return 1
    if not os.path.isfile(exe):
        print(f"ERROR: executable not found: {exe}")
        return 1

    base_dir = os.path.abspath(args.log_dir) if args.log_dir else tempfile.mkdtemp(prefix="refcol_logs_")

    print(f"Library:  {lib}")
    print(f"Exe:      {exe}")
    print(f"Log dir:  {base_dir}")

    # Each scenario writes to its own subdirectory so the .log and .json
    # outputs do not mix.
    text_dir = os.path.join(base_dir, "text")
    json_dir = os.path.join(base_dir, "json")
    os.makedirs(text_dir, exist_ok=True)
    os.makedirs(json_dir, exist_ok=True)

    print("\n=== Scenario 1: plain-text log (default) ===")
    if not run_exe(exe, lib, text_dir, gen_json=False):
        return 1
    if not check_text_log(text_dir):
        return 1

    print("\n=== Scenario 2: JSON trace (EXP_LIBITTNOTIFY_GEN_JSON=1) ===")
    if not run_exe(exe, lib, json_dir, gen_json=True):
        return 1
    if not check_json_trace(json_dir):
        return 1

    print("\nSmoke test passed.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
