.. _thread-naming-api:


Thread Naming API
=================


By default, each thread in your application is displayed in the
**timeline** track with a default label generated from the process ID
and the thread ID, or with the OS thread name. You can use the Thread
Naming API in your code to give threads meaningful names.


To set thread name using a char or Unicode string, use the primitive:


``void __itt_thread_set_nameA (const __itt_char *name)``


``void __itt_thread_set_nameW (const wchar_t *name)``


Or use the macro in the following way:


``void__itt_thread_set_name(__TEXT("Main"))``


**Parameters of the primitive:**


.. list-table:: 

   * -     \ [in]\    
     -     \ ``name``\    
     -     The thread name    




If the thread name is set multiple times, only the last name is used.


Usage Example
-------------


You can use the following thread naming example to give a meaningful
name to the thread you wish to focus on and ignore the service thread.


.. code:: cpp


   DWORD WINAPI service_thread(LPVOID lpArg)
   {
     __itt_thread_set_name(__TEXT("My worker thread"));
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

