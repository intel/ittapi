.. _overview:

Overview
========


When you use Intel analyzer tools to improve the performance of your software
application, during execution, use the Intel® Instrumentation and Tracing
Technology (ITT) and Just-In-Time (JIT) APIs to instrument your code to generate
trace data and control its collection. You use the ITT/JIT APIs to identify and
measure specific areas of code to get insights into performance bottlenecks and
resource utilization.


Components
----------


-  **ITT API**: Powers your application to generate and control the collection
   of trace data during its execution, seamlessly integrating with Intel tools.
-  **JIT API**: Reports detailed information about just-in-time (JIT) compiled
   code, enabling you to profile the performance of dynamically generated code.


Architecture
------------


The ITT/JIT APIs consist of two parts:

-  **Static Part**: An open-source static library
   (`ittapi <https://github.com/intel/ittapi>`__) that you compile and link
   with your application to enable tracing features.
-  **Dynamic Part**: A tool-specific shared library that collects and writes
   trace data. You can find the reference implementation of the dynamic part
   as a *Reference Collector* `here <ref_collector.html>`__.

