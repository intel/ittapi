.. _compile-and-link-with-itt-api:

Compile and Link with ITT API
=============================


Before instrumenting your application, configure your build system to be
able to reach the API headers and libraries:


-  Add ``<gpa_install_dir>\sdk\include`` to your ``INCLUDE`` path
-  Add ``<gpa_install_dir>\sdk\lib\x86`` to your 32-bit ``LIBRARIES``
   path
-  Add ``<gpa_install_dir>\sdk\lib\x64`` to your 64-bit ``LIBRARIES``
   path


The default installation directory on Windows\* OS is
``C:\Program Files\IntelSWTools\GPA``.


Include the ITT API Header in Your Application
----------------------------------------------


Add the following ``#include`` statements to every source file that you
want to instrument:


.. code-block:: cpp


   #include <ittnotify.h>


The ``ittnotify.h`` header contains definitions of ITT API routines and
important macros which provide the correct logic of API invocation from
a user application.


.. note::


   The\ ``ittnotify.h`` header defines many functions that are not
   covered in this document. However, Intel® GPA supports only the
   functions that are documented herein.


The ITT API is designed to incur almost zero overhead when tracing is
disabled. But if you need fully zero overhead, you can compile out all
ITT API calls from your application by defining the
INTEL_NO_ITTNOTIFY_API macro in your project at compile time, either on
the compiler command line, or in your source file, prior to including
the ``ittnotify.h``\ file.


Link the ``libittnotify.lib`` Static Library to Your Application
----------------------------------------------------------------


You need to link the static library, ``libittnotify.lib``, to your
application. If tracing is enabled, this static library loads the ITT
API data collector and forwards ITT API instrumentation data to Intel
GPA.If tracing is disabled, the static library ignores ITT API calls,
providing nearly zero instrumentation overhead.


.. note::


   If you are running Intel® GPA on a system with a legacy version of
   Microsoft\* Visual Studio*, you must have the corresponding security
   updates installed to link against the dynamic version of the
   ``libittnotify.lib`` static library:


   .. list-table:: 
      :header-rows: 1

      * -  Microsoft\* Visual Studio\* Version
        -  Security Update
      * -  Microsoft Visual Studio 2005
        -  KB2538218
      * -  Microsoft Visual Studio 2008
        -  KB2538241




If you do not want to update your version of Visual Studio, you must
link the ``libittnotify.lib`` static library to your application.


UNICODE Support
---------------


All ITT API functions with string arguments follow the Windows OS
UNICODE convention, the actual function names are suffixed with A for
the ASCII API and W for the UNICODE API. For example:


.. code-block:: cpp


   __itt_domain_createA( const char* ); 
   __itt_domain_createW( const wchar_t* );


To facilitate cross-platform code development, the ITT API features the
C macro ``__itt_domain_create``. If a UNICODE macro is defined when
compiling on a Windows OS, the C macro resolves into
``__itt_domain_createW``, otherwise it resolves into
``__itt_domain_createA``.


Conditional Compilation
-----------------------


For best performance in the release version of your code, use
conditional compilation to turn off annotations. Define the macro
INTEL_NO_ITTNOTIFY_API before you include ``ittnotify.h`` during
compilation to eliminate all ``__itt_*`` functions from your code.


You can also remove the static library from the linking stage by
defining this macro.

