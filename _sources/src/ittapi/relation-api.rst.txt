.. _relation-api:

Relation API
============


The Relation API binds two named instances, like tasks, with a
reasonable relation attribute. You can add relations before or after
the actual instances are created. These relations exist independently
outside the instances.

To group a bunch of tasks logically, you can use different types of relations:


.. code-block:: cpp


   void ITTAPI __itt_relation_add(const __itt_domain *domain, __itt_id head,
                                  __itt_relation relation, __itt_id tail);

   void ITTAPI __itt_relation_add_ex(const __itt_domain *domain, __itt_clock_domain* clock_domain,
                                     unsigned long long timestamp, __itt_id head,
                                     __itt_relation relation, __itt_id tail);


**Parameters of the primitives:**


+--------+-------------------------------------+---------------------------------------+
| Type   | Parameter                           | Description                           |
+========+=====================================+=======================================+
| [in]   | .. code-block:: cpp                 | Relation domain                       |
|        |                                     |                                       |
|        |    __itt_domain* domain             |                                       |
+--------+-------------------------------------+---------------------------------------+
| [in]   | .. code-block:: cpp                 | User-defined logical relation between |
|        |                                     | two named instances                   |
|        |    __itt_relation relation          |                                       |
+--------+-------------------------------------+---------------------------------------+
| [in]   | .. code-block:: cpp                 | Metadata name                         |
|        |                                     |                                       |
|        |    __itt_id head                    |                                       |
+--------+-------------------------------------+---------------------------------------+
| [in]   | .. code-block:: cpp                 | IDs of two named related instances    |
|        |                                     | ``size_t`` count                      |
|        |    __itt_id tail                    |                                       |
+--------+-------------------------------------+---------------------------------------+
| [in]   | .. code-block:: cpp                 | User-defined clock domain             |
|        |                                     |                                       |
|        |    __itt_clock_domain* clock_domain |                                       |
+--------+-------------------------------------+---------------------------------------+
| [in]   | .. code-block:: cpp                 | User-defined timestamp for the        |
|        |                                     | corresponding clock domain            |
|        |    unsigned long long timestamp     |                                       |
+--------+-------------------------------------+---------------------------------------+

