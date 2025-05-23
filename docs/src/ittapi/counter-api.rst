.. _counter-api:

Counter API
===========


A Counter is a user-defined characteristic or metric of hardware or software
behavior that you use to collect information about execution breakdown. You can
also use counters to correlate this information with tasks, events, and markers.

For example, the development of system-on-a-chip (SoC) benefits from several
counters that represent different parts of the SoC to count some hardware
characteristics.


**Define and create a counter object**


Use these primitives:

.. code-block:: cpp


   __itt_counter __itt_counter_create(const char *name, const char *domain);

   __itt_counter __itt_counter_createA(const char *name, const char *domain);

   __itt_counter __itt_counter_createW(const wchar_t *name, const wchar_t *domain);

   __itt_counter __itt_counter_create_typed(const char *name, const char *domain, __itt_metadata_type type);

   __itt_counter __itt_counter_create_typedA(const char *name, const char *domain, __itt_metadata_type type);

   __itt_counter __itt_counter_create_typedW(const wchar_t *name, const wchar_t *domain, __itt_metadata_type type);

   __itt_counter __itt_counter_create_v3(__itt_domain* domain, const char* name,__itt_metadata_type type);


You must specify a counter name and domain name. To load a specialized type of
data, specify the counter type. The default counter type is ``uint64_t``.


**Parameters of the primitives:**


+--------+--------------------------+-------------------+
| Type   | Parameter                | Description       |
+========+==========================+===================+
| [in]   | .. code-block:: cpp      | Counter domain    |
|        |                          |                   |
|        |    domain                |                   |
+--------+--------------------------+-------------------+
| [in]   | .. code-block:: cpp      | Counter name      |
|        |                          |                   |
|        |    name                  |                   |
+--------+--------------------------+-------------------+
| [in]   | .. code-block:: cpp      | Counter type      |
|        |                          |                   |
|        |    type                  |                   |
+--------+--------------------------+-------------------+


**Increment/decrement a counter value**


Use these primitives:


.. code-block:: cpp
  
  
   void __itt_counter_inc (__itt_counter id);
   
   void __itt_counter_inc_delta(__itt_counter id, unsigned long long value);
   
   void __itt_counter_dec(__itt_counter id);
   
   void __itt_counter_dec_delta(__itt_counter id, unsigned long long value);


.. note::


   These primitives are applicable to uint64 counters only.


**Directly set the counter value**


Use:

.. code-block:: cpp
  
  
   void __itt_counter_set_value(__itt_counter id, void *value_ptr);
   
   void __itt_counter_set_value_v3(__itt_counter counter, void *value_ptr);


Parameters of the primitive:


+--------+--------------------------+------------------+
| Type   | Parameter                | Description      |
+========+==========================+==================+
| [in]   | .. code-block:: cpp      | Counter ID       |
|        |                          |                  |
|        |    id                    |                  |
+--------+--------------------------+------------------+
| [in]   | .. code-block:: cpp      | Counter value    |
|        |                          |                  |
|        |    value_ptr             |                  |
+--------+--------------------------+------------------+


**Remove an existing counter**


Use:

.. code-block:: cpp


   void __itt_counter_destroy(__itt_counter id);


Usage Example
-------------


This example creates a counter that measures temperature and memory
usage metrics:


.. code:: cpp


   #include "ittnotify.h"


   __itt_counter temperatureCounter = __itt_counter_create("Temperature", "Domain");
   __itt_counter memoryUsageCounter = __itt_counter_create("Memory Usage", "Domain");
   unsigned __int64 temperature;


   while (...)
   {
       ...
       temperature = getTemperature();
       __itt_counter_set_value(temperatureCounter, &temperature);


       __itt_counter_inc_delta(memoryUsageCounter, getAllocatedMemSize());
       __itt_counter_dec_delta(memoryUsageCounter, getDeallocatedMemSize());
       ...
   }


   __itt_counter_destroy(temperatureCounter);
   __itt_counter_destroy(memoryUsageCounter);

