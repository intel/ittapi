.. _marker-api:

Marker API
==========


A marker is an instant event on a timeline that can be associated with a
particular process, a thread, or specified in a global scope.


**To create a marker, use the following primitive:**

.. code-block:: cpp


   void __itt_marker(const __itt_domain *domain, __itt_id id,
                    __itt_string_handle *name, __itt_scope scope);


**Parameters of the primitive:**


+--------+-----------------------+---------------------------------------------------------+
| Type   | Parameter             | Description                                             |
+========+=======================+=========================================================+
| [in]   | .. code-block:: cpp   | Marker domain                                           |
|        |                       |                                                         |
|        |    domain             |                                                         |
+--------+-----------------------+---------------------------------------------------------+
| [in]   | .. code-block:: cpp   | Marker name                                             |
|        |                       |                                                         |
|        |    name               |                                                         |
+--------+-----------------------+---------------------------------------------------------+
| [in]   | .. code-block:: cpp   | Optional parameter. Marker ID, or ``__itt_null``.       |
|        |                       | Markers with different domains cannot have the same IDs.|
|        |    id                 |                                                         |
+--------+-----------------------+---------------------------------------------------------+
| [in]   | .. code-block:: cpp   | Marker scope: process, thread, and global               |
|        |                       |                                                         |
|        |    scope              |                                                         |
+--------+-----------------------+---------------------------------------------------------+

