.. _minimize-itt-api-overhead:

Minimize ITT API Overhead
=========================


The ITT API overhead and its impact on the overall application
performance depends on the amount of instrumentation code added to the
application. When instrumenting an application with ITT API, you should
balance between application performance and the amount of performance
data that you need to collect, in order to minimize API overhead while
collecting sufficient performance data.


Follow these guidelines to achieve good balance between overall
performance of the instrumented application and instrumentation detail:


-  Instrument only those paths within your application that are
   important for analysis.
-  Create ITT domains and string handles outside the critical paths.
-  Filter data collection by different aspects of your application that
   can be analyzed separately. The overhead for a disabled API call
   (thus filtering out the associated call) is always less than 10 clock
   ticks, regardless of the API.


Conditional Compilation
-----------------------


For best performance in the release version of your code, use
conditional compilation to turn off annotations. Define the macro
INTEL_NO_ITTNOTIFY_API before you include ittnotify.h during compilation
to eliminate all \__itt_\* functions from your code.


You can also remove the static library from the linking stage by
defining this macro.


Usage Example: Using Domains and String Handles
-----------------------------------------------


The ITT APIs include a subset of functions which create domains and
string handles. These functions always return identical handles for the
same domain names and strings. This requires these functions to perform
string comparisons and table lookups, which can incur serious
performance penalties. In addition, the performance of these functions
is proportional to the log of the number of created domains or string
handles. It is best to create domains and string handles at global
scope, or during application startup.


The following code section creates two domains in the global scope. You
can use these domains to control the level of detail that is written to
the trace file.


.. code:: cpp


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
           //Disable detailed domain if we do not need tracing from that in this
           //application run 
   detailed ->flags = 0; 
         MyFunction(atoi(argv[1])); }

