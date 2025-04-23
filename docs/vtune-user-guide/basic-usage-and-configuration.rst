.. _basic-usage-and-configuration:

Basic Usage and Configuration
=============================


You can control performance data collection for your application by
adding basic instrumentation to your application and by configuring your
environment and your build system to use the Instrumentation and Tracing
Technology (ITT) APIs.


User applications/modules linked to the static ITT API library do not
have a runtime dependency on a dynamic library. Therefore, they can be
executed without Intel® VTune™ Profiler.


To use the ITT APIs, set up your C/C++ or Fortran application using the
steps provided in `Configuring Your Build
System <configure-your-build-system.html>`__.


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


See Also
--------


.. container:: linklist


   `Minimize ITT API
   Overhead <minimize-itt-api-overhead.html>`__


`Configure Your Build
System <configure-your-build-system.html>`__


`Task Analysis <task-analysis.html>`__

.. toctree::
   :maxdepth: 1

   
   configure-your-build-system
   attach-itt-apis-to-a-launched-application
   instrument-your-application
   minimize-itt-api-overhead
   view-instrumentation-and-tracing-technology-itt-api-task-data-in-intel-vtune-profiler