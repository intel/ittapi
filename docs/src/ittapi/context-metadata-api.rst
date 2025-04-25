.. _context-metadata-api:

Context Metadata API
====================


Use context metadata to collect counter-based metrics and attribute them
to hardware topology like:


-  PCIe devices
-  Block devices
-  CPU cores
-  Threads


With the Context Metadata API, you can define custom counters in your
code with special attributes. You can also get a set of metrics for the
collected data in any classical form of data representation in Intel®
VTune™ Profiler.


**Availability:**


By default, the Context Metadata API for data collection and
visualization is available in the `Input and Output
analysis <input-and-output-analysis.html>`__ only.


To see this data when running other analysis types,


#. `Create a custom
   analysis <custom-analysis.html>`__ based on
   the predefined analysis type of your interest.
#. In `custom analysis
   options <custom-analysis-options.html>`__, enable
   the **Analyze all ITT API user data** checkbox.


**Define and create a counter object**


Use this structure to store context metadata:


.. code-block:: cpp


   __itt_context_metadata
   {
       __itt_context_type type;    /*!< Type of the context metadata value */
       void* value;                /*!< Pointer to context metadata value itself */
   }


The structure accepts the following types of context metadata:


.. list-table:: 
   :header-rows: 0

   * -      \__itt_context_type    
     -     Value    
     -     Description    
   * -  \__itt_context_name
     -  ASCII string char*/ Unicode string wchar_t\* type
     -  The name of the counter-based metric. This value is required.
   * -  \__itt_context_device
     -  ASCII string char*/ Unicode string wchar_t\* type
     -  Statistics subdomain to break down the counter samples (for example, network port ID, disk partition, etc.)
   * -  \__itt_context_units
     -  ASCII string char*/ Unicode string wchar_t\* type
     -  Units of measurement. For measurement of time, use the ns/us/ms/s units to correct data representation in VTune Profiler.
   * -  \__itt_context_pci_addr
     -  ASCII string char*/ Unicode string wchar_t\* type
     -  PCI address of device to associate with the counter.
   * -  \__itt_context_tid
     -  Unsigned 64-bit integer type
     -  Thread ID to associate with the counter.
   * -  \__itt_context_bandwidth_flag
     -  Unsigned 64-bit integer type (0,1)
     -  If this flag is set to 1, calculate latency histogram and counter/sec timeline distribution.
   * -  \__itt_context_latency_flag
     -  Unsigned 64-bit integer type (0,1)
     -  If this flag is set to 1,calculate the throughput histogram and counter/sec timeline distribution.
   * -  \__itt_context_on_thread_flag
     -  Unsigned 64-bit integer type (0,1)
     -  If this flag is set to 1, show the counter on top of the Thread graph as percentage of the CPU Time distribution.




Before you associate context metadata with a counter, you should create
an ITT API Domain and ITT API Counter Instances.


The domain name provides a heading for the section of metrics for the
counters in the results of VTune Profiler. A single domain can combine
data from any number of counters. However, the name of the counters must
be unique within the same domain.


You can combine different counters under a single metric of the Context
Metadata.


**Add context information**


Once you have created all objects, you can add context information for
the selected counters. Use these primitives:


``__itt_bind_context_metadata_to_counter(__itt_counter counter, size_t length, __itt_context_metadata* metadata);``


**Parameters of the primitive:**


.. list-table:: 
   :header-rows: 0

   * -  Type
     -  Parameter
     -  Description
   * -     [in]    
     -     \ ``__itt_counter counter``\    
     -      Pointer to the counter instance associated with the context metadata    
   * -     [in]    
     -     \ ``size_t length``\    
     -     Number of elements in the array of context metadata    
   * -     [in]    
     -     \ ``__itt_context_metadata* metadata``\    
     -      Pointer to the array of context metadata    




To create counter instances and submit counter data, use:


.. code-block:: cpp


    __itt_counter_create_v3(__itt_domain* domain, const char* name, __itt_metadata_type type);
   __itt_counter_set_value_v3(__itt_counter counter, void *value_ptr);


Usage Example
-------------


This example creates counters with context metadata that measures random
read operation metrics for an SSD NVMe device:


.. code:: cpp


   #include "ittnotify.h"
   #include "ittnotify_types.h"


   // Create domain and counters:
   __itt_domain* domain = __itt_domain_create("ITT API collected data");
   __itt_counter counter_read_op = __itt_counter_create_v3(domain, "Read Operations", __itt_metadata_u64);
   __itt_counter counter_read_mb = __itt_counter_create_v3(domain, "Read Megabytes", __itt_metadata_u64);
   __itt_counter counter_spin_time = __itt_counter_create_v3(domain, "Spin Time", __itt_metadata_u64);


   // Create context metadata:
   __itt_context_metadata metadata_read_op[] = {
           {__itt_context_name, "Reads"},
           {__itt_context_device, "NVMe SSD Intel DC 660p"},
           {__itt_context_units, "Operations"},
           {__itt_context_pci_addr, "0001:10:00.1"},
           {__itt_context_latency_flag, &true_flag}
   };
   __itt_context_metadata metadata_read_mb[] = {
           {__itt_context_name, "Read"},
           {__itt_context_device, "NVMe SSD Intel DC 660p"},
           {__itt_context_units, "MB"},
           {__itt_context_pci_addr, "0001:10:00.1"},
           {__itt_context_bandwidth_flag, &true_flag}
   };
   __itt_context_metadata metadata_spin_time[] = {
           {__itt_context_name, "Spin Time"},
           {__itt_context_device, "NVMe SSD Intel DC 660p"},
           {__itt_context_units, "ms"},
           {__itt_context_tid, &thread_id}
   };


   // Bind context metadata to counters:
   __itt_bind_context_metadata_to_counter(counter_read_op, n, metadata_read_op);
   __itt_bind_context_metadata_to_counter(counter_read_mb, n, metadata_read_mb);
   __itt_bind_context_metadata_to_counter(counter_spin_time, n, metadata_spin_time);


   while(1)
   {
       // Get collected data:
       uint64_t read_op   = get_user_read_operation_num();
       uint64_t read_mb   = get_user_read_megabytes_num();
       uint64_t spin_time = get_user_spin_time();


       // Dump collected data:
       __itt_counter_set_value_v3(counter_read_op, &read_op);
       __itt_counter_set_value_v3(counter_read_mb, &read_mb);
       __itt_counter_set_value_v3(counter_spin_time, &spin_time);
   }

