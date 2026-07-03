.. _ref_collector:

ITT API Reference Collector
===========================


This is a reference implementation of the ITT API **dynamic** part that
performs tracing data from ITT API function calls to log files.

To use this solution, build the collector as a shared library and point the
full library path to the `INTEL_LIBITTNOTIFY64` environment variable.


Building
--------

Use CMake from the repository root, enabling the ``ITT_API_REFERENCE_COLLECTOR`` option:

.. code-block:: console

    cmake -B <build_dir> -DITT_API_REFERENCE_COLLECTOR=ON
    cmake --build <build_dir>

Alternatively, use the provided ``buildall.py`` script:

.. code-block:: console

    python buildall.py --refcol

The shared library is placed in the ``bin/`` subdirectory of the build directory:

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


**On Linux**


.. code-block:: bash

    export INTEL_LIBITTNOTIFY64=<build_dir>/bin/libittnotify_refcol.so


**On FreeBSD**


.. code-block:: bash

    setenv INTEL_LIBITTNOTIFY64 <build_dir>/bin/libittnotify_refcol.so


**On Windows**


.. code-block:: bat

    set INTEL_LIBITTNOTIFY64=<build_dir>\bin\libittnotify_refcol.dll


Log File Location
-----------------

By default, log files are saved in the system temporary directory. Each run
creates a file named ``libittnotify_refcol_<timestamp>.log``. To change the
location, use the ``INTEL_LIBITTNOTIFY_LOG_DIR`` environment variable:


**On Linux**


.. code-block:: bash

    export INTEL_LIBITTNOTIFY_LOG_DIR=<log_dir>


**On FreeBSD**


.. code-block:: bash

    setenv INTEL_LIBITTNOTIFY_LOG_DIR <log_dir>


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

