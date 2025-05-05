.. _ref_collector:

ITT API Reference Collector
===========================


This is a reference implementation of the ITT API **dynamic** part that
performs tracing data from ITT API function calls to log files.


To use this solution, it is required to build it as a shared library and the
full library path to the ``INTEL_LIBITTNOTIFY64`` or ``INTEL_LIBITTNOTIFY32``
environment variable:


**On Linux**


.. code-block:: console

    make
    export INTEL_LIBITTNOTIFY64=<build_dir>/libittnotify_refcol.so


**On FreeBSD**


.. code-block:: console

    make
    setenv INTEL_LIBITTNOTIFY64 <build_dir>/libittnotify_refcol.so


Temp directory is used by default to save log files. To change the log directory,
use the ``INTEL_LIBITTNOTIFY_LOG_DIR`` environment variable:


**On Linux**


.. code-block:: console


    export INTEL_LIBITTNOTIFY_LOG_DIR=<log_dir>


**On FreeBSD**


.. code-block:: console


    setenv INTEL_LIBITTNOTIFY_LOG_DIR <log_dir>


This implementation adds logging of some of the ITT API function calls. Adding
logging of other ITT API function calls is welcome. The solution provides 4
functions with different log levels that take ``printf`` format for logging:


.. code-block:: c

    LOG_FUNC_CALL_INFO(const char *msg_format, ...);
    LOG_FUNC_CALL_WARN(const char *msg_format, ...);
    LOG_FUNC_CALL_ERROR(const char *msg_format, ...);
    LOG_FUNC_CALL_FATAL(const char *msg_format, ...);

