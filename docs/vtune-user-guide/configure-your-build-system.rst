.. _configure-your-build-system:

Configure Your Build System
===========================


.. note::


   ITT API usage is supported on Windows*, Linux*, FreeBSD*, and
   Android\* systems. It is not supported for QNX\* systems.


Before instrumenting your application, you need to configure your build
system to be able to reach the API headers and libraries.


For Windows\* and Linux\* systems:


-  Add ``<install_dir>/sdk/include`` to your INCLUDE path for C/C++
   applications or ``<install_dir>/sdk/[lib32 or lib64]`` to your
   INCLUDE path for Fortran applications
-  Add ``<install_dir>/sdk/lib32`` to your 32-bit LIBRARIES path
-  Add ``<install_dir>/sdk/lib64`` to your 64-bit LIBRARIES path


.. note::


   On Linux\* systems, you have to link the ``dl`` and ``pthread``
   libraries to enable ITT API functionality. Not linking these
   libraries will not prevent your application from running, but no ITT
   API data will be collected.


For FreeBSD\* systems:


.. note::


   Header and library files are available from the
   ``vtune_profiler_target_x86_64.tgz`` FreeBSD target package. See `Set
   Up FreeBSD\*
   System <set-up-freebsd-system.html>`__ for more
   information.


-  Add ``<target-package>/sdk/include`` to your INCLUDE path for C/C++
   applications or ``<install_dir>/sdk/[lib32 or lib64]`` to your
   INCLUDE path for Fortran applications
-  Add ``<target-package>/sdk/lib64`` to your 64-bit LIBRARIES path


For the Android\* system, add the following libraries to your LIBRARIES
path depending on your device architecture:


-  Add ``<install_dir>/target/android_v5_x86_64/lib-x86_64`` for the
   Intel® 64 architecture
-  Add ``<install_dir>/target/android_v5/lib-x86`` for the IA-32
   architecture
-  Add ``<install_dir>/target/android_arm/lib-arm`` for the ARM\*
   architecture


``<install_dir>`` is the Intel® VTune™ Profiler installation directory.
The default installation path for the VTune Profiler varies with the
product shipment.


.. note::


   The ITT API headers, static libraries, and Fortran modules previously
   located at ``<install_dir>/include`` and ``<install_dir>/lib32 [64]``
   folders were moved to the ``<install_dir>/sdk`` folder starting the
   VTune Profiler 2021.1-beta08 release. Copies of these files are
   retained at their old locations for backwards compatibility and these
   copies should not be used for new projects.


Include the ITT API Header or Module in Your Application
--------------------------------------------------------


**For C/C++ Applications**


Add the following ``#include`` statements to every source file that you
want to instrument:


.. code:: cpp


   #include <ittnotify.h>


The ``ittnotify.h`` header contains definitions of ITT API routines and
important macros which provide the correct logic of API invocation from
a user application.


The ITT API is designed to incur almost zero overhead when tracing is
disabled. But if you need fully zero overhead, you can compile out all
ITT API calls from your application by defining the
``INTEL_NO_ITTNOTIFY_API`` macro in your project at compile time, either
on the compiler command line, or in your source file, prior to including
the ``ittnotify.h`` file.


**For Fortran Applications**


Add the ``ITTNOTIFY`` module to your source files with the following
source line:


.. code:: 


   USE ITTNOTIFY


Insert ITT Notifications in Your Application
--------------------------------------------


Insert ``__itt_*`` (C/C++) or ``ITT_*`` (Fortran) notifications in your
source code.


C/C++ example:


.. code:: cpp


   __itt_pause();


Fortran example:


.. code:: 


   CALL ITT_PAUSE()


For more information, see `Instrumenting Your
Application <instrument-your-application.html>`__.


Link the libittnotify.a (Linux*, Android*, FreeBSD*) or libittnotify.lib (Windows*) Static Library to Your Application
----------------------------------------------------------------------------------------------------------------------


You need to link the static library, ``libittnotify.a`` (Linux*,
FreeBSD*, Android*) or ``libittnotify.lib`` (Windows*), to your
application. If tracing is enabled, this static library loads the ITT
API implementation and forwards ITT API instrumentation data to VTune
Profiler. If tracing is disabled, the static library ignores ITT API
calls, causing nearly zero instrumentation overhead.


After you instrument your application by adding ITT API calls to your
code and link the ``libittnotify.a`` (Linux*, FreeBSD*, Android*) or
``libittnotify.lib`` (Windows*) static library, your application will
check the ``INTEL_LIBITTNOTIFY32`` or the\ ``INTEL_LIBITTNOTIFY64``
environment variable depending on your application's architecture. If
that variable is set, it will load the libraries defined in the
variable.


Make sure to set these environment variables for the
``ittnotify_collector`` to enable data collection:


On Windows*:


``INTEL_LIBITTNOTIFY32=<install-dir>\bin32\runtime\ittnotify_collector.dll``


``INTEL_LIBITTNOTIFY64=<install-dir>\bin64\runtime\ittnotify_collector.dll``


On Linux*:


``INTEL_LIBITTNOTIFY32=<install-dir>/lib32/runtime/libittnotify_collector.so``


``INTEL_LIBITTNOTIFY64=<install-dir>/lib64/runtime/libittnotify_collector.so``


On FreeBSD*:


``INTEL_LIBITTNOTIFY64=<target-package>/lib64/runtime/libittnotify_collector.so``

