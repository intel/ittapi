.. _minimize-itt-api-overhead:

Minimize ITT API Overhead
=========================


The extent of instrumentation you add to your application determines the
amount of overhead introduced by the ITT API and its impact on application
performance. To minimize this overhead, aim for a balance between desired
application performance and the amount of performance data you want to collect.

Use these guidelines:

-  Add instrumentation to only those paths in your application that are
   important for analysis.
-  Create ITT domains and string handles outside the critical paths.
-  Filter data collection by different aspects of your application that
   can be analyzed separately. The overhead for a disabled API call
   (thus filtering out the associated call) is always less than 10 clock
   ticks.


Conditional Compilation
-----------------------


In the release version of your code, use conditional compilation to turn off
annotations. Before you include ``ittnotify.h`` during compilation, define the
macro ``INTEL_NO_ITTNOTIFY_API`` to eliminate all ``__itt_*`` functions from
your code.

By defining this macro, you can also remove the static library from the
linking stage.


Usage Example:
--------------


The ITT APIs include a subset of functions that create domains and string
handles. These functions always return identical handles for the same domain
names and strings. This action requires the subset of functions to perform
string comparisons and table lookups. These comparisons and lookups can incur
serious performance penalties. Additionally, the performance of these functions
is proportional to the log of the number of created domains or string handles.
A good practice is to create domains and string handles in the global scope,
or during application startup.

The following code section creates two domains in the global scope. You can use
these domains to control the level of detail that is written to the trace file.


.. code-block:: cpp


     #include "ittnotify.h"

     // Create domains at global scope.
     __itt_domain* basic = __itt_domain_create(L"MyFunction.Basic");
     __itt_domain* detailed = __itt_domain_create(L"MyFunction.Detailed");

     // Create string handles at global scope.
     __itt_string_handle* h_my_funcion = __itt_string_handle_create(L"MyFunction");
     void MyFunction(int arg)
     {
         __itt_task_begin(basic, __itt_null, __itt_null, h_my_function);
         Foo(arg);
         FooEx();
         __itt_task_end(basic);
     }

     __itt_string_handle* h_foo = __itt_string_handle_create(L"Foo");
     void Foo(int arg)
     {
         // Skip tracing detailed data if the detailed domain is disabled.
         __itt_task_begin(detailed, __itt_null, __itt_null, h_foo);
         // Do some work here...
         __itt_task_end(detailed);
     }

     __itt_string_handle* h_foo_ex = __itt_string_handle_create(L"FooEx");
     void FooEx()
     {
         // Skip tracing detailed data if the detailed domain is disabled.
         __itt_task_begin(detailed, __itt_null, __itt_null, h_foo_ex);
         // Do some work here...
         __itt_task_end(detailed);
     }

     // This is my entry point.
     int main(int argc, char** argv)
     {
       if(argc < 2)
       {
         // Disable detailed domain if we do not need tracing from that
         // in this application run.
         detailed ->flags = 0;
       }

       MyFunction(atoi(argv[1]));
       return 0;
     }

