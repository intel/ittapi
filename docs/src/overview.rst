.. _overview:

Overview
========

The Intel® Instrumentation and Tracing Technology (ITT) and Just-In-Time (JIT) API is an open
source feature of Intel's Analyzers tools that allows applications to generate and control
trace data during their execution. It enables developers to pinpoint and measure specific areas
of their code, providing valuable insights into performance bottlenecks and resource utilization.


The ITT/JIT API includes:

-  Instrumentation and Tracing Technology (ITT) API.
   It enables your application to generate and control the collection of trace data during
   its execution across different Intel tools.
-  JIT (Just-In-Time) Profiling API.
   It provides functionality to report information about just-in-time generated code that
   can be used by performance profiling tools.

The ITT/JIT API consists of two parts: a **static** part and a **dynamic** part.

-  The **dynamic** part is specific for a tool and distributed only with a particular tool as
   a dynamic(shared) library. The reference implementation of the **dynamic** part is provided
   `here <ref_collector>`__ as a Reference Collector.
-  The **static** part is a common part shared between tools. Currently, the static part is
   distributed as an open source static library: `ittapi <https://github.com/intel/ittapi>`__
