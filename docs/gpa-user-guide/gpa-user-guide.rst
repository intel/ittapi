.. _gpa-user-guide:

******************
API Support in GPA
******************


.. note::


   The Instrumentation and Tracing Technology API (ITT API) and the
   Just-in-Time Profiling API (JIT API) are open source components.
   Visit the `GitHub\* repository <https://github.com/intel/ittapi>`__
   to access source code and contribute.


The Instrumentation and Tracing Technology API (ITT API) provided by the
Intel GPA enables your application to generate and control the
collection of trace data during its execution.


ITT API has the following features:


-  Controls application performance overhead based on the amount of
   traces that you collect.
-  Enables trace collection without recompiling your application.
-  Supports applications in C/C++ environments.
-  Automatically traces data from Microsoft DirectX\* applications.
-  Automatically traces data from OpenGL\* ES v1, v2, and v3 APIs on
   Android\* OS applications.
-  Supports instrumentation for tracing application code.


This chapter describes how to add basic instrumentation to your
application, and also includes examples of several advanced
instrumentation topics.


User applications/modules linked to the static ITT API library do not
have a runtime dependency on a dynamic library. Therefore, they can be
executed without Intel GPA or other Intel tools.


ITT API enables you to manually instrument your application for further
application performance analysis.


You can use the Intel GPA Trace Analyzer to view ITT trace data. The
Intel GPA Trace Analyzer obtains this data from your application through
the Intel GPA Monitor.


.. note::


   The ITT API is a set of pure C/C++ functions. There are no Java\* or
   .NET\* APIs. If you need runtime environment support, you can use a
   JNI, or C/C++ function call from the managed code. If the collector
   causes significant overhead or data storage, you can pause the
   analysis to reduce the overhead.


See Also
--------


.. toctree::
   :maxdepth: 1

   
   compile-and-link-with-itt-api
   instrumenting-your-application
   minimizing-itt-api-overhead
   clock-domain-api
   counter-api
   domain-api
   marker-api
   metadata-api
   relation-api
   string-handle-api
   thread-naming-api
   task-api