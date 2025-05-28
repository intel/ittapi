.. _context-metadata-api:

Context Metadata API
====================


Use the Context Metadata API to define custom counters in your code with
special attributes. You can also get a set of metrics for the collected data
in any classical form of data representation (bandwidth/latency/utilization
metrics) in Intel® VTune™ Profiler.


You can use Context Metadata API to collect counter-based metrics and
attribute these metrics to hardware topology like:

-  PCIe devices
-  Block devices
-  CPU cores
-  Threads


**Define and create a counter object**


Use this structure to store context metadata:


.. code-block:: cpp


   __itt_context_metadata
   {
       __itt_context_type type;    /*!< Type of the context metadata value */
       void* value;                /*!< Pointer to context metadata value itself */
   }


The structure accepts the following types of context metadata:


+-------------------------------+------------------------------------+-----------------------------------------------+
| __itt_context_type            | Value                              | Description                                   |
+===============================+====================================+===============================================+
| .. code-block:: cpp           | ASCII string char* /               | The name of the counter-based metric.         |
|                               | Unicode string wchar_t*            | This value is required.                       |
|   __itt_context_name          |                                    |                                               |
|                               |                                    |                                               |
+-------------------------------+------------------------------------+-----------------------------------------------+
| .. code-block:: cpp           | ASCII string char* /               | Statistics subdomain to break down the        |
|                               | Unicode string wchar_t*            | counter samples (for example, network port    |
|   __itt_context_device        |                                    | ID, disk partition, etc.)                     |
|                               |                                    |                                               |
+-------------------------------+------------------------------------+-----------------------------------------------+
| .. code-block:: cpp           | ASCII string char* /               | Units of measurement. For measurement of      |
|                               | Unicode string wchar_t*            | time, use the ns/us/ms/s units to correct     |
|   __itt_context_units         |                                    | data representation in VTune Profiler.        |
|                               |                                    |                                               |
+-------------------------------+------------------------------------+-----------------------------------------------+
| .. code-block:: cpp           | ASCII string char* /               | PCI address of device to associate with       |
|                               | Unicode string wchar_t*            | the counter.                                  |
|   __itt_context_pci_addr      |                                    |                                               |
|                               |                                    |                                               |
+-------------------------------+------------------------------------+-----------------------------------------------+
| .. code-block:: cpp           | Unsigned 64-bit integer type       |                                               |
|                               |                                    | Thread ID to associate with the counter.      |
|   __itt_context_tid           |                                    |                                               |
|                               |                                    |                                               |
+-------------------------------+------------------------------------+-----------------------------------------------+
| .. code-block:: cpp           | Unsigned 64-bit integer type (0,1) | If this flag is set to 1, calculate latency   |
|                               |                                    | histogram and counter/sec timeline            |
|   __itt_context_bandwidth_flag|                                    | distribution.                                 |
|                               |                                    |                                               |
+-------------------------------+------------------------------------+-----------------------------------------------+
| .. code-block:: cpp           | Unsigned 64-bit integer type (0,1) | If this flag is set to 1, calculate the       |
|                               |                                    | throughput histogram and counter/sec          |
|   __itt_context_latency_flag  |                                    | timeline distribution.                        |
|                               |                                    |                                               |
+-------------------------------+------------------------------------+-----------------------------------------------+
| .. code-block:: cpp           | Unsigned 64-bit integer type (0,1) | If this flag is set to 1, show the counter    |
|                               |                                    | on top of the Thread graph as percentage      |
|   __itt_context_on_thread_flag|                                    | of the CPU Time distribution.                 |
|                               |                                    |                                               |
+-------------------------------+------------------------------------+-----------------------------------------------+


Before you associate context metadata with a counter, make sure to create
an ITT API Domain and ITT API Counter Instances first.


The domain name provides a heading for the section of metrics for the
counters in the results of VTune Profiler. A single domain can combine
data from any number of counters. However, the name of the counters must
be unique within the same domain.


You can combine different counters under a single metric of the Context
Metadata.


**Add context information**


Once you have created all objects, you can add context information for
the selected counters. Use these primitives:


.. code-block:: cpp


   __itt_bind_context_metadata_to_counter(
        __itt_counter counter, size_t length, __itt_context_metadata* metadata);


**Parameters of the primitive:**


+--------+-------------------------------+-----------------------------------------------------+
| Type   | Parameter                     | Description                                         |
+========+===============================+=====================================================+
| [in]   | .. code-block:: cpp           | Pointer to the counter instance associated with the |
|        |                               |                                                     |
|        |    __itt_counter counter      | context metadata                                    |
+--------+-------------------------------+-----------------------------------------------------+
| [in]   | .. code-block:: cpp           | Number of elements in the array of context metadata |
|        |                               |                                                     |
|        |    size_t length              |                                                     |
+--------+-------------------------------+-----------------------------------------------------+
| [in]   | .. code-block:: cpp           | Pointer to the array of context metadata            |
|        |                               |                                                     |
|        |    __itt_context_metadata*    |                                                     |
|        |    metadata                   |                                                     |
+--------+-------------------------------+-----------------------------------------------------+


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
   __itt_domain* domain =
    __itt_domain_create("ITT API collected data");
   __itt_counter counter_read_op =
    __itt_counter_create_v3(domain, "Read Operations", __itt_metadata_u64);
   __itt_counter counter_read_mb =
    __itt_counter_create_v3(domain, "Read Megabytes", __itt_metadata_u64);
   __itt_counter counter_spin_time =
    __itt_counter_create_v3(domain, "Spin Time", __itt_metadata_u64);


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

