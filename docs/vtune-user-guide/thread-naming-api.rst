.. _thread-naming-api:

Thread Naming API
=================


By default, each thread in your application displays in the **timeline**
track with a default label generated from the process ID and the thread
ID, or with the OS thread name. You can use the Thread Naming API in
your code to give meaningful names to threads.


Thread Naming API is a per-thread function that works in all states
(paused or resumed). You must call this API from within the thread.


To set thread name using a char or Unicode string, use the primitive:


``void         __itt_thread_set_name (const __itt_char *name)``


**Parameters of the primitive:**


.. list-table:: 
   :header-rows: 0

   * -     \ ``[in]``\    
     -     \ ``name``\    
     -     The thread name    




**To indicate that this thread should be ignored from analysis:**


``void         __itt_thread_ignore (void)``


It does not affect the concurrency of the application. It does not be
visible in the **Timeline** pane.


If the thread name is set multiple times, only the last name is used.


Usage Example
-------------


You can use the following thread naming example to give a meaningful
name to the thread you wish to focus on and ignore the service thread.


.. code:: cpp


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
     ...
     CreateThread(NULL, 0, service_thread, NULL, 0, NULL);
     CreateThread(NULL, 0, thread_function, NULL, 0, NULL);
     ...
     return 0;
   }

