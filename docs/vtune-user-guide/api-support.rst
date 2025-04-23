.. _api-support:

API Support
===========


Intel® VTune™ Profiler supports two kinds of APIs:


-  The Instrumentation and Tracing Technology API (ITT API) provided by
   the Intel® VTune™ Profiler enables your application to generate and
   control the collection of trace data during its execution.


-  The JIT (Just-In-Time) Profiling API provides functionality to report
   information about just-in-time generated code that can be used by
   performance tools. You must insert JIT Profiling API calls in the
   code generator to report information before JIT-compiled code goes to
   execution. This information is collected at runtime and used by tools
   like Intel® VTune™ Profiler to display performance metrics associated
   with JIT-compiled code.

.. toctree::
   :maxdepth: 1

   
   support-for-instrumentation-and-tracing-technology-api-itt-api
   jit-profiling-api
