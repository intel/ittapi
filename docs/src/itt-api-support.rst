.. _itt-api-support:

ITT API
=======


The Instrumentation and Tracing Technology API (ITT API) enables
your application to generate and control the collection of trace
data during its execution.


ITT API has the following features:


-  Controls application performance overhead based on the amount of
   traces that you collect.
-  Enables trace collection without recompiling your application.
-  Supports applications in C/C++ and Fortran environments on Windows*,
   Linux* or FreeBSD* systems.
-  Supports instrumentation for tracing application code.


User applications/modules linked to the static ITT API library do not
have a runtime dependency on a dynamic library. Therefore, they can be
executed independently.


ITT API enables you to manually instrument your application for further
application performance analysis.


ITT API Usage and Reference
---------------------------

.. toctree::
   :maxdepth: 1


   compile-and-link-with-itt-api
   instrument-your-application
   minimize-itt-api-overhead
   itt-api-reference

