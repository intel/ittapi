.. _metadata-api:

Metadata API
============


Metadata is extra information, generic data which can be attached to a
task, a thread, a process, etc. Metadata has a type, name, and value.
The value encoding depends on a metadata type. It may contain either
string data or a number of integer or floating point values.


To create metadata, use the following primitives:


``void __itt_metadata_add(const __itt_domain *domain, __itt_id id, __itt_string_handle *key, __itt_metadata_type type, size_t count, void *data)``


``void __itt_metadata_str_addA(const __itt_domain *domain, __itt_id id, __itt_string_handle *key, const char *data, size_t length)``


``void __itt_metadata_str_addW(const __itt_domain *domain, __itt_id id, __itt_string_handle *key, const wchar_t *data, size_t length)``


``void __itt_metadata_add_with_scope(const __itt_domain *domain, __itt_scope scope, __itt_string_handle *key, __itt_metadata_type type, size_t count, void *data)``


``void __itt_metadata_str_add_with_scopeA(const __itt_domain *domain, __itt_scope scope, __itt_string_handle *key, const char *data, size_t length)``


``void __itt_metadata_str_add_with_scopeW(const __itt_domain *domain, __itt_scope scope, __itt_string_handle *key, const wchar_t *data, size_t length)``


The following table defines the parameters used in the Metadata API
primitives.


.. list-table:: 
   :header-rows: 1

   * -  Type
     -  Parameter
     -  Description
   * -  [in]
     -  ``__itt_domain* domain``
     -  Metadata domain
   * -  [in]
     -  ``__itt_scope scope``
     -  Metadata scope: task, thread, process, and global. If a scope is undefined, metadata belongs to the last task in the thread.
   * -  [in]
     -  ``__itt_string_handle* name``
     -  Metadata name
   * -  [in]
     -  ``__itt_metadata_type type``
     -  Metadata type; used only for numeric metadata
   * -  [in]
     -  ``size_t count``
     -  Number of numeric metadata items ``[in] size_t``\ length
   * -  [in]
     -  ``size_t length``
     -      Number of symbols a metadata string    
   * -  [in]
     -  ``void *data``
        ``const char *data``
        ``const wchar_t *data``
     -  Actual metadata (array of numerics or string)



