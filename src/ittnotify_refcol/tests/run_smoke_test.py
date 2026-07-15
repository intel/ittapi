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


EXPECTED_ITT_APIS = [
    "__itt_task_begin",
    "__itt_task_end",
    "__itt_metadata_add",
]


def main():
    parser = argparse.ArgumentParser(description="Run reference collector smoke test")
    parser.add_argument("--lib", required=True, help="Path to libittnotify_refcol shared library")
    parser.add_argument("--exe", required=True, help="Path to refcol_smoke_test executable")
    parser.add_argument("--log-dir", help="Directory for log files (default: temp dir)")
    args = parser.parse_args()

    lib = os.path.abspath(args.lib)
    exe = os.path.abspath(args.exe)

    if not os.path.isfile(lib):
        print(f"ERROR: library not found: {lib}")
        return 1
    if not os.path.isfile(exe):
        print(f"ERROR: executable not found: {exe}")
        return 1

    log_dir = os.path.abspath(args.log_dir) if args.log_dir else tempfile.mkdtemp(prefix="refcol_logs_")
    os.makedirs(log_dir, exist_ok=True)

    env = os.environ.copy()
    env["INTEL_LIBITTNOTIFY64"] = lib
    env["INTEL_LIBITTNOTIFY_LOG_DIR"] = log_dir

    print(f"Library:  {lib}")
    print(f"Exe:      {exe}")
    print(f"Log dir:  {log_dir}")

    result = subprocess.run([exe], env=env)
    if result.returncode != 0:
        print(f"ERROR: smoke test executable exited with code {result.returncode}")
        return 1

    logs = glob.glob(os.path.join(log_dir, "libittnotify_refcol_*.json"))
    if not logs:
        print("ERROR: no trace file found in", log_dir)
        return 1

    trace_path = logs[0]
    print(f"Trace file: {trace_path}")

    with open(trace_path, encoding="utf-8", errors="replace") as f:
        content = f.read()

    try:
        events = json.loads(content)
    except json.JSONDecodeError as exc:
        print(f"ERROR: trace file is not valid JSON: {exc}")
        return 1

    if not isinstance(events, list) or not events:
        print("ERROR: trace file does not contain a non-empty JSON array of events")
        return 1

    seen_apis = {
        e.get("args", {}).get("itt_api")
        for e in events
        if isinstance(e, dict) and isinstance(e.get("args"), dict)
    }
    missing = [api for api in EXPECTED_ITT_APIS if api not in seen_apis]
    if missing:
        for api in missing:
            print(f"ERROR: '{api}' event not found in trace")
        return 1

    print("Smoke test passed.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
