.. _memory-allocation-apis:

Memory Allocation APIs
======================


Intel® VTune™ Profiler contains a set of APIs that help identify the
semantics of your ``malloc``-like heap management functions.

Annotating your code with these APIs enables VTune Profiler to correctly
determine memory objects as part of the **Memory Access Analysis**.


Usage Guidelines
----------------


When using the Memory Allocation APIs, follow these guidelines:


-  Create *wrapper* functions for your routines. Put ``__itt_heap_*_begin``
   and ``__itt_heap_*_end`` calls in these functions.
-  When your application calls ``__itt_heap_function_create``, allocate a
   unique domain for each pair of allocate/free functions. This enables
   VTune Profiler to verify that a matching free function gets called for
   every allocate function call.
-  Annotate the beginning and end of every allocate function and free
   function.
-  Call all function pairs from the same stack frame. Otherwise,
   VTune Profiler assumes that an exception occurred, and that the allocation
   attempt failed.
-  Do not call an end function without calling the matching begin
   function first.


Using Memory Allocation APIs in Your Code
-----------------------------------------


+----------------------------------+-----------------------------------------------------------------+
| Use This                         | To Do This                                                      |
+==================================+=================================================================+
| .. code-block:: cpp              | Declare a handle type to match begin and end calls and domains. |
|                                  |                                                                 |
|    typedef void*                 | name = Name of the function you want to annotate.               |
|    __itt_heap_function;          | domain = String identifying a matching set of functions.        |
|                                  |                                                                 |
|    __itt_heap_function           | For example, if there are three functions that all work with    |
|    __itt_heap_function_create(   | my_struct, such as alloc_my_structs, free_my_structs, and       |
|        const __itt_char* <name>, | realloc_my_structs, pass the same domain to all three           |
|        const __itt_char* <domain>| __itt_heap_function_create() calls.                             |
|    );                            |                                                                 |
+----------------------------------+-----------------------------------------------------------------+
| .. code-block:: cpp              | Identify allocation functions.                                  |
|                                  |                                                                 |
|    void                          | h = Handle returned when this function's name was passed        |
|    __itt_heap_allocate_begin(    | to __itt_heap_function_create().                                |
|        __itt_heap_function <h>,  |                                                                 |
|        size_t <size>,            | size = Size in bytes of the requested memory region.            |
|        int <initialized>         |                                                                 |
|    );                            | initialized = Flag indicating if the memory region will be      |
|                                  | initialized by this routine.                                    |
|    void                          |                                                                 |
|    __itt_heap_allocate_end(      | addr = Pointer to the address of the memory region this         |
|        __itt_heap_function <h>,  | function has allocated, or 0 if the allocation failed.          |
|        void**,                   |                                                                 |
|        size_t <size>,            |                                                                 |
|        int <initialized>         |                                                                 |
|    );                            |                                                                 |
+----------------------------------+-----------------------------------------------------------------+
| .. code-block:: cpp              | Identify deallocation functions.                                |
|                                  |                                                                 |
|    void                          | h = Handle returned when this function's name was passed        |
|    __itt_heap_free_begin(        | to __itt_heap_function_create().                                |
|        __itt_heap_function <h>,  |                                                                 |
|        void*                     | addr = Pointer to the address of the memory region this         |
|    );                            | function is deallocating.                                       |
|                                  |                                                                 |
|    void                          |                                                                 |
|    __itt_heap_free_end(          |                                                                 |
|        __itt_heap_function <h>,  |                                                                 |
|        void*                     |                                                                 |
|    );                            |                                                                 |
+----------------------------------+-----------------------------------------------------------------+
| .. code-block:: cpp              | Identify reallocation functions.                                |
|                                  |                                                                 |
|    void                          | Note that itt_heap_reallocate_end() must be called after        |
|    __itt_heap_reallocate_begin(  | the attempt even if no memory is returned. VTune Profiler       |
|        __itt_heap_function <h>,  | assumes C-runtime realloc semantics.                            |
|        void*,                    |                                                                 |
|        size_t <new_size>,        | h = Handle returned when this function's name was passed        |
|        int <initialized>         | to __itt_heap_function_create().                                |
|    );                            |                                                                 |
|                                  | addr = Pointer to the address of the memory region this         |
|    void                          | function is reallocating. If addr is NULL, the VTune Profiler   |
|    __itt_heap_reallocate_end(    | treats this as if it is an allocation.                          |
|        __itt_heap_function <h>,  |                                                                 |
|        void*,                    | new_addr = Pointer to a pointer to hold the address of the      |
|        void** <new_addr>,        | reallocated memory region.                                      |
|        size_t <new_size>,        |                                                                 |
|        int <initialized>         | size = Size in bytes of the requested memory region. If         |
|    );                            | new_size is 0, the VTune Profiler treats this as if it is       |
|                                  | a deallocation.                                                 |
+----------------------------------+-----------------------------------------------------------------+


Usage Example: Heap Allocation
------------------------------


.. code-block:: cpp


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

