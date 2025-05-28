.. _itt-api-support:

Instrumentation and Tracing Technology (ITT) API
================================================


Use the Intel® Instrumentation and Tracing Technology (ITT) API to generate
trace data and control its collection during the execution of your application.


Use the ITT API to:


-  Control application performance overhead based on the amount of
   traces that you collect.
-  Enable trace collection without having to recompiling your application
-  Enable code annotation for deeper analysis.

You can use the ITT API to collect trace data from C, C++, or Fortran
applications that run on Windows*, Linux* or FreeBSD* systems.


The ITT API has **static** and **dynamic** library components. The applications
and modules you link to the static library do not have a runtime dependency
on the dynamic library. Therefore, you can run these components independently.


ITT API Usage and Reference
---------------------------

.. toctree::
   :maxdepth: 1


   compile-and-link-with-itt-api
   instrument-your-application
   minimize-itt-api-overhead
   itt-api-reference

