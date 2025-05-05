.. _ijit_notifyevent:

iJIT_NotifyEvent
================


Reports information about JIT-compiled code to the agent.


Syntax
------

.. code:: cpp


    int iJIT_NotifyEvent(iJIT_JVM_EVENT event_type, void EventSpecificData);


Description
-----------


The ``iJIT_NotifyEvent``\ function sends a notification of
``event_type`` with the data pointed by ``EventSpecificData`` to the
agent. The reported information is used to attribute samples obtained
from any profiling tool collector. This API needs to be called
after JIT compilation and before the first entry into the JIT-compiled
code.


Input Parameters
----------------


.. list-table:: 
   :header-rows: 0

   * -     Parameter   
     -     Description   
   * -     \ ``iJIT_JVM_EVENT event_type``\    
     -     Notification code sent to the agent. See a complete list of
           `event types <#EVENT_TYPE>`__ below.    
   * -     \ ``void *EventSpecificData``\    
     -     Pointer to `event specific data <#STRUCTURE>`__.   




The following values are allowed for ``event_type``:


.. list-table:: 
   :header-rows: 0

   * - 
     -     Send this notification after a JITted method has been loaded into
           memory, and possibly JIT compiled, but before the code is executed.
           Use the ``iJIT_Method_Load`` structure for ``EventSpecificData``.
           The return value of ``iJIT_NotifyEvent``\ is undefined.    
   * -     \ ``iJVM_EVENT_TYPE_SHUTDOWN``\    
     -     Send this notification to terminate profiling. Use NULL for
           ``EventSpecificData``. ``iJIT_NotifyEvent`` returns 1 on success.   
   * -     \ ``JVM_EVENT_TYPE_METHOD_UPDATE``\    
     -     Send this notification to provide new content for a previously reported
           dynamic code. The previous content will be invalidated starting from the
           time of the notification. Use the ``iJIT_Method_Load`` structure for
           ``EventSpecificData`` with the following required fields:   
   * - 
     -     Send this notification when an inline dynamic code is JIT compiled and
           loaded into memory by the JIT engine, but before the parent code region
           starts executing. Use the ``iJIT_Method_Inline_Load``
           structure for ``EventSpecificData``.   
   * - 
     -     Send this notification when a dynamic code is JIT compiled and loaded into
           memory by the JIT engine, but before the code is executed. Use the
           ``iJIT_Method_Load_V2`` structure for ``EventSpecificData``.   




The following structures can be used for ``EventSpecificData``:


**iJIT_Method_Inline_Load Structure**


When you use the ``iJIT_Method_Inline_Load`` structure to describe the
JIT compiled method, use ``iJVM_EVENT_TYPE_METHOD_INLINE_LOAD_FINISHED``
as an event type to report it. The\ ``iJIT_Method_Inline_Load``
structure has the following fields:


.. list-table:: 
   :header-rows: 0

   * -     Field    
     -     Description    
   * -     \ ``unsigned int method_id``\    
     -     Unique method ID. Method ID cannot be smaller than 999. You must
           either use the API function ``iJIT_GetNewMethodID`` to get a valid
           and unique method ID, or else manage ID uniqueness and correct
           range by yourself.    
   * -     \ ``unsigned int parent_method_id``\    
     -     Unique immediate parent’s method ID. Method ID may not be smaller
           than 999. You must either use the API function iJIT_GetNewMethodID
           to get a valid and unique method ID, or else manage ID uniqueness
           and correct range by yourself.    
   * -     \ ``char *method_name``\    
     -     The name of the method, optionally prefixed with its class name
           and appended with its complete signature. This argument cannot be set to NULL.    
   * -     \ ``void *method_load_address``\    
     -     The base address of the method code. Can be NULL if the method is not JITted.    
   * -     \ ``unsigned int method_size``\    
     -     The virtual address on which the method is inlined. If NULL,
           then data provided with the event are not accepted.    
   * -     \ ``unsigned int line_number_size``\    
     -     The number of entries in the line number table. 0 if none.    
   * -     \ ``pLineNumberInfo line_number_table``\    
     -     Pointer to the line numbers info array. Can be NULL if ``line_number_size``
           is 0. See ``LineNumberInfo`` structure for a description of a single entry
           in the line number info array.    
   * -     \ ``char *class_file_name``\    
     -     Class name. Can be NULL.    
   * -     \ ``char *source_file_name``\    
     -     Source file name. Can be NULL.    




**iJIT_Method_Load Structure**


When you use the\ ``iJIT_Method_Load`` structure to describe the JIT
compiled method, use ``iJVM_EVENT_TYPE_METHOD_LOAD_FINISHED`` as an
event type to report it. The\ ``iJIT_Method_Load`` structure has the
following fields:


.. list-table:: 
   :header-rows: 0

   * -     Field    
     -     Description    
   * -     \ ``unsigned int method_id``\    
     -     Unique method ID. Method ID cannot be smaller than 999. You must
           either use the API function ``iJIT_GetNewMethodID`` to get a valid
           and unique method ID, or else manage ID uniqueness and correct
           range by yourself.    
   * -     \ ``char *method_name``\    
     -     The name of the method, optionally prefixed with its class name
           and appended with its complete signature. This argument cannot be set to NULL.    
   * -     \ ``void *method_load_address``\    
     -     The base address of the method code. Can be NULL if the method is not JITted.    
   * -     \ ``unsigned int method_size``\    
     -     The virtual address on which the method is inlined. If NULL,
           then data provided with the event are not accepted.    
   * -     \ ``unsigned int line_number_size``\    
     -     The number of entries in the line number table. 0 if none.    
   * -     \ ``pLineNumberInfo line_number_table``\    
     -     Pointer to the line numbers info array. Can be NULL if ``line_number_size``
           is 0. See ``LineNumberInfo`` structure for a description of a single entry
           in the line number info array.    
   * -     \ ``unsigned int class_id``\    
     -     This field is obsolete.    
   * -     \ ``char *class_file_name``\    
     -     Class name. Can be NULL.    
   * -     \ ``char *source_file_name``\    
     -     Source file name. Can be NULL.    
   * -     \ ``void *user_data``\    
     -     This field is obsolete.    
   * -     \ ``unsigned int user_data_size``\    
     -     This field is obsolete.    
   * -     \ ``iJDEnvironmentType env``\    
     -     This field is obsolete.    




**iJIT_Method_Load_V2 Structure**


When you use the ``iJIT_Method_Load_V2`` structure to describe the JIT
compiled method, use ``iJVM_EVENT_TYPE_METHOD_LOAD_FINISHED_V2`` as an
event type to report it. The\ ``iJIT_Method_Load_V2`` structure has the
following fields:


.. list-table:: 
   :header-rows: 0

   * -     Field    
     -     Description    
   * -     \ ``unsigned int method_id``\    
     -     Unique method ID. Method ID cannot be smaller than 999. You must either
           use the API function ``iJIT_GetNewMethodID`` to get a valid and unique
           method ID, or else manage ID uniqueness and correct range by yourself.    
   * -     \ ``char *method_name``\    
     -     The name of the method, optionally prefixed with its class name and
           appended with its complete signature. This argument cannot be set to NULL.    
   * -     \ ``void *method_load_address``\    
     -     The base address of the method code. Can be NULL if the method is not JITted.    
   * -     \ ``unsigned int method_size``\    
     -     The virtual address on which the method is inlined. If NULL,
           then data provided with the event are not accepted.    
   * -     \ ``unsigned int line_number_size``\    
     -     The number of entries in the line number table. 0 if none.    
   * -     \ ``pLineNumberInfo line_number_table``\    
     -     Pointer to the line numbers info array. Can be NULL if ``line_number_size``
           is 0. See ``LineNumberInfo`` structure for a description of a single entry
           in the line number info array.    
   * -     \ ``char *class_file_name``\    
     -     Class name. Can be NULL.    
   * -     \ ``char *source_file_name``\    
     -     Source file name. Can be NULL.    
   * -     \ ``char *module_name``\    
     -     Module name. Can be NULL. The module name can be useful for
           distinguishing among different JIT engines.   




**LineNumberInfo Structure**


Use the ``LineNumberInfo`` structure to describe a single entry in the
line number information of a code region. A table of line number entries
provides information about how the reported code region is mapped to
source file. Profiling tool uses line number information to attribute
the samples (virtual address) to a line number. It is acceptable to
report different code addresses for the same source line:


.. list-table:: 
   :header-rows: 0

   * -     \ **Offset**\    
     -     \ **Line Number**\    
   * -      1    
     -     2    
   * -     12    
     -     4    
   * -     15    
     -     2   
   * -     18    
     -     1   
   * -     21    
     -     30   




Profilers construct the following table using the client data:


.. list-table:: 
   :header-rows: 0

   * -     \ **Code sub-range**\    
     -     \ **Line Number**\    
   * -      0-1    
     -     2    
   * -     1-12    
     -     4    
   * -     12-15    
     -     2   
   * -     15-18    
     -     1   
   * -     18-21    
     -     30   




The ``LineNumberInfo`` structure has the following fields:


.. list-table:: 
   :header-rows: 0

   * -     Field    
     -     Description    
   * -     \ ``unsigned int Offset``\    
     -     Opcode byte offset from the beginning of the method.    
   * -     \ ``unsigned int LineNumber``\    
     -     Matching source line number offset (from beginning of source file).    




Return Values
-------------


The return values are dependent on the particular ``iJIT_JVM_EVENT``.

