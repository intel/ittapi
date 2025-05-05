.. _marker-api:

Marker API
==========


Marker is an instant event on a timeline that can be associated with a
particular process, a thread, or specified in a global scope.


**To create a marker, use the following primitive:**


``void __itt_marker(const __itt_domain *domain, __itt_id id, __itt_string_handle *name, __itt_scope scope)``


**Parameters of the primitive:**


.. list-table:: 
   :header-rows: 0

   * -     [in]    
     -     \ ``domain``\    
     -     Marker domain    
   * -     [in]    
     -     \ ``name``\    
     -     Marker name    
   * -     [in]    
     -     \ ``id``\    
     -     Optional parameter. Marker ID, or ``__itt_nullNOTEMarkers`` with different domains cannot have same IDs.    
   * -     [in]    
     -     \ ``scope``\    
     -     Marker scope: process, thread, and global    



