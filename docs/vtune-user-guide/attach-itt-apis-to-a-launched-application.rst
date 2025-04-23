.. _attach-itt-apis-to-a-launched-application:

Attach ITT APIs to a Launched Application
=========================================


You can use the Intel® VTune™ Profiler to attach to a running
application instrumented with ITT API. But before launching the
application, make sure to set up the following environment variable for
the ``ittnotify_collector``:


On Windows*:


``INTEL_LIBITTNOTIFY32=<install-dir>\bin32\runtime\ittnotify_collector.dll``


``INTEL_LIBITTNOTIFY64=<install-dir>\bin64\runtime\ittnotify_collector.dll``


On Linux*:


``INTEL_LIBITTNOTIFY32=<install-dir>/lib32/runtime/libittnotify_collector.so``


``INTEL_LIBITTNOTIFY64=<install-dir>/lib64/runtime/libittnotify_collector.so``


On FreeBSD:


.. note::


   Header and library files are available from the
   ``vtune_profiler_target_x86_64.tgz`` FreeBSD target package. See `Set
   Up FreeBSD\*
   System <set-up-freebsd-system.html>`__ for more
   information.


``INTEL_LIBITTNOTIFY64=<target-package>/lib64/runtime/libittnotify_collector.so``


.. note::


   The variables should contain the full path to the library without
   quotes.


Example
-------


On Windows:


.. code:: bash


   set INTEL_LIBITTNOTIFY32=C:\Program Files (x86)\Intel\oneAPI\vtune\latest\bin32\runtime\ittnotify_collector.dll 
   set INTEL_LIBITTNOTIFY64=C:\Program Files (x86)\Intel\oneAPI\vtune\latest\bin64\runtime\ittnotify_collector.dll 


On Linux:


.. code:: bash


   export INTEL_LIBITTNOTIFY32=/opt/intel/oneapi/vtune/latest/lib32/runtime/libittnotify_collector.so
   export INTEL_LIBITTNOTIFY64=/opt/intel/oneapi/vtune/latest/lib64/runtime/libittnotify_collector.so


On FreeBSD:


.. note::


   You may need to change the path to reflect the placement of the
   FreeBSD target package on your target system.


.. code:: bash


   setenv INTEL_LIBITTNOTIFY64 /tmp/vtune_profiler_2021.9.0/lib64/runtime/libittnotify_collector.so


After you complete the configuration, you can start the instrumented
application in the correct environment and Intel® VTune™ Profiler will
collect user API data even if the application was launched before the
VTune Profiler.

