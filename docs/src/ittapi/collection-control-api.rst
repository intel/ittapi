.. _collection-control-api:

Collection Control API
======================


Use Collection Control APIs in your code to manage how and when Intel® VTune™
Profiler collects data for your applications. By calling these APIs, you can
pause, resume, or detach data collection to focus analysis on specific code
regions, reduce profiling overhead, or exclude unimportant sections from your
performance results.


+-------------------------------+----------------------------------------------+
| Use This Primitive            | To Do This                                   |
+===============================+==============================================+
| .. code:: cpp                 | Run the application without collecting data. |
|                               | VTune Profiler reduces the overhead of       |
|    void __itt_pause(void)     | collection by collecting only critical       |
|                               | information, like thread and process         |
|                               | creation.                                    |
+-------------------------------+----------------------------------------------+
| .. code:: cpp                 | Resume data collection.                      |
|                               |                                              |
|    void __itt_resume(void)    |                                              |
+-------------------------------+----------------------------------------------+
| .. code:: cpp                 | Detach data collection. VTune Profiler       |
|                               | detaches all collectors from all processes.  |
|    void __itt_detach(void)    | Your application continues to work but no    |
|                               | data is collected for the running collection.|
+-------------------------------+----------------------------------------------+


Pause data collection
---------------------


When you pause the data collection in any thread, you pause the collection for
the entire program and not just the active thread. Also, pausing a data collection
can reduce the overhead from runtime analysis.


Unaffected APIs:

   -  Domain API
   -  String Handle API
   -  Thread Naming API


Affected APIs (No Data Collection in Paused State):

   -  Task API
   -  Frame API
   -  Event API
   -  User-Defined Synchronization API


.. note::


   The Pause/Resume API call frequency is about 1Hz for a reasonable rate.
   Since this operation pauses and resumes data collection in all processes
   in the analysis run with the corresponding collection state notification
   sent to the GUI, for small workloads, do not call this operation on a
   frequent basis. Use `Frame APIs <frame-api.html>`__ instead.


Usage Example: Focus on a Specific Code Section
-----------------------------------------------


In this code example, the pause/resume calls help to focus data collection
from a specific section of code. The application run begins when the collection
is paused.


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


This example shows how you use pause/resume calls to hide intensive work that
may not need attention for a brief period.


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

