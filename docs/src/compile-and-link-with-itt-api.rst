.. _compile-and-link-with-itt-api:

Compile and Link with ITT API
=============================


Step 1: Configure Your Build System
-----------------------------------


Before instrumenting your application with ITT API, configure your build
system to establish access to the headers and libraries of the API:


-  Add ``<ittapi_dir>\include`` to your ``INCLUDE`` path
-  Add ``<ittapi_dir>\build_<target_platform>\<target_bits>\bin``
   to your ``LIBRARIES`` path


Step 2: Include the ITT API Header/Module in Your Application
-------------------------------------------------------------


**C/C++ Applications**


For every source file that you want to instrument, add the following
``#include`` statements:


.. code-block:: cpp


   #include <ittnotify.h>


The ``ittnotify.h`` header contains definitions for ITT API routines
and important macros that provide the correct logic to invoke the API
from your application.


When tracing is disabled, The ITT API incur almost zero overhead. To achieve
completely zero overhead, you can compile out all ITT API calls from your
application. To do this, prior to including the ``ittnotify.h`` file, define
the ``INTEL_NO_ITTNOTIFY_API`` macro in your project at compile time. You can
do this from the compiler command line or in your source file.


**Fortran Applications**


Add the ``ITTNOTIFY`` module to your source files. Use the following
source line:


.. code-block:: cpp


   USE ITTNOTIFY


Step 3: Insert ITT Notifications in Your Application
----------------------------------------------------


To insert ITT notifications in your application, use:

+-----------+-------------------+-----------------------------+
| Language  | Notification      | Example                     |
+===========+===================+=============================+
| C/C++     | .. code:: cpp     | .. code:: cpp               |
|           |                   |                             |
|           |    __itt_*        |    __itt_pause();           |
+-----------+-------------------+-----------------------------+
| Fortran   | .. code:: cpp     | .. code:: cpp               |
|           |                   |                             |
|           |    ITT_*          |    CALL ITT_PAUSE()         |
+-----------+-------------------+-----------------------------+


To learn more, open:


-  `Instrumenting Your Application <instrument-your-application.html>`__
-  `ITT API Reference <itt-api-reference.html>`__


Step 4: Link the libittnotify Static Library to Your Application
----------------------------------------------------------------


Once you finish inserting ITT notifications in your application, the next step
is to link the libittnotify static library. This library is ``libittnotify.a``
in Linux* and FreeBSD* systems and ``libittnotify.lib`` in Windows* systems.

If you have enabled tracing, the static library loads the dynamic collector of
the ITT API data and forwards to the collector instrumentation data from the ITT API.

If you have disabled tracing, the static library ignores ITT API calls, resulting in
near-zero instrumentation overhead.


Step 5: Load the Dynamic Library
--------------------------------


After you instrument your application and link the static library, you must
load the dynamic library of the ITT API to your application. To do this,
depending on your system architecture, set the ``INTEL_LIBITTNOTIFY32`` or
the ``INTEL_LIBITTNOTIFY64`` environment variable.


**Windows OS:**


.. code-block:: bash
   
   
   set INTEL_LIBITTNOTIFY32=<install-dir>\bin32\runtime\ittnotify_collector.dll
   set INTEL_LIBITTNOTIFY64=<install-dir>\bin64\runtime\ittnotify_collector.dll


**Linux OS:**


.. code-block:: bash


   export INTEL_LIBITTNOTIFY32=<install-dir>/lib32/runtime/libittnotify_collector.so
   export INTEL_LIBITTNOTIFY64=<install-dir>/lib64/runtime/libittnotify_collector.so


**FreeBSD OS:**


.. code-block:: bash
   
   
   setenv INTEL_LIBITTNOTIFY64=<target-package>/lib64/runtime/libittnotify_collector.so


Additional Information: Unicode Support
---------------------------------------


All API functions that take parameters of type ``__itt_char`` follow the
Windows OS Unicode convention.

When compilation happens on a Windows system, if the ``UNICODE`` macro is
defined, ``__itt_char`` is set to ``wchar_t``. If the ``UNICODE`` macro is
not defined, ``__itt_char`` is set to ``char``.

The actual function names are suffixed with ``A`` for the ASCII APIs and
``W`` for the Unicode APIs. Both types of functions are defined in the
DLL that implements the API.

Strings that contain only ASCII characters are internally equivalent for both
the Unicode and ASCII API versions. For example, the following
strings are equivalent:


.. code-block:: cpp


   __itt_sync_createA( addr, "OpenMP Scheduler", "Critical Section", 0); 
   __itt_sync_createW( addr, L"OpenMP Scheduler", L"Critical Section", 0); 

