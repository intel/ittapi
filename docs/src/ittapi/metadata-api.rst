.. _metadata-api:

Metadata API
============


Metadata is additional information or generic data that can be attached to a
task, a thread, a process, etc. Metadata has a type, name, and value.
The value encoding depends on the metadata type. The encoding may contain
either string data or a number of integer or floating point values.


To create metadata, use the following primitives:


.. code-block:: cpp


   void __itt_metadata_add(const __itt_domain *domain, __itt_id id, __itt_string_handle *key,
                           __itt_metadata_type type, size_t count, void *data);

   void __itt_metadata_str_addA(const __itt_domain *domain, __itt_id id, __itt_string_handle *key,
                                const char *data, size_t length);

   void __itt_metadata_str_addW(const __itt_domain *domain, __itt_id id, __itt_string_handle *key,
                                const wchar_t *data, size_t length);
   
   void __itt_metadata_add_with_scope(const __itt_domain *domain, __itt_scope scope,
                                      __itt_string_handle *key, __itt_metadata_type type,
                                      size_t count, void *data);
   
   void __itt_metadata_str_add_with_scopeA(const __itt_domain *domain, __itt_scope scope,
                                        __itt_string_handle *key, const char *data, size_t length);
   
   void __itt_metadata_str_add_with_scopeW(const __itt_domain *domain, __itt_scope scope,
                                        __itt_string_handle *key, const wchar_t *data, size_t length);


The following table defines the parameters used in the Metadata API primitives.


+--------+------------------------------+----------------------------------------------------+
| Type   | Parameter                    | Description                                        |
+========+==============================+====================================================+
| [in]   | .. code-block:: cpp          | Metadata domain                                    |
|        |                              |                                                    |
|        |    __itt_domain* domain      |                                                    |
+--------+------------------------------+----------------------------------------------------+
| [in]   | .. code-block:: cpp          | Metadata scope: task, thread, process, and global. |
|        |                              | If a scope is undefined, metadata belongs to the   |
|        |    __itt_scope scope         | last task in the thread.                           |
+--------+------------------------------+----------------------------------------------------+
| [in]   | .. code-block:: cpp          | Metadata name                                      |
|        |                              |                                                    |
|        |    __itt_string_handle* name |                                                    |
+--------+------------------------------+----------------------------------------------------+
| [in]   | .. code-block:: cpp          | Metadata type; used only for numeric metadata      |
|        |                              |                                                    |
|        |    __itt_metadata_type type  |                                                    |
+--------+------------------------------+----------------------------------------------------+
| [in]   | .. code-block:: cpp          | Number of numeric metadata items                   |
|        |                              | ``[in] size_t`` length                             |
|        |    size_t count              |                                                    |
+--------+------------------------------+----------------------------------------------------+
| [in]   | .. code-block:: cpp          | Number of symbols a metadata string                |
|        |                              |                                                    |
|        |    size_t length             |                                                    |
+--------+------------------------------+----------------------------------------------------+
| [in]   | .. code-block:: cpp          | Actual metadata (array of numerics or string)      |
|        |                              |                                                    |
|        |    void *data                |                                                    |
|        |    const char *data          |                                                    |
|        |    const wchar_t *data       |                                                    |
+--------+------------------------------+----------------------------------------------------+

