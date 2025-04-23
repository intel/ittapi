.. _using-jit-profiling-api:

Using JIT Profiling API
=======================


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
   * -     \ ``int                   iJIT_NotifyEvent( iJIT_JVM_EVENT event_type, void *EventSpecificData );``\    
     -     Use this API to send a notification of ``event_type`` with the data pointed by ``EventSpecificData`` to the agent. The reported information is used to attribute samples obtained from any Intel® VTune™ Profiler collector.    
   * -     \ ``unsigned int                   iJIT_GetNewMethodID( void );``\    
     -     Generate a new method ID. You must use this function to assign unique and valid method IDs to methods reported to the profiler.       This API returns a new unique method ID. When out of unique method IDs, this API function returns 0.    
   * -     \ ``iJIT_IsProfilingActiveFlags                   iJIT_IsProfilingActive( void );``\    
     -     Returns the current mode of the profiler: off, or sampling, using the ``iJIT_IsProfilingActiveFlags`` enumeration.       This API returns ``iJIT_SAMPLING_ON`` by default, indicating that Sampling is running. It returns ``iJIT_NOTHING_RUNNING`` if no profiler is running.    




Lifetime of Allocated Data
--------------------------


You send an event notification to the agent (VTune Profiler) with
event-specific data, which is a structure. The pointers in the structure
refer to memory you allocated and you are responsible for releasing it.
The pointers are used by the ``iJIT_NotifyEvent`` method to copy your
data in a trace file, and they are not used after the
``iJIT_NotifyEvent`` method returns.


JIT Profiling API Sample Application
------------------------------------


VTune Profiler is installed with a sample application in the
``jitprofiling_vtune_amp_xe.zip`` (Windows*) or
``jitprofiling_vtune_amp_xe.tgz`` (Linux*) that emulates the creation
and execution of dynamic code. In addition, it uses the JIT profiling
API to notify the VTune Profiler when it transfers execution control to
dynamic code.


**To install and set up the sample code:**


#. Copy the ``jitprofiling_vtune_amp_xe.zip`` (Windows*) or
   ``jitprofiling_vtune_amp_xe.tgz`` (Linux*) file from the
   <install-dir>\samples\<locale>\C++ (Windows*) or
   <install-dir>/samples/<locale>/C++ (Linux*)directory to a writable
   directory or share on your system.
#. Extract the sample from the archive file.


Build jitprofiling.c in Microsoft Visual Studio\*
-------------------------------------------------


#. Copy the ``jitprofiling_vtune_amp_xe.zip`` file from the
   <install-dir>\samples\<locale>\C++ directory to a writable directory
   or share on your system.
#. Extract the sample from the ``.zip`` file.
#. Open the ``jitprofiling.sln`` file with Microsoft Visual Studio.
#. Right-click **jitprofiling** in the **Solution Explorer** and select
   **Properties**. The **jitprofiling Property Pages** window opens.
#. Set the **Platform** (top of the window) to **x64**.
#. Select **Configuration Properties** > **C/C++** > **General** and add
   the path to the headers (``<install-dir>``/include) to **Additional
   Include Directories**.
#. Select **Configuration Properties** > **C/C++** > **Linker** >
   **General** and add the path to the library (``<install-dir>``/lib32
   or ``<install-dir>``/lib64) to **Additional Library Directories**.
#. Click **OK** to apply the changes and close the window.
#. Rebuild the solution with the new project settings.


Installation Information
------------------------


Follow these instructions in the Installation Guide to install VTune
Profiler on:


-  `Windows
   host <https://www.intel.com/content/www/us/en/docs/vtune-profiler/installation-guide/current/windows.html>`__
-  `Linux
   host <https://www.intel.com/content/www/us/en/docs/vtune-profiler/installation-guide/current/linux.html>`__


Whether you download Intel® VTune™ Profiler as a standalone component or
with the Intel® oneAPI Base Toolkit, the default path for your
``<install-dir>`` is:


.. list-table:: 
   :header-rows: 0

   * -  Operating System
     -  Path to <install-dir>
   * -     Windows\* OS    
     - 
   * -     Linux\* OS    
     - 




Make sure that your installation of Intel® VTune™ Profiler has also
installed the `sampling
drivers <https://www.intel.com/content/www/us/en/docs/vtune-profiler/user-guide/current/sep-driver.html>`__
necessary for data collection.

