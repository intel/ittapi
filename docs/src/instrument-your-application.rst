.. _instrument-your-application:

Instrument Your Application
===========================


When you collect performance data with the ITT/JIT APIs, for optimum results,
add API calls in your code to designate logical tasks. You can then visualize
the relationship between tasks in your code (for example, when they start and
end) relative to other CPU and GPU tasks.


At the highest level, a task is a logical group of work that executes on a
specific thread. A task can correspond to any grouping of code within your
program that you consider important. You can mark up your code by
identifying the beginning and end of each logical task with
``__itt_task_begin`` and ``__itt_task_end`` calls.


To get started, use the following API calls:

-  ``__itt_domain_create()`` creates a domain that is required in most ITT
   API calls. Define atleast one domain.
-  ``__itt_string_handle_create()`` creates string handles to identify your
   tasks. String handles are more efficient than strings to identify traces.
-  ``__itt_task_begin()`` marks the beginning of a task.
-  ``__itt_task_end()`` marks the end of a task.


Example
-------


This example shows how you use four basic ITT API functions in a
multi-threaded application:


-  `Domain API <domain-api.html>`__
-  `String Handle API <string-handle-api.html>`__
-  `Task API <task-api.html>`__
-  `Thread Naming API <thread-naming-api.html>`__


.. code-block:: cpp


   #include <windows.h>
   #include <ittnotify.h>


   // Forward declaration of a thread function.
   DWORD WINAPI workerthread(LPVOID);
   bool g_done = false;

   // Create a domain that is visible globally: we will use it in our example.
   __itt_domain* domain = __itt_domain_create("Example.Domain.Global");
   // Create string handles which associates with the "main" task.
   __itt_string_handle* handle_main = __itt_string_handle_create("main");
   __itt_string_handle* handle_createthread = __itt_string_handle_create("CreateThread");

   void main(int, char* argv[])
   {
     // Create a task associated with the "main" routine.
     __itt_task_begin(domain, __itt_null, __itt_null, handle_main);

     // Now we'll create 4 worker threads
     for (int i = 0; i < 4; i++)
     {
       // We might be curious about the cost of CreateThread. We add tracing to do the measurement.
       __itt_task_begin(domain, __itt_null, __itt_null, handle_createthread);
       cppCreateThread(NULL, 0, workerthread, (LPVOID)i, 0, NULL);
       __itt_task_end(domain);
     }

     // Wait a while,...
     cppSleep(5000);
     g_done = true;

     // Mark the end of the main task
     __itt_task_end(domain);
   }

   // Create string handle for the work task.
   __itt_string_handle* handle_work = __itt_string_handle_create("work");
   DWORD WINAPI workerthread(LPVOID data)
   {
     // Set the name of this thread so it shows up in the UI as something meaningful
     char threadname[32];
     wsprintf(threadname, "Worker Thread %d", data);
     __itt_thread_set_name(threadname);

     // Each worker thread does some number of "work" tasks
     while(!g_done)
     {
       __itt_task_begin(domain, __itt_null, __itt_null, handle_work);
       cppSleep(150);
       __itt_task_end(domain);
     }

     return 0;
   }

