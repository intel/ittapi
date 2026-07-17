.. _ref_collector:

ITT API Reference Collector
===========================


This is a reference implementation of the ITT API **dynamic** part that
performs tracing data from ITT API function calls to log files.

To use this solution, build the collector as a shared library and point the
full library path to the `INTEL_LIBITTNOTIFY64` environment variable.


Building
--------

There are two ways to build the ITT API reference collector shared library.

**Standalone** — run this single CMake build command from the
``src/ittnotify_refcol`` directory (the library is written into the ``./build``
directory):

.. code-block:: console

    cmake -B build && cmake --build build --config Release

**From the ittapi repository** — use the ``buildall.py`` script in the
repository root (the library is written into the ``build_linux/bin`` or
``build_win/bin`` directory):

.. code-block:: console

    python buildall.py --refcol

Resulting library name:

.. list-table::
   :header-rows: 1

   * - Platform
     - Library name
   * - Linux
     - ``libittnotify_refcol.so``
   * - Windows
     - ``libittnotify_refcol.dll``


Usage
-----

Point the ``INTEL_LIBITTNOTIFY64`` environment variable to the full path of the
library you built above (adjust the path to your build directory).

**On Linux**


.. code-block:: bash

    export INTEL_LIBITTNOTIFY64=<build_dir>/libittnotify_refcol.so


**On Windows**


.. code-block:: bat

    set INTEL_LIBITTNOTIFY64=<build_dir>\libittnotify_refcol.dll


Log File Location
-----------------

By default, log files are saved in the current working directory. Each run
creates a file named ``libittnotify_refcol_<timestamp>.log``. To change the
location, use the ``INTEL_LIBITTNOTIFY_LOG_DIR`` environment variable:


**On Linux**


.. code-block:: bash

    export INTEL_LIBITTNOTIFY_LOG_DIR=<log_dir>


**On Windows**


.. code-block:: bat

    set INTEL_LIBITTNOTIFY_LOG_DIR=<log_dir>


Extending
---------

This implementation adds logging of some of the ITT API function calls. Adding
logging of other ITT API function calls is welcome. The solution provides 4
functions with different log levels that take `printf` format for logging:

.. code-block:: c

    LOG_FUNC_CALL_INFO(const char *msg_format, ...);
    LOG_FUNC_CALL_WARN(const char *msg_format, ...);
    LOG_FUNC_CALL_ERROR(const char *msg_format, ...);
    LOG_FUNC_CALL_FATAL(const char *msg_format, ...);


Experimental: JSON trace generation
------------------------------------

In addition to the default text log, the collector can produce a trace in
JSON format (Perfetto). This mode is experimental and is disabled by default.

To enable it, set the ``EXP_LIBITTNOTIFY_GEN_JSON`` environment variable to ``1``:

**On Linux**


.. code-block:: bash

    export EXP_LIBITTNOTIFY_GEN_JSON=1


**On Windows**


.. code-block:: bat

    set EXP_LIBITTNOTIFY_GEN_JSON=1


When enabled, the collector writes a ``libittnotify_refcol_<timestamp>.json``
file (instead of the text log) into the log directory. The file can be opened
directly in https://ui.perfetto.dev.

