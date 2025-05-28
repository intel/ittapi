# Instrumentation and Tracing Technology (ITT) API Reference Collector

This is a reference implementation of the ITT API *dynamic* part that
performs tracing data from ITT API function calls to log files.

To use this solution, build the collector as a shared library and point the
full library path to the `INTEL_LIBITTNOTIFY64` or `INTEL_LIBITTNOTIFY32`
environment variable:

**On Linux**

```
make
export INTEL_LIBITTNOTIFY64=<build_dir>/libittnotify_refcol.so
```

**On FreeBSD**

```
make
setenv INTEL_LIBITTNOTIFY64 <build_dir>/libittnotify_refcol.so
```

By default, log files save in the `tmp` directory. To change the location,
use the `INTEL_LIBITTNOTIFY_LOG_DIR` environment variable:

**On Linux**

```
export INTEL_LIBITTNOTIFY_LOG_DIR=<log_dir>
```

**On FreeBSD**
```
setenv INTEL_LIBITTNOTIFY_LOG_DIR <log_dir>
```

This implementation adds logging of some of the ITT API function calls. Adding
logging of other ITT API function calls is welcome. The solution provides 4
functions with different log levels that take `printf` format for logging:

```
LOG_FUNC_CALL_INFO(const char *msg_format, ...);
LOG_FUNC_CALL_WARN(const char *msg_format, ...);
LOG_FUNC_CALL_ERROR(const char *msg_format, ...);
LOG_FUNC_CALL_FATAL(const char *msg_format, ...);
```
