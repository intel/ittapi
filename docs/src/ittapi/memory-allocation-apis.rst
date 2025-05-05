.. _memory-allocation-apis:

Memory Allocation APIs
======================


Intel® VTune™ Profiler provides a set of APIs to help it identify the
semantics of your ``malloc``-like heap management functions.


Annotating your code with these APIs allows VTune Profiler to correctly
determine memory objects as part of `Memory Access
Analysis <memory-access-analysis-for-cache-misses-and-high-bandwidth-issues.html>`__.


Usage Tips
----------


Follow these guidelines when using the memory allocation APIs:


-  Create *wrapper* functions for your routines, and put the
   ``__itt_heap_*_begin`` and ``__itt_heap_*_end`` calls in these
   functions.


-  Allocate a unique domain for each pair of allocate/free functions
   when calling ``__itt_heap_function_create``. This allows the VTune
   Profiler to verify a matching free function is called for every
   allocate function call.


-  Annotate the beginning and end of every allocate function and free
   function.


-  Call all function pairs from the same stack frame, otherwise the
   VTune Profiler assumes an exception occurred and the allocation
   attempt failed.


-  Do not call an end function without first calling the matching begin
   function.


Using Memory Allocation APIs in Your Code
-----------------------------------------


.. list-table:: 
   :header-rows: 0

   * -     Use This    
     -     To Do This    
   * - 
     -     Declare a handle type to match begin and end calls and domains.      
   * - 
     -     Identify allocation functions.
   * - 
     -     Identify deallocation functions.
   * - 
     -     Identify reallocation functions.       Note that ``itt_heap_reallocate_end()`` must be called after the attempt even if no memory is returned. VTune Profiler assumes C-runtime ``realloc`` semantics.




Usage Example: Heap Allocation
------------------------------


.. code:: cpp


   #include <ittnotify.h>


   void* user_defined_malloc(size_t size);
   void user_defined_free(void *p);
   void* user_defined_realloc(void *p, size_t s);


   __itt_heap_function my_allocator;
   __itt_heap_function my_reallocator;
   __itt_heap_function my_freer;


   void* my_malloc(size_t s)
   {
       void* p;


       __itt_heap_allocate_begin(my_allocator, s, 0);
       p = user_defined_malloc(s);
       __itt_heap_allocate_end(my_allocator, &p, s, 0);


       return p;
   }




   void my_free(void *p)
   {
       __itt_heap_free_begin (my_freer, p);
       user_defined_free(p);
       __itt_heap_free_end (my_freer, p);
   }


   void* my_realloc(void *p, size_t s)
   {
       void *np;


       __itt_heap_reallocate_begin (my_reallocator, p, s, 0);
       np = user_defined_realloc(p, s);
       __itt_heap_reallocate_end(my_reallocator, p, &np, s, 0);


       return(np);
   }


   // Make sure to call this init routine before any calls to
   // user defined allocators.
   void init_itt_calls()
   {
       my_allocator = __itt_heap_function_create("my_malloc", "mydomain");
       my_reallocator = __itt_heap_function_create("my_realloc", "mydomain");
       my_freer = __itt_heap_function_create("my_free", "mydomain");
   }

