.. _string-handle-api:

String Handle API
=================


Many API calls require names to identify API objects. String handles are
pointers to these names. String handles enable efficient handling of named
objects during runtime. The handles also make the collected trace data more
compact.


**To create and return a handle value that can be associated with a
string, use the following primitive:**


.. code-block:: cpp


   __itt_string_handle* __itt_string_handle_create(const char *name);


Consecutive calls to ``__itt_string_handle_create`` with the same name return
the same value. The set of string handles is expected to remain static during
the execution time of the application. Therefore, there is no mechanism to
destroy a string handle. Any thread in the process can access any string handle,
irrespective of the thread that created the string handle. This call is thread-safe.


**Parameters of the primitive:**


+--------+------------------------+-------------------+
| Type   | Parameter              | Description       |
+========+========================+===================+
| [in]   | .. code-block:: cpp    | The input string  |
|        |                        |                   |
|        |    name                |                   |
+--------+------------------------+-------------------+

