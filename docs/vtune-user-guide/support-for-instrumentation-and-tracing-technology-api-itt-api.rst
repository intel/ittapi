.. _support-for-instrumentation-and-tracing-technology-api-itt-api:

Support for Instrumentation and Tracing Technology API (ITT API)
================================================================


.. note::


   The Instrumentation and Tracing Technology API (ITT API) and the
   Just-in-Time Profiling API (JIT API) are open source components.
   Visit the `GitHub\* repository <https://github.com/intel/ittapi>`__
   to access source code and contribute.


The Instrumentation and Tracing Technology API (ITT API) provided by the
Intel® VTune™ Profiler enables your application to generate and control
the collection of trace data during its execution.


ITT API has the following features:


-  Controls application performance overhead based on the amount of
   traces that you collect.
-  Enables trace collection without recompiling your application.
-  Supports applications in C/C++ and Fortran environments on Windows*,
   Linux*, FreeBSD*, or Android\* systems.
-  Supports instrumentation for tracing application code.


To use the APIs, add API calls in your code to designate logical tasks.
These markers will help you visualize the relationship between tasks in
your code relative to other CPU and GPU tasks. To see user tasks in your
performance analysis results, enable the **Analyze user tasks** checkbox
in analysis settings.


.. note::


   The ITT API is a set of pure C/C++ functions. There are no Java\* or
   .NET\* APIs. If you need runtime environment support, you can use a
   JNI, or C/C++ function call from the managed code. If the collector
   causes significant overhead or data storage, you can pause the
   analysis to reduce the overhead.


See Also
--------


.. container:: linklist


   `Task Analysis <task-analysis.html>`__


`View Instrumentation and Tracing Technology (ITT) API Task Data in
Intel® VTune™
Profiler <view-instrumentation-and-tracing-technology-itt-api-task-data-in-intel-vtune-profiler.html>`__

.. toctree::
   :maxdepth: 1

   
   basic-usage-and-configuration
   instrumentation-and-tracing-technology-api-reference