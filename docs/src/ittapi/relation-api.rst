.. _relation-api:

Relation API
============


Relation API binds two named instances, for example, tasks, with a
reasonable relation attribute. Relations can be added before or after
the actual instances are created and persist independently of the
instances.


To logically group a bunch of tasks, you can use different types of
relations:


``void ITTAPI __itt_relation_add(const __itt_domain *domain, __itt_id head, __itt_relation relation, __itt_id tail)``


``void ITTAPI __itt_relation_add_ex(const __itt_domain *domain, __itt_clock_domain* clock_domain, unsigned long long timestamp, __itt_id head, __itt_relation relation, __itt_id tail)``


**Parameters of the primitives:**


.. list-table:: 
   :header-rows: 1

   * -  Type
     -  Parameter
     -  Description
   * -  [in]
     -  ``__itt_domain* domain``
     -     Relation domain    
   * -  [in]
     -  ``__itt_relation relation``
     -     User-defined logical relation between two named instances    
   * -  [in]
     -  ``__itt_id head``
     -     Metadata name    
   * -  [in]
     -  ``__itt_id tail``
     -     IDs of two named related instances ``size_t`` count    
   * -  [in]
     -  ``__itt_clock_domain* clock_domain``
     -     User-defined clock domain    
   * -  [in]
     -  ``unsigned long long timestamp``
     -      User-defined timestamp for the corresponding clock domain    



