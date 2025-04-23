.. _home:

********************************************************************************************
Intel® Instrumentation and Tracing Technology (ITT) and Just-In-Time (JIT) API Documentation
********************************************************************************************

Introduction
============


.. note::


    The ITT and JIT API documentation from the Intel® VTune Profiler and Intel® Graphics
    Performance Analyzers User Guides has been consolidated into this repository,
    providing a single, unified source of information.


The ITT/JIT API is a open source feature of Intel's Analyzers tools that allows applications
to generate and control trace data during their execution. It enables developers to pinpoint
and measure specific areas of their code, providing valuable insights into performance
bottlenecks and resource utilization.


This open source profiling API includes:

-   Instrumentation and Tracing Technology (ITT) API.
    It enables your application to generate and control the collection of trace data during
    its execution across different Intel tools.
-   JIT (Just-In-Time) Profiling API.
    It provides functionality to report information about just-in-time generated code that
    can be used by performance profiling tools.

The ITT/JIT API consists of two parts: a *static* part and a *dynamic* part.

-   The dynamic part is specific for a tool and distributed only with a particular tool as
    a dynamic(shared) library.
-   The static part is a common part shared between tools. Currently, the static part is
    distributed as an open source static library: `ittapi <https://github.com/intel/ittapi>`__


.. toctree::
   :maxdepth: 1
   :caption: Contents:

   vtune-user-guide/vtune-user-guide.rst
   gpa-user-guide/gpa-user-guide.rst
