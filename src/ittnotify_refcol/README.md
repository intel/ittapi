# Instrumentation and Tracing Technology (ITT) API Reference Collector

This is a reference implementation of the ITT API *dynamic* part that
translates ITT API function calls into a
[Perfetto / Chrome Trace Event](https://ui.perfetto.dev) trace written in JSON.
The resulting file can be opened directly in <https://ui.perfetto.dev> or
`chrome://tracing`.

To use this solution, build the collector as a shared library and point the
full library path to the `INTEL_LIBITTNOTIFY64` environment variable.

## Building

There are two ways to build the ITT API reference collector shared library.

**Standalone** — run this single CMake build command from this directory
(the library is written into the `./build` directory):

```bash
cmake -B build && cmake --build build --config Release
```

**From the ittapi repository** — use the `buildall.py` script in the repository root
(the library is written into the `build_linux/bin` or `build_win/bin` directory):

```bash
python buildall.py --refcol
```

Resulting library name:

| Platform | Library name                   |
|----------|--------------------------------|
| Linux    | `libittnotify_refcol.so`       |
| Windows  | `libittnotify_refcol.dll`      |

## Usage

Point the `INTEL_LIBITTNOTIFY64` environment variable to the full path of the
library you built above (adjust the path to your build directory).

**On Linux**

```
export INTEL_LIBITTNOTIFY64=<build_dir>/libittnotify_refcol.so
```

**On FreeBSD**

```
setenv INTEL_LIBITTNOTIFY64 <build_dir>/libittnotify_refcol.so
```

**On Windows**

```
set INTEL_LIBITTNOTIFY64=<build_dir>\libittnotify_refcol.dll
```

By default, trace files are saved in the system temporary directory. To change
the location, use the `INTEL_LIBITTNOTIFY_LOG_DIR` environment variable:

**On Linux**

```
export INTEL_LIBITTNOTIFY_LOG_DIR=<log_dir>
```

**On FreeBSD**
```
setenv INTEL_LIBITTNOTIFY_LOG_DIR <log_dir>
```

**On Windows**
```
set INTEL_LIBITTNOTIFY_LOG_DIR=<log_dir>
```

The collector writes one trace file per run named
`libittnotify_refcol_<timestamp>.json`. It is a Chrome Trace Event trace in the
streaming-friendly "JSON Array Format", so it loads directly into
<https://ui.perfetto.dev> or `chrome://tracing`. ITT API calls are mapped to
trace events as follows:

| ITT API                     | Trace event phase                          |
|-----------------------------|--------------------------------------------|
| `__itt_task_begin` / `_end` | `B` / `E` (synchronous, per-thread)        |
| `__itt_region_begin`/`_end` | `b` / `e` (asynchronous, matched by id)    |
| `__itt_frame_begin`/`_end`  | `b` / `e` (asynchronous, matched by id)    |
| `__itt_frame_submit_v3`     | `X` (complete event with explicit duration)|
| `__itt_counter_set_value`   | `C` (counter series)                       |
| metadata / pause / resume   | `i` (thread-scoped instant marker)         |

Each event carries an `itt_api` argument identifying the originating ITT call.
Adding support for other ITT API calls is welcome: emit events with the
`trace_emit()` helper, which takes a phase, category, name, timestamp, and an
optional raw-JSON `extra` field (for `args`, `id`, `dur`, etc.).
