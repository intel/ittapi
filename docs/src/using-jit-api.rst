.. _using-jit-api:

Compile and Link with JIT API
=============================


To include JIT Profiling support, do one of the following:


-  Include the following files to your source tree:

   #.  ``jitprofiling.h``, located under ``<ittapi_dir>\include`` directory,
       in your code. This header file provides all API function prototype
       and type definitions.
   #.  ``ittnotify_config.h``, ``ittnotify_types.h`` and ``jitprofiling.c``,
       located under ``<ittapi_dir>/src/ittnotify`` directory.

-  Link the jitprofiling Static Library:

   #.  ``jitprofiling.h``, located under ``<ittapi_dir>\include`` directory,
       in your code. This header file provides all API function prototype
       and type definitions.
   #.  Link to ``jitprofiling.lib`` (Windows*) or ``jitprofiling.a`` (Linux*),
       located under ``<ittapi_dir>\build_<target_platform>\<target_bits>\bin``
       directory.


+----------------------------------------------------------------+-------------------------------------------------------------------------------+
| Use This Primitive                                             | To Do This                                                                    |
+================================================================+===============================================================================+
| .. code-block:: cpp                                            | Use this API to send a notification of ``event_type`` with the data pointed   |
|                                                                | by ``EventSpecificData`` to the agent. The reported information is used to    |
|    int iJIT_NotifyEvent( iJIT_JVM_EVENT event_type,            | attribute samples obtained from any profiling tool collector.                 |
|        void *EventSpecificData );                              |                                                                               |
+----------------------------------------------------------------+-------------------------------------------------------------------------------+
| .. code-block:: cpp                                            | Generate a new method ID. You must use this function to assign unique and     |
|                                                                | valid method IDs to methods reported to the profiler. This API returns a new  |
|    unsigned int iJIT_GetNewMethodID( void );                   | unique method ID. When out of unique method IDs, this API function returns 0. |
+----------------------------------------------------------------+-------------------------------------------------------------------------------+
| .. code-block:: cpp                                            | Returns the current mode of the profiler: off, or sampling, using the         |
|                                                                | ``iJIT_IsProfilingActiveFlags`` enumeration. This API returns                 |
|    iJIT_IsProfilingActiveFlags iJIT_IsProfilingActive( void ); | ``iJIT_SAMPLING_ON`` by default, indicating that Sampling is running.         |
|                                                                | It returns ``iJIT_NOTHING_RUNNING`` if no profiler is running.                |
+----------------------------------------------------------------+-------------------------------------------------------------------------------+


Lifetime of Allocated Data
--------------------------


You send an event notification to the agent (Collector) with event-specific
data, which is a structure. The pointers in the structure refer to memory that
you allocated. You are responsible for releasing the allocated memory. The
``iJIT_NotifyEvent`` method uses these pointers to copy your data in a trace
file. The pointers are not used after the ``iJIT_NotifyEvent`` method returns.

