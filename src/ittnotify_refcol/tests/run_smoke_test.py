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
    "__itt_domain_create",
    "__itt_string_handle_create",
    "__itt_task_begin",
    "__itt_task_end",
    "__itt_metadata_add",
    "__itt_formatted_metadata_add",
    "__itt_frame_begin_v3",
    "__itt_frame_end_v3",
    "__itt_frame_submit_v3",
    "__itt_region_begin",
    "__itt_region_end",
    "__itt_counter_set_value_v3",
    "__itt_task_begin_overlapped",
    "__itt_task_end_overlapped",
    "__itt_histogram_submit",
    "__itt_thread_set_name",
    "__itt_pause",
    "__itt_resume",
]

# Friendly "api" values expected in the JSON trace args (mode 2).
# Note: __itt_counter_set_value_v3 emits a "C" event whose args are the series
# value directly (no "api" tag); counters are validated by check_counter.
# Note: regions are traced identically to tasks (api "task"), so there is no
# separate "region" api value.
EXPECTED_JSON_APIS = [
    "task",
    "metadata",
    "frame",
    "histogram",
    "flow",
    "pause",
    "resume",
]

# Chrome Trace Event phases expected in the JSON trace (mode 2).
#   B/E  synchronous task and region slices
#   b/e  asynchronous overlapped-task and frame slices
#   X    frame submit (complete event)
#   C    counter series
#   s/f  flow start / finish
#   i    instant markers (metadata, histogram, pause, resume)
#   M    thread name metadata
EXPECTED_JSON_PHASES = ["B", "E", "b", "e", "X", "C", "s", "f", "i", "M"]


def run_exe(exe, lib, log_dir, gen_json):
    """Run the smoke test executable with the collector attached.

    gen_json selects the collector output mode: False -> plain-text log,
    True -> JSON trace (INTEL_LIBITTNOTIFY_GEN_JSON=1).
    """
    env = os.environ.copy()
    env["INTEL_LIBITTNOTIFY64"] = lib
    env["INTEL_LIBITTNOTIFY_LOG_DIR"] = log_dir
    if gen_json:
        env["INTEL_LIBITTNOTIFY_GEN_JSON"] = "1"

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


def _api_of(event):
    """Return the friendly api tag from an event's args, or None."""
    args = event.get("args")
    if not isinstance(args, dict):
        return None
    return args.get("api")


def _check(condition, message):
    """Print an error and return False when condition is falsy."""
    if not condition:
        print(f"ERROR: {message}")
    return bool(condition)


def check_json_phases(events):
    """Every expected Chrome Trace phase is present."""
    phases = {e.get("ph") for e in events}
    missing = [ph for ph in EXPECTED_JSON_PHASES if ph not in phases]
    return _check(not missing, f"missing JSON phases: {missing} (got {sorted(phases)})")


def check_json_apis(events):
    """Every expected friendly api value is present in the args."""
    seen = {_api_of(e) for e in events}
    missing = [api for api in EXPECTED_JSON_APIS if api not in seen]
    return _check(not missing, f"missing api values: {missing}")


def check_slice_balance(events):
    """Synchronous (B/E) and asynchronous (b/e) slices are balanced and each
    end matches a begin. The smoke test is single-threaded, so a single stack
    per phase family is sufficient."""
    ok = True

    sync_open = 0
    sync_min = 0
    for e in events:
        if e.get("ph") == "B":
            sync_open += 1
        elif e.get("ph") == "E":
            sync_open -= 1
            sync_min = min(sync_min, sync_open)
    ok &= _check(sync_open == 0, f"unbalanced sync slices (net {sync_open})")
    ok &= _check(sync_min >= 0, "a sync 'E' appeared with no open 'B'")

    async_begin = {e.get("id") for e in events if e.get("ph") == "b"}
    async_end = {e.get("id") for e in events if e.get("ph") == "e"}
    n_begin = sum(1 for e in events if e.get("ph") == "b")
    n_end = sum(1 for e in events if e.get("ph") == "e")
    ok &= _check(n_begin == n_end, f"async b/e count mismatch ({n_begin} vs {n_end})")
    orphan_ends = async_end - async_begin
    ok &= _check(not orphan_ends, f"async 'e' ids with no matching 'b': {orphan_ends}")
    return ok


def check_overlap(events):
    """At least one pair of overlapped async tasks actually overlaps, i.e. a
    second 'b' opens before the first 'e' closes (non-LIFO ordering)."""
    depth = 0
    max_depth = 0
    for e in events:
        if e.get("ph") == "b":
            depth += 1
            max_depth = max(max_depth, depth)
        elif e.get("ph") == "e":
            depth -= 1
    return _check(max_depth >= 2, "no overlapping async tasks observed (max concurrent < 2)")


def check_flows(events):
    """Flow events use numeric ids, and every finish ('f') matches a start
    ('s'). Flows are only emitted for synchronous slices, so each flow event
    must sit inside an open B/E slice (otherwise Perfetto drops it as
    flow_no_enclosing_slice)."""
    ok = True
    starts = {e.get("id") for e in events if e.get("ph") == "s"}
    finishes = [e for e in events if e.get("ph") == "f"]

    ok &= _check(len(starts) > 0, "no flow start ('s') events found")
    numeric = all(
        isinstance(e.get("id"), int)
        for e in events
        if e.get("ph") in ("s", "f")
    )
    ok &= _check(numeric, "flow ids must be numeric for Perfetto to link them")

    matched = sum(1 for e in finishes if e.get("id") in starts)
    ok &= _check(
        matched == len(finishes),
        f"flow finishes without a matching start: {len(finishes) - matched}",
    )

    sync_open = 0
    orphan_flows = 0
    for e in events:
        ph = e.get("ph")
        if ph == "B":
            sync_open += 1
        elif ph == "E":
            sync_open = max(0, sync_open - 1)
        elif ph in ("s", "f") and sync_open == 0:
            orphan_flows += 1
    ok &= _check(
        orphan_flows == 0,
        f"{orphan_flows} flow events have no enclosing sync slice",
    )
    return ok


def check_metadata_pinned(events):
    """__itt_formatted_metadata_add is folded into the enclosing task_end, so
    at least one 'E' event must carry a 'metadata' arg."""
    pinned = any(
        e.get("ph") == "E" and isinstance(e.get("args"), dict) and "metadata" in e["args"]
        for e in events
    )
    return _check(pinned, "no task_end ('E') carries pinned formatted metadata")


def check_counter(events):
    """Counter events ('C') carry a numeric series value."""
    counters = [e for e in events if e.get("ph") == "C"]
    if not _check(counters, "no counter ('C') events found"):
        return False
    numeric = all(
        isinstance(v, (int, float))
        for e in counters
        for v in (e.get("args") or {}).values()
    )
    return _check(numeric, "counter values must be numeric")


def check_frame_submit(events):
    """Frame submit maps to a complete event ('X') with a duration."""
    submits = [e for e in events if e.get("ph") == "X"]
    if not _check(submits, "no frame submit ('X') events found"):
        return False
    have_dur = all("dur" in e for e in submits)
    return _check(have_dur, "frame submit events must carry a 'dur' field")


def check_thread_name(events):
    """Thread naming maps to a metadata event ('M') with a name arg."""
    named = any(
        e.get("ph") == "M" and (e.get("args") or {}).get("name")
        for e in events
    )
    return _check(named, "no thread-name ('M') event with a name arg")


def check_pause_suppression(events):
    """The task issued between pause and resume must not appear in the trace:
    no event should reference the 'smoke_test_paused' handle, and pause must
    precede resume."""
    ok = True
    ok &= _check(
        not any("smoke_test_paused" in json.dumps(e) for e in events),
        "an event issued while paused leaked into the trace",
    )
    apis = [_api_of(e) for e in events]
    if "pause" in apis and "resume" in apis:
        ok &= _check(
            apis.index("pause") < apis.index("resume"),
            "pause marker must precede resume marker",
        )
    return ok


def check_json_trace(log_dir):
    """Scenario 2: the JSON trace is a valid Chrome Trace Event array and every
    instrumented ITT construct is represented with the expected shape."""
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

    checks = [
        ("phases", check_json_phases),
        ("api values", check_json_apis),
        ("slice balance", check_slice_balance),
        ("async overlap", check_overlap),
        ("flows", check_flows),
        ("pinned metadata", check_metadata_pinned),
        ("counter", check_counter),
        ("frame submit", check_frame_submit),
        ("thread name", check_thread_name),
        ("pause suppression", check_pause_suppression),
    ]

    all_ok = True
    for label, fn in checks:
        result = fn(events)
        print(f"  [{'PASS' if result else 'FAIL'}] {label}")
        all_ok &= result

    if not all_ok:
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

    print("\n=== Scenario 2: JSON trace (INTEL_LIBITTNOTIFY_GEN_JSON=1) ===")
    if not run_exe(exe, lib, json_dir, gen_json=True):
        return 1
    if not check_json_trace(json_dir):
        return 1

    print("\nSmoke test passed.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
