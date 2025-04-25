.. _using-jit-api:

Using JIT API
=============


To include JIT Profiling support, do one of the following:


-  


   .. container::
      :name: LI_DB9BBDF5654E49BA9743FA34A8BEFC9C


      Include the following files to your source tree:


      -  


         .. container::
            :name: LI_EABD97D44F984162B38420A57167AC5B


            ``jitprofiling.h``, located under <install-dir>\include
            (Windows*) or <install-dir>/include (Linux*)


      -  


         .. container::
            :name: LI_6E4F12C743D144E4A1515F4829612FB3


            ``ittnotify_config.h``, ``ittnotify_types.h`` and
            ``jitprofiling.c``, located under
            <install-dir>\sdk\src\ittnotify (Windows*) or
            <install-dir>/sdk/src/ittnotify (Linux*)


      .. note::


         To locate your <install-dir> see `Installation
         Directory <#INSTALL-DIR-JIT>`__.


-  


   .. container::
      :name: LI_1B044705871E4F2F90A3733AAB95D071


      Use the static library provided with the product:


      #. 


         .. container::
            :name: LI_EBB0F0A78ADC45FD973DB21B3CEFF629


            Include\ ``jitprofiling.h`` file, located under the
            <install-dir>\include (Windows*) or <install-dir>/include
            (Linux*) directory, in your code. This header file provides
            all API function prototypes and type definitions.


      #. 


         .. container::
            :name: LI_C273EB329F1449B2A460BED51529BC6B


            Link to ``jitprofiling.lib`` (Windows*) or
            ``jitprofiling.a`` (Linux*), located under
            <install-dir>\lib32or <install-dir>\lib64 (Windows*) or
            <install-dir>/lib32 or <install-dir>/lib32 (Linux*).


.. list-table:: 
   :header-rows: 0

   * -     Use This Primitive    
     -     To Do This    
   * -     \ ``int iJIT_NotifyEvent( iJIT_JVM_EVENT event_type, void *EventSpecificData );``\    
     -     Use this API to send a notification of ``event_type`` with the data pointed by
           ``EventSpecificData`` to the agent. The reported information is used to attribute
           samples obtained from any profiling tool collector.    
   * -     \ ``unsigned int iJIT_GetNewMethodID( void );``\    
     -     Generate a new method ID. You must use this function to assign unique and valid
           method IDs to methods reported to the profiler. This API returns a new unique
           method ID. When out of unique method IDs, this API function returns 0.    
   * -     \ ``iJIT_IsProfilingActiveFlags iJIT_IsProfilingActive( void );``\    
     -     Returns the current mode of the profiler: off, or sampling, using the
           ``iJIT_IsProfilingActiveFlags`` enumeration. This API returns
           ``iJIT_SAMPLING_ON`` by default, indicating that Sampling is running.
           It returns ``iJIT_NOTHING_RUNNING`` if no profiler is running.    




Lifetime of Allocated Data
--------------------------


You send an event notification to the agent (Collector) with
event-specific data, which is a structure. The pointers in the structure
refer to memory you allocated and you are responsible for releasing it.
The pointers are used by the ``iJIT_NotifyEvent`` method to copy your
data in a trace file, and they are not used after the
``iJIT_NotifyEvent`` method returns.

