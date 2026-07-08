# Instrumentation and Tracing Technology (ITT) API Reference Collector

This is a reference implementation of the ITT API *dynamic* part that
performs tracing data from ITT API function calls to log files.

To use this solution, build the collector as a shared library and point the
full library path to the `INTEL_LIBITTNOTIFY64` environment variable.

## Building

There are two ways to build the ITT API reference collector shared library:

**Standalone** — run this single CMake build command:

```
cmake -B build && cmake --build build --config Release
```

**From the ittapi repository** — use the `buildall.py` script in the repository
root:

```
python buildall.py --refcol
```

Resulting library name:

| Platform | Library name                   |
|----------|--------------------------------|
| Linux    | `libittnotify_refcol.so`       |
| Windows  | `libittnotify_refcol.dll`      |

## Usage

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

By default, log files are saved in the system temporary directory. To change
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

This implementation adds logging of some of the ITT API function calls. Adding
logging of other ITT API function calls is welcome. The solution provides 4
functions with different log levels that take `printf` format for logging:

```c
LOG_FUNC_CALL_INFO(const char *msg_format, ...);
LOG_FUNC_CALL_WARN(const char *msg_format, ...);
LOG_FUNC_CALL_ERROR(const char *msg_format, ...);
LOG_FUNC_CALL_FATAL(const char *msg_format, ...);
```
