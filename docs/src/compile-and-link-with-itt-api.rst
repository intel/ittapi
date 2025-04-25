.. _compile-and-link-with-itt-api:

Compile and Link with ITT API
=============================


Before instrumenting your application, configure your build system to be
able to reach the API headers and libraries:


-  Add ``<ittapi_dir>\sdk\include`` to your ``INCLUDE`` path
-  Add ``<ittapi_dir>\build_<target_platform>\<target_bits>\bin``
   to your ``LIBRARIES`` path


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


.. code:: cpp


   USE ITTNOTIFY


Insert ITT Notifications in Your Application
--------------------------------------------


Insert ``__itt_*`` (C/C++) or ``ITT_*`` (Fortran) notifications in your
source code.


C/C++ example:


.. code:: cpp


   __itt_pause();


Fortran example:


.. code:: cpp


   CALL ITT_PAUSE()


For more information, see `Instrumenting Your Application
<instrument-your-application.html>`__ and `ITT API Reference
<itt-api-reference.html>`__.


Link the libittnotify Static Library to Your Application
--------------------------------------------------------


You need to link the static library, ``libittnotify.a`` (Linux*, FreeBSD*) or
``libittnotify.lib`` (Windows*), to your application. If tracing is enabled,
this static library loads the ITT API data dynamic collector and forwards ITT
API instrumentation data to it. If tracing is disabled, the static library
ignores ITT API calls, providing nearly zero instrumentation overhead.


Set the INTEL_LIBITTNOTIFY environment variables to enable data collection
--------------------------------------------------------------------------


After you instrument your application by adding ITT API calls to your code and
link the ``libittnotify.a/libittnotify.lib`` static library, your application
will check the ``INTEL_LIBITTNOTIFY32`` or the ``INTEL_LIBITTNOTIFY64``
environment variable depending on your application's architecture to load the
**dynamic** part of the library. If that variable is set, it will load the
libraries defined in the variable.


Make sure to set these environment variables to enable data collection:


**On Windows**


.. code-block:: console
   
   
   set INTEL_LIBITTNOTIFY32=<install-dir>\bin32\runtime\ittnotify_collector.dll
   set INTEL_LIBITTNOTIFY64=<install-dir>\bin64\runtime\ittnotify_collector.dll


**On Linux**


.. code-block:: console


   export INTEL_LIBITTNOTIFY32=<install-dir>/lib32/runtime/libittnotify_collector.so
   export INTEL_LIBITTNOTIFY64=<install-dir>/lib64/runtime/libittnotify_collector.so


**On FreeBSD**


.. code-block:: console
   
   
   setenv INTEL_LIBITTNOTIFY64=<target-package>/lib64/runtime/libittnotify_collector.so


Unicode Support
---------------


All API functions that take parameters of type ``__itt_char`` follow the
Windows OS unicode convention. If UNICODE is defined when compiling on a
Windows OS, ``__itt_char`` is ``wchar_t``, otherwise it is ``char``. The
actual function names are suffixed with ``A`` for the ASCII APIs and
``W`` for the unicode APIs. Both types of functions are defined in the
DLL that implements the API.


Strings that are all ASCII characters are internally equivalent for both
the unicode and the ASCII API versions. For example, the following
strings are equivalent:


.. code:: cpp


   __itt_sync_createA( addr, "OpenMP Scheduler", "Critical Section", 0); 
   __itt_sync_createW( addr, L"OpenMP Scheduler", L"Critical Section", 0); 

