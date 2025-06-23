.. _thread-naming-api:

Thread Naming API
=================


By default, each thread in your application displays in the **timeline** track.
The threads use a default label that uses the OS thread name or gets generated
from the process ID and the thread ID. To give meaningful names to your
threads, use the Thread Naming API.

he Thread Naming API is a per-thread function that works in all states
(paused or resumed). You must call this API from within the thread.

To set thread name using a char or Unicode string, use the primitive:

.. code-block:: cpp


   void __itt_thread_set_name (const __itt_char *name);


**Parameters of the primitive:**


+--------+------------------------+---------------------+
| Type   | Parameter              | Description         |
+========+========================+=====================+
| [in]   | .. code-block:: cpp    | The thread name     |
|        |                        |                     |
|        |    name                |                     |
+--------+------------------------+---------------------+


**To indicate that this thread should be ignored from analysis:**

.. code-block:: cpp


   void __itt_thread_ignore (void);


Calling ``__itt_thread_ignore()`` does not affect the application's
concurrency. After this call, the current thread will not be visible in the
**Timeline** pane.


If the thread name is set multiple times, only the last name is used.


Usage Example
-------------


This example shows how you set a meaningful name to a specific thread
and ignore the service thread.

.. code-block:: cpp


   DWORD WINAPI service_thread(LPVOID lpArg)
   {
     __itt_thread_ignore();
     // Do service work here. This thread will not be displayed.
     return 0;
   }


   DWORD WINAPI thread_function(LPVOID lpArg)
   {
     __itt_thread_set_name("My worker thread");
     // Do thread work here
     return 0;
   }


   int main(int argc, char* argv[])
   {
     CreateThread(NULL, 0, service_thread, NULL, 0, NULL);
     CreateThread(NULL, 0, thread_function, NULL, 0, NULL);
     
     return 0;
   }

