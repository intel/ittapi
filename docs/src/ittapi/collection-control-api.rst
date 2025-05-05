.. _collection-control-api:

Collection Control API
======================


You can use the collection control APIs in your code to control the way
the Intel® VTune™ Profiler collects data for applications.


.. list-table:: 
   :header-rows: 0

   * -     Use This Primitive    
     -     To Do This    
   * -     \ ``void                   __itt_pause (void)``\    
     -      Run the application without collecting data. VTune Profiler reduces the overhead of collection, by collecting only critical information, such as thread and process creation.    
   * -     \ ``void                   __itt_resume (void)``\    
     -     Resume data collection. VTune Profiler resumes collecting all data.    
   * -     \ ``void                   __itt_detach (void)``\    
     -     Detach data collection. VTune Profiler detaches all collectors from all processes. Your application continues to work but no data is collected for the running collection.    




Pausing the data collection has the following effects:


-  Data collection is paused for the whole program, not only within the
   current thread.


-  Some runtime analysis overhead reduction.


-  The following APIs are not affected by pausing the data collection:


   -  Domain API


   -  String Handle API


   -  Thread Naming API


-  The following APIs are affected by pausing the data collection. Data
   is not collected for these APIs while in paused state:


   -  Task API


   -  Frame API


   -  Event API


   -  User-Defined Synchronization API


.. note::


   The Pause/Resume API call frequency is about 1Hz for a reasonable
   rate. Since this operation pauses and resumes data collection in all
   processes in the analysis run with the corresponding collection state
   notification to GUI, you are not recommended to call it on frequent
   basis for small workloads. For small workloads, consider using the
   `Frame
   APIs <frame-api.html>`__.


Usage Example: Focus on Specific Code Section
---------------------------------------------


The pause/resume calls shown in the following code snippet enable you to
focus the collection on a specific section of code, and start the
application run with collection paused.


.. code:: cpp


   int main(int argc, char* argv[])
   {
     // Do initialization work here
     __itt_resume();
       // Do profiling work here
     __itt_pause();
     // Do finalization work here
     return 0;
   }


Usage Example: Hide Sections of Code
------------------------------------


The pause/resume calls shown in the following code snippet enable you to
hide some intensive work that you are not currently focusing on:


.. code:: cpp


   int main(int argc, char* argv[])
   {
     // Do work here
     __itt_pause();
     // Do uninteresting work here
     __itt_resume();
     // Do work here
     __itt_detach();
     // Do uninteresting work here
     return 0;
   }

