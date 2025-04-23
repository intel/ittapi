.. _task-api:

Task API
========


A *task* is a logical unit of work performed by a particular thread.
Tasks can nest; thus, tasks typically correspond to functions, scopes,
or a case block in a switch statement. You can use the Task API to
assign tasks to threads.


Task API is a per-thread function that works in resumed state. This
function does not work in paused state.


The Task API does not enable a thread to suspend the current task and
switch to a different task (task switching), or move a task to a
different thread (task stealing).


A task instance represents a piece of work performed by a particular
thread for a period of time. The task is defined by the bracketing of
\__itt_task_begin() and \__itt_task_end() on the same thread.


.. note::


   To be able to see user tasks in your results, enable the **Analyze
   user tasks** checkbox in analysis settings.


Task API Functions
------------------


Create a task instance on a thread. This becomes the current task
instance for that thread. A call to\ ``__itt_task_end()`` on the same
thread ends the current task instance.


``void            __itt_task_begin (const __itt_domain *domain,__itt_id taskid, __itt_id parentid, __itt_string_handle *name)``


Trace the end of the current task.


``void           __itt_task_end (const __itt_domain *domain)``


``ITTAPI__itt_task_*`` Function Parameters
------------------------------------------


The following table defines the parameters used in the Task API
primitives.


.. list-table:: 
   :header-rows: 0

   * -  Type
     -     Parameter    
     -     Description    
   * -  ``[in]``
     -     \ ``__itt_domain``\    
     -     The domain of the task.    
   * -  ``[in]``
     -     \ ``__itt_id taskid``\    
     -     This is a reserved parameter.    
   * -  ``[in]``
     -     \ ``__itt_id parentid``\    
     -     This is a reserved parameter.    
   * -  ``[in]``
     -     \ ``__itt_string_handle``\    
     -     The task string handle.    




Enable Task APIs
----------------


The following steps are required to begin using task APIs:


#. Include ``ittnotify.h`` header.
#. Create domain and string handles for your tasks.
#. Insert task begin and task end marks in your code.
#. Link to ``libittnotify.lib`` (Windows*) or ``libittnotify.a``
   (Linux*).
#. Enable the **Analyze user tasks, events, and counters** option before
   profiling. For more information, see `Task
   Analysis <task-analysis.html>`__ topic.


Usage Example
-------------


The following code snippet creates a domain and a couple of tasks at
global scope.


.. code:: cpp


   #include "ittnotify.h"


   void do_foo(double seconds);


   __itt_domain* domain = __itt_domain_create("MyTraces.MyDomain");
   __itt_string_handle* shMyTask = __itt_string_handle_create("My Task");
   __itt_string_handle* shMySubtask = __itt_string_handle_create("My SubTask");


   void BeginFrame() {
        __itt_task_begin(domain, __itt_null, __itt_null, shMyTask);
        do_foo(1);
   }


   void DoWork() {
        __itt_task_begin(domain, __itt_null, __itt_null, shMySubtask);
        do_foo(1);
        __itt_task_end(domain);
   }
   void EndFrame() {
        do_foo(1);
        __itt_task_end(domain);
   }


   int main() {
       BeginFrame();
       DoWork();
       EndFrame();
       return 0;
   }


   #ifdef _WIN32
   #include <ctime>


   void do_foo(double seconds) {
       clock_t goal = (clock_t)((double)clock() + seconds * CLOCKS_PER_SEC);
       while (goal > clock());
   }
   #else
   #include <time.h>


   #define NSEC 1000000000
   #define TYPE long


   void do_foo(double sec) {
         struct timespec start_time;
         struct timespec current_time;


         clock_gettime(CLOCK_REALTIME, &start_time);
         while(1) {
             clock_gettime(CLOCK_REALTIME, &current_time);
             TYPE cur_nsec=(long)((current_time.tv_sec-start_time.tv_sec-sec)*NSEC + current_time.tv_nsec - start_time.tv_nsec);
             if(cur_nsec>=0)
                   break;
         }
   }
   #endif

