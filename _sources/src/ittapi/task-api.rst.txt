.. _task-api:

Task API
========


A task is a logical unit of work that is performed by a particular thread.
Tasks can nest; thus, tasks typically correspond to functions, scopes,
or a case block in a switch statement.

Use the Task API to assign tasks to threads

The Task API does not enable a thread to perform:

-  Task switching, where a thread suspends the current task and switches to
   a different task.
-  Task stealing, where a thread moves a task to a different thread.


A task instance represents a piece of work performed by a particular
thread for a period of time. The task is defined by the bracketing of
``__itt_task_begin()`` and ``__itt_task_end()`` on the same thread.


Tasks can be simple or overlapped.


Simple tasks implicitly support the concept of embedded execution. The call
``__itt_task_end()`` finalizes the most recent ``__itt_task_begin()`` call.
For example, the following metacode is a valid sequence, and the execution time
of "a" tasks incorporates the execution time of "b" tasks:


.. code-block:: cpp


   __itt_task_begin(a);
   __itt_task_begin(b);
   __itt_task_end(b);
   __itt_task_end(a);


The execution regions of overlapped tasks may intercept. For example, the
following metacode is a valid sequence. A "b" task that started after an
"a" task can finish upon completion of the "a" task:


.. code-block:: cpp


    __itt_task_begin_overlapped(a);
    __itt_task_begin_overlapped(b);
    __itt_task_end_overlapped(a);
    __itt_task_end_overlapped(b);


Task API Functions
------------------


**To create a simple task instance on a thread, use the following functions:**


.. code-block:: cpp
  
  
   void ITTAPI __itt_task_begin(const __itt_domain *domain, __itt_id taskid,
                                __itt_id parentid, __itt_string_handle *name);

   void ITTAPI __itt_task_begin_fn (const __itt_domain *domain,__itt_id taskid,
                                    __itt_id parentid, void* address);

   void ITTAPI __itt_task_end (const __itt_domain *domain);


**To create a simple task instance in a different clock domain, use the
following functions:**

.. code-block:: cpp


   void ITTAPI __itt_task_begin_ex(const __itt_domain* domain, __itt_clock_domain* clock_domain,
                                  unsigned long long timestamp, __itt_id taskid, __itt_id parentid,
                                  __itt_string_handle* name);

   void ITTAPI __itt_task_begin_fn_ex(const __itt_domain* domain, __itt_clock_domain* clock_domain,
                                    unsigned long long timestamp, __itt_id taskid,
                                    __itt_id parentid, void* fn);

   void ITTAPI _itt_task_end_ex(const __itt_domain* domain, __itt_clock_domain* clock_domain,
                                unsigned long long timestamp);


**To create an overlapped task instance on a thread, use the following
functions:**


.. code-block:: cpp


   void ITTAPI __itt_task_begin_overlapped(const __itt_domain* domain, __itt_id taskid,
                                           __itt_id parentid, __itt_string_handle* name);

   void ITTAPI __itt_task_end_overlapped(const __itt_domain *domain, __itt_id taskid);


The argument ``taskid`` is mandatory for these functions.


**To create an overlapped task instance in a different clock domain, use
the following functions:**


.. code-block:: cpp


   void ITTAPI __itt_task_begin_overlapped_ex(const __itt_domain* domain, __itt_clock_domain* clock_domain,
                                              unsigned long long timestamp, __itt_id taskid,
                                              __itt_id parentid, __itt_string_handle* name);

   void ITTAPI __itt_task_end_overlapped_ex(const __itt_domain* domain, __itt_clock_domain* clock_domain,
                                          unsigned long long timestamp, __itt_id taskid);


The argument ``taskid`` is mandatory for these functions.


.. _task-api-IJIT_NOTIFYEVENT_FUNCTION:


ITTAPI__itt_task_* Function Parameters
------------------------------------------


The following table defines the parameters used in the Task API
primitives.


+--------+---------------------------------+---------------------------------------------------------------------+
| Type   | Parameter                       | Description                                                         |
+========+=================================+=====================================================================+
| [in]   | .. code-block:: cpp             | The domain of the task.                                             |
|        |                                 |                                                                     |
|        |    __itt_domain                 |                                                                     |
+--------+---------------------------------+---------------------------------------------------------------------+
| [in]   | .. code-block:: cpp             | User-defined ID optional for all task instances,                    |
|        |                                 | except for overlapped task instances.                               |
|        |    __itt_id taskid              | ``__itt_null`` can be used as default for undefined task instances. |
|        |                                 | Task ID is used to define relations between task instances.         |
+--------+---------------------------------+---------------------------------------------------------------------+
| [in]   | .. code-block:: cpp             | Optional parameter. Parent instance ID, to which                    |
|        |                                 | the task belongs, or ``__itt_null``.                                |
|        |    __itt_id parentid            |                                                                     |
+--------+---------------------------------+---------------------------------------------------------------------+
| [in]   | .. code-block:: cpp             | The task string handle.                                             |
|        |                                 |                                                                     |
|        |    __itt_string_handle          |                                                                     |
+--------+---------------------------------+---------------------------------------------------------------------+
| [in]   | .. code-block:: cpp             | Function address that can be used instead of the name.              |
|        |                                 | For example, the function address can be resolved                   |
|        |    void* fn                     | into the function name by using debug symbol information.           |
+--------+---------------------------------+---------------------------------------------------------------------+
| [in]   | .. code-block:: cpp             | User-defined clock domain.                                          |
|        |                                 |                                                                     |
|        |    __itt_clock_domain           |                                                                     |
+--------+---------------------------------+---------------------------------------------------------------------+
| [in]   | .. code-block:: cpp             | User-defined timestamp for the corresponding clock domain.          |
|        |                                 |                                                                     |
|        |    unsigned long long timestamp |                                                                     |
+--------+---------------------------------+---------------------------------------------------------------------+


Usage Example
-------------


The following code snippet creates a domain and a couple of tasks at
global scope.


.. code-block:: cpp


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
             TYPE cur_nsec=(long)((current_time.tv_sec-start_time.tv_sec-sec)*NSEC +
                           current_time.tv_nsec - start_time.tv_nsec);
             if(cur_nsec>=0)
                   break;
         }
   }
   #endif

