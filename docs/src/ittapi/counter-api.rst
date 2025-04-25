.. _counter-api:

Counter API
===========


A Counter is a user-defined characteristic or metric of hardware or
software behavior that can be used to collect information about and
correlate with execution breakdown, such as tasks, events, and markers.
For example, system on a chip (SoC) development benefits from several
counters representing different parts of the SoC to count some hardware
characteristics.


**Define and create a counter object**


Use these primitives:


``__itt_counter``


``__itt_counter_create(const char *name, const char *domain);``


``__itt_counter_createA(const char *name, const char *domain);``


``__itt_counter_createW(const wchar_t *name, const wchar_t *domain);``


``__itt_counter_create_typed (const char *name, const char *domain, __itt_metadata_type type);``


``__itt_counter  __itt_counter_create_typedA __itt_counter_create_typedA(const char *name, const char *domain, __itt_metadata_type type)``


``__itt_counter  __itt_counter_create_typedW __itt_counter_create_typedW(const wchar_t *name, const wchar_t *domain, __itt_metadata_type type)``


``__itt_counter_create_v3(__itt_domain* domain, const char* name,__itt_metadata_type type);``


A counter name and domain name should be specified. To load a
specialized type of data, specify the counter type. By default the
unsigned int64 type is used.


**Parameters of the primitives:**


.. list-table:: 
   :header-rows: 0

   * -     [in]    
     -     \ ``domain``\    
     -     Counter domain    
   * -     [in]    
     -     \ ``name``\    
     -     Counter name    
   * -     [in]    
     -     \ ``type``\    
     -     Counter type    




**Increment/decrement a counter value**


Use these primitives:


``__itt_counter_inc (__itt_counter id);``


``__itt_counter_inc_delta(__itt_counter id, unsigned long long value);``


``__itt_counter_dec(__itt_counter id);``


``__itt_counter_dec_delta(__itt_counter id, unsigned long long value);``


.. note::


   These primitives are applicable to uint64 counters only.


**Directly set the counter value**


Use:


``__itt_counter_set_value(__itt_counter id, void *value_ptr);``


``__itt_counter_set_value_v3(__itt_counter counter, void *value_ptr);``


Parameters of the primitive:


.. list-table:: 
   :header-rows: 0

   * -     [in]    
     -     \ ``id``\    
     -     Counter ID    
   * -     [in]    
     -     \ ``value_ptr``\    
     -     Counter value    




**Remove an existing counter**


Use:


``__itt_counter_destroy(__itt_counter id);``


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

