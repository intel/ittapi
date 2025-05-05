.. _jit-api-support:

JIT API
=======


The JIT (Just-In-Time) Profiling API provides functionality to report
information about just-in-time generated code that can be used by
performance tools. You must insert JIT Profiling API calls in the code
generator to report information before JIT-compiled code goes to
execution. This information is collected at runtime and used by tools
like Intel® VTune™ Profiler to display performance metrics associated
with JIT-compiled code.


You can use the JIT Profiling API to profile such environments as
dynamic JIT compilation of JavaScript code traces, JIT execution in
OpenCL™ applications, Java*/.NET\* managed execution environments, and
custom ISV JIT engines.


The JIT engine generating code during runtime communicates with a profiler
object (Collector) through the static part. During runtime, the JIT engine
reports the information about JIT-compiled code stored in a trace file by
the profiler object. After collection, profiling tool uses the generated
trace file to resolve the JIT-compiled code.


Use the JIT Profiling API to:


-  :ref:`Profile trace-based and method-based JIT-compiled code`


-  :ref:`Analyze split functions`


-  :ref:`Explore inline functions`


JIT profiling is supported with the **Launch Application** target option
for event based sampling.


Environment Variables in the JIT Profiling API
----------------------------------------------


The JIT Profiling API contains two environment variables, which in turn
contain paths to specific runtime libraries.


-  ``INTEL_JIT_PROFILER32``
-  ``INTEL_JIT_PROFILER64``


These variables are used to signal the replacement of the stub
implementation of the JIT API with the JIT API collector.
Once you instrument your code with the JIT API and link it to the
JIT API stub (``libjitprofiling.lib/libjitprofiling.a``), your code
loads the libraries defined in these environment variables, once the
variables are set.


Make sure to set these environment variables for the ittnotify_collector
to enable data collection:   

On Windows*:
   
.. code-block:: bash

   INTEL_JIT_PROFILER32=<install-dir>\bin32\runtime\ittnotify_collector.dll
   INTEL_JIT_PROFILER64=<install-dir>\bin64\runtime\ittnotify_collector.dll

On Linux*:

.. code-block:: bash

   INTEL_JIT_PROFILER32=<install-dir>/lib32/runtime/libittnotify_collector.so
   INTEL_JIT_PROFILER64=<install-dir>/lib64/runtime/libittnotify_collector.so    

On FreeBSD*:

.. code-block:: bash

   INTEL_JIT_PROFILER64=<target-package>/lib64/runtime/libittnotify_collector.so  


.. _Profile trace-based and method-based JIT-compiled code :

Profile Trace-based and Method-based JIT-compiled Code
------------------------------------------------------


This is the most common scenario for using JIT Profiling API to profile
trace-based and method-based JIT-compiled code:


.. code:: cpp


   #include <jitprofiling.h>


   if (iJIT_IsProfilingActive() != iJIT_SAMPLING_ON) {
       return;
   }


   iJIT_Method_Load jmethod = {0};
   jmethod.method_id = iJIT_GetNewMethodID();
   jmethod.method_name = "method_name";
   jmethod.class_file_name = "class_name";
   jmethod.source_file_name = "source_file_name";
   jmethod.method_load_address = code_addr;
   jmethod.method_size = code_size;


   iJIT_NotifyEvent(iJVM_EVENT_TYPE_METHOD_LOAD_FINISHED,
         (void*)&jmethod);
   iJIT_NotifyEvent(iJVM_EVENT_TYPE_SHUTDOWN, NULL);


**Usage Tips**


-  If any ``iJVM_EVENT_TYPE_METHOD_LOAD_FINISHED`` event overwrites an
   already reported method, then such a method becomes invalid and its
   memory region is treated as unloaded.


-  If supplied line number information contains multiple source lines
   for the same assembly instruction (code location), then profiling
   tool picks up the first line number.


-  Dynamically generated code can be associated with a module name. Use
   the ``iJIT_Method_Load_V2`` structure.


-  If you register a function with the same method ID multiple times,
   specifying different module names, then profiling tool picks
   up the module name registered first. If you want to distinguish
   the same function between different JIT engines, supply different
   method IDs for each function. Other symbolic information (for
   example, source file) can be identical.

.. _Analyze split functions :

Analyze Split Functions
-----------------------


You can use the JIT Profiling API to analyze split functions (multiple
joint or disjoint code regions belonging to the same function) including
re-JITting with potential overlapping of code regions in time, which is
common in resource-limited environments.


.. code:: cpp


    

    #include <jitprofiling.h>
    

    unsigned int method_id = iJIT_GetNewMethodID();
    

    iJIT_Method_Load a = {0};
    a.method_id = method_id;
    a.method_load_address = 0x100;
    a.method_size = 0x20;
    

    iJIT_Method_Load b = {0};
    b.method_id = method_id;
    b.method_load_address = 0x200;
    b.method_size = 0x30;
    

    iJIT_NotifyEvent(iJVM_EVENT_TYPE_METHOD_LOAD_FINISHED, (void*)&a);
    iJIT_NotifyEvent(iJVM_EVENT_TYPE_METHOD_LOAD_FINISHED, (void*)&b) 


**Usage Tips**


-  If a ``iJVM_EVENT_TYPE_METHOD_LOAD_FINISHED`` event overwrites an
   already reported method, then such a method becomes invalid and its
   memory region is treated as unloaded.


-  All code regions reported with the same method ID are considered as
   belonging to the same method. Symbolic information (method name,
   source file name) will be taken from the first notification, and all
   subsequent notifications with the same method ID will be processed
   only for line number table information.


-  If you register a second code region with a different source file
   name and the same method ID, this information will be saved and
   will not be considered as an extension of the first code region,
   but profiling tool will use the source file of the first code
   region and map performance metrics incorrectly.


-  If you register a second code region with the same source file as
   for the first region and the same method ID, the source file will
   be discarded but profiling tool will map metrics to the source
   file correctly.


-  If you register a second code region with a null source file and
   the same method ID, provided line number info will be associated
   with the source file of the first code region.

.. _Explore inline functions:

Explore Inline Functions
------------------------


You can use the JIT Profiling API to explore inline functions including
multi-level hierarchy of nested inline methods that shows how
performance metrics are distributed through them.


.. code:: cpp


   #include <jitprofiling.h>


     //                                    method_id   parent_id
     //   [-- c --]                          3000        2000
     //                  [---- d -----]      2001        1000
     //  [---- b ----]                       2000        1000
     // [------------ a ----------------]    1000         n/a
    

   iJIT_Method_Load a = {0};
   a.method_id = 1000;
    

   iJIT_Method_Inline_Load b = {0};
   b.method_id = 2000;
   b.parent_method_id = 1000;


   iJIT_Method_Inline_Load c = {0};
   c.method_id = 3000;
   c.parent_method_id = 2000;


   iJIT_Method_Inline_Load d = {0};
   d.method_id = 2001;
   d.parent_method_id = 1000;


   iJIT_NotifyEvent(iJVM_EVENT_TYPE_METHOD_LOAD_FINISHED, (void*)&a);
   iJIT_NotifyEvent(iJVM_EVENT_TYPE_METHOD_INLINE_LOAD_FINISHED, (void*)&b);
   iJIT_NotifyEvent(iJVM_EVENT_TYPE_METHOD_INLINE_LOAD_FINISHED, (void*)&c);
   iJIT_NotifyEvent(iJVM_EVENT_TYPE_METHOD_INLINE_LOAD_FINISHED, (void*)&d);


**Usage Tips**


-  Each inline (``iJIT_Method_Inline_Load``) method should be associated
   with two method IDs: one for itself; one for its immediate parent.


-  Address regions of inline methods of the same parent method cannot
   overlap each other.


-  Execution of the parent method must not be started until it and all
   its inline methods are reported.


-  In case of nested inline methods an order of
   ``iJVM_EVENT_TYPE_METHOD_INLINE_LOAD_FINISHED`` events is not
   important.


-  If any event overwrites either inline method or top parent method,
   then the parent, including inline methods, becomes invalid and its
   memory region is treated as unloaded.


See Also
--------


.. toctree::
   :maxdepth: 1

   
   using-jit-api
   jit-api-reference

