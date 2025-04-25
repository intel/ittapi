.. _string-handle-api:

String Handle API
=================


Many API calls require names to identify API objects. String handles are
pointers to names. They enable efficient handling of named objects in
run time and make collected traces data more compact.


**To create and return a handle value that can be associated with a
string, use the following primitive:**


``__itt_string_handle *ITTAPI__itt_string_handle_create ( const char *name)``


Consecutive calls to ``__itt_string_handle_create`` with the same name
return the same value. The set of string handles is expected to remain
static during the application's execution time, therefore, there is no
mechanism to destroy a string handle. Any string handle can be accessed
by any thread in the process, regardless of which thread created the
string handle. This call is thread-safe.


**Parameters of the primitive:**


.. list-table:: 
   :header-rows: 0

   * -     \ ``[in]``\    
     -     \ *name*\    
     -     The input string    



