.. _ijit_notifyevent:

iJIT_NotifyEvent
================


Reports information about JIT-compiled code to the agent.


Syntax
------

.. code-block:: cpp


   int iJIT_NotifyEvent(iJIT_JVM_EVENT event_type, void EventSpecificData);


Description
-----------


The ``iJIT_NotifyEvent`` function sends a notification of
``event_type`` with the data pointed by ``EventSpecificData`` to the
agent. The reported information is used to attribute samples obtained
from any profiling tool collector. Make sure to call this API after
JIT compilation and before the first entry into the JIT-compiled code.


Input Parameters
----------------


+-------------------------------+--------------------------------------------+
| Parameter                     | Description                                |
+===============================+============================================+
| .. code-block:: cpp           | Notification code sent to the agent.       |
|                               | See a complete list of event types below.  |
|   iJIT_JVM_EVENT event_type   | See a complete list of event types below.  |
+-------------------------------+--------------------------------------------+
| .. code-block:: cpp           | Pointer to event specific data.            |
|                               |                                            |
|   void *EventSpecificData     |                                            |
+-------------------------------+--------------------------------------------+


The following values are acceptable for ``event_type``:


+---------------------------------------------+---------------------------------------------------------------+
| Value                                       | Description                                                   |
+=============================================+===============================================================+
| .. code-block:: cpp                         | Send this notification after a JITted method has been loaded  |
|                                             | into memory, and possibly JIT compiled, but before the code   |
|   iJVM_EVENT_TYPE_METHOD_LOAD_FINISHED      | is executed. Use the iJIT_Method_Load structure for           |
|                                             | EventSpecificData. The return value of iJIT_NotifyEvent is    |
|                                             | undefined.                                                    |
+---------------------------------------------+---------------------------------------------------------------+
| .. code-block:: cpp                         | Send this notification to terminate profiling. Use NULL for   |
|                                             | EventSpecificData. iJIT_NotifyEvent returns 1 on success.     |
|   iJVM_EVENT_TYPE_SHUTDOWN                  |                                                               |
+---------------------------------------------+---------------------------------------------------------------+
| .. code-block:: cpp                         | Send this notification to provide new content for a dynamic   |
|                                             | code that was reported previously. The previous content is    |
|   JVM_EVENT_TYPE_METHOD_UPDATE              | invalidated, starting from the time of the notification.      |
|                                             | Use the iJIT_Method_Load structure for EventSpecificData      |
|                                             | with the following required fields:                           |
+---------------------------------------------+---------------------------------------------------------------+
| .. code-block:: cpp                         | Send this notification when an inline dynamic code is JIT     |
|                                             | compiled and loaded into memory by the JIT engine, but before |
|   JVM_EVENT_TYPE_METHOD_INLINE_LOAD_FINISHED| the parent code region starts executing. Use the              |
|                                             | iJIT_Method_Inline_Load structure for EventSpecificData.      |
+---------------------------------------------+---------------------------------------------------------------+
| .. code-block:: cpp                         | Send this notification when a dynamic code is JIT compiled    |
|                                             | and loaded into memory by the JIT engine, but before the code |
|   iJVM_EVENT_TYPE_METHOD_LOAD_FINISHED_V2   | is executed. Use the iJIT_Method_Load_V2 structure for        |
|                                             | EventSpecificData.                                            |
+---------------------------------------------+---------------------------------------------------------------+


You can use the following structures for ``EventSpecificData``:


**iJIT_Method_Inline_Load Structure**


When you use the ``iJIT_Method_Inline_Load`` structure to describe the
JIT compiled method, use ``iJVM_EVENT_TYPE_METHOD_INLINE_LOAD_FINISHED``
as an event type to report it. The\ ``iJIT_Method_Inline_Load``
structure has the following fields:


+------------------------------+------------------------------------------------+
| Field                        | Description                                    |
+==============================+================================================+
| .. code-block:: cpp          | Unique method ID.                              |
|                              | The Method ID cannot be smaller than 999.      |
|    unsigned int method_id    | Use the API function                           |
|                              | ``iJIT_GetNewMethodID`` to get a valid and     |
|                              | unique method ID, or choose to manage the      |
|                              | uniqueness and range of the ID.                |
+------------------------------+------------------------------------------------+
| .. code-block:: cpp          | Unique immediate parents method ID.            |
|                              | The Method ID cannot be smaller than 999.      |
|    unsigned int              | Use the API function                           |
|    parent_method_id          | ``iJIT_GetNewMethodID`` to get a valid and     |
|                              | unique method ID, or choose to manage the      |
|                              | uniqueness and range of the ID.                |
+------------------------------+------------------------------------------------+
| .. code-block:: cpp          | The name of the method, optionally prefixed    |
|                              | with its class name and appended with its      |
|    char *method_name         | complete signature. This argument cannot be    |
|                              | set to NULL.                                   |
+------------------------------+------------------------------------------------+
| .. code-block:: cpp          | The base address of the method code.           |
|                              | Can be NULL if the method is not JITted.       |
|    void *method_load_address |                                                |
+------------------------------+------------------------------------------------+
| .. code-block:: cpp          | The virtual address on which the method is     |
|                              | inlined. If NULL, then data provided with      |
|    unsigned int method_size  | the event are not accepted.                    |
+------------------------------+------------------------------------------------+
| .. code-block:: cpp          | The number of entries in the line number       |
|                              | table. 0 if none.                              |
|    unsigned int              |                                                |
|    line_number_size          |                                                |
+------------------------------+------------------------------------------------+
| .. code-block:: cpp          | Pointer to the line numbers info array.        |
|                              | Can be NULL if ``line_number_size`` is 0.      |
|    pLineNumberInfo           | See ``LineNumberInfo`` structure for a         |
|    line_number_table         | description of a single entry in the line      |
|                              | number info array.                             |
+------------------------------+------------------------------------------------+
| .. code-block:: cpp          | Class name.                                    |
|                              | Can be NULL.                                   |
|    char *class_file_name     |                                                |
+------------------------------+------------------------------------------------+
| .. code-block:: cpp          | Source file name.                              |
|                              | Can be NULL.                                   |
|    char *source_file_name    |                                                |
+------------------------------+------------------------------------------------+


**iJIT_Method_Load Structure**


When you use the\ ``iJIT_Method_Load`` structure to describe the JIT
compiled method, use ``iJVM_EVENT_TYPE_METHOD_LOAD_FINISHED`` as an
event type to report it. The\ ``iJIT_Method_Load`` structure has the
following fields:

+------------------------------+------------------------------------------------+
| Field                        | Description                                    |
+==============================+================================================+
| .. code-block:: cpp          | Unique method ID.                              |
|                              | Method ID cannot be smaller than 999.          |
|    unsigned int method_id    | You must either use the API function           |
|                              | ``iJIT_GetNewMethodID`` to get a valid and     |
|                              | unique method ID, or else manage ID            |
|                              | uniqueness and correct range by yourself.      |
+------------------------------+------------------------------------------------+
| .. code-block:: cpp          | The name of the method, optionally prefixed    |
|                              | with its class name and appended with its      |
|    char *method_name         | complete signature. This argument cannot be    |
|                              | set to NULL.                                   |
+------------------------------+------------------------------------------------+
| .. code-block:: cpp          | The base address of the method code.           |
|                              | Can be NULL if the method is not JITted.       |
|    void *method_load_address |                                                |
+------------------------------+------------------------------------------------+
| .. code-block:: cpp          | The virtual address on which the method is     |
|                              | inlined. If NULL, then data provided with      |
|    unsigned int method_size  | the event are not accepted.                    |
+------------------------------+------------------------------------------------+
| .. code-block:: cpp          | The number of entries in the line number       |
|                              | table. 0 if none.                              |
|    unsigned int              |                                                |
|    line_number_size          |                                                |
+------------------------------+------------------------------------------------+
| .. code-block:: cpp          | Pointer to the line numbers info array.        |
|                              | Can be NULL if ``line_number_size`` is 0.      |
|    pLineNumberInfo           | See ``LineNumberInfo`` structure for a         |
|    line_number_table         | description of a single entry in the line      |
|                              | number info array.                             |
+------------------------------+------------------------------------------------+
| .. code-block:: cpp          | This field is obsolete.                        |
|                              |                                                |
|    unsigned int class_id     |                                                |
+------------------------------+------------------------------------------------+
| .. code-block:: cpp          | Class name.                                    |
|                              | Can be NULL.                                   |
|    char *class_file_name     |                                                |
+------------------------------+------------------------------------------------+
| .. code-block:: cpp          | Source file name.                              |
|                              | Can be NULL.                                   |
|    char *source_file_name    |                                                |
+------------------------------+------------------------------------------------+
| .. code-block:: cpp          | This field is obsolete.                        |
|                              |                                                |
|    void *user_data           |                                                |
+------------------------------+------------------------------------------------+
| .. code-block:: cpp          | This field is obsolete.                        |
|                              |                                                |
|    unsigned int              |                                                |
|    user_data_size            |                                                |
+------------------------------+------------------------------------------------+
| .. code-block:: cpp          | This field is obsolete.                        |
|                              |                                                |
|    iJDEnvironmentType env    |                                                |
+------------------------------+------------------------------------------------+


**iJIT_Method_Load_V2 Structure**


When you use the ``iJIT_Method_Load_V2`` structure to describe the JIT
compiled method, use ``iJVM_EVENT_TYPE_METHOD_LOAD_FINISHED_V2`` as an
event type to report it. The\ ``iJIT_Method_Load_V2`` structure has the
following fields:

+------------------------------+------------------------------------------------+
| Field                        | Description                                    |
+==============================+================================================+
| .. code-block:: cpp          | Unique method ID.                              |
|                              | Method ID cannot be smaller than 999. You must |
|    unsigned int method_id    | either use the API function                    |
|                              | ``iJIT_GetNewMethodID`` to get a valid and     |
|                              | unique method ID, or else manage ID            |
|                              | uniqueness and correct range by yourself.      |
+------------------------------+------------------------------------------------+
| .. code-block:: cpp          | The name of the method, optionally prefixed    |
|                              | with its class name and appended with its      |
|    char *method_name         | complete signature. This argument cannot be    |
|                              | set to NULL.                                   |
+------------------------------+------------------------------------------------+
| .. code-block:: cpp          | The base address of the method code.           |
|                              | Can be NULL if the method is not JITted.       |
|    void *method_load_address |                                                |
+------------------------------+------------------------------------------------+
| .. code-block:: cpp          | The virtual address on which the method is     |
|                              | inlined. If NULL, then data provided with      |
|    unsigned int method_size  | the event are not accepted.                    |
+------------------------------+------------------------------------------------+
| .. code-block:: cpp          | The number of entries in the line number       |
|                              | table. 0 if none.                              |
|    unsigned int              |                                                |
|    line_number_size          |                                                |
+------------------------------+------------------------------------------------+
| .. code-block:: cpp          | Pointer to the line numbers info array.        |
|                              | Can be NULL if ``line_number_size`` is 0.      |
|    pLineNumberInfo           | See ``LineNumberInfo`` structure for a         |
|    line_number_table         | description of a single entry in the line      |
|                              | number info array.                             |
+------------------------------+------------------------------------------------+
| .. code-block:: cpp          | Class name.                                    |
|                              | Can be NULL.                                   |
|    char *class_file_name     |                                                |
+------------------------------+------------------------------------------------+
| .. code-block:: cpp          | Source file name.                              |
|                              | Can be NULL.                                   |
|    char *source_file_name    |                                                |
+------------------------------+------------------------------------------------+
| .. code-block:: cpp          | Module name.                                   |
|                              | Can be NULL. The module name can be useful for |
|    char *module_name         | distinguishing among different JIT engines.    |
+------------------------------+------------------------------------------------+


**LineNumberInfo Structure**


Use the ``LineNumberInfo`` structure to describe a single entry in the
line number information of a code region. A table of line number entries
provides information about how the reported code region is mapped to
source file. The Profiling tool uses line number information to attribute
the samples (virtual address) to a line number. You can report different
code addresses for the same source line:


+------------+-----------------+
| **Offset** | **Line Number** |
+============+=================+
| 1          | 2               |
+------------+-----------------+
| 12         | 4               |
+------------+-----------------+
| 15         | 2               |
+------------+-----------------+
| 18         | 1               |
+------------+-----------------+
| 21         | 30              |
+------------+-----------------+


Profilers construct the following table using the client data:


+-------------------+-----------------+
| **Code sub-range**| **Line Number** |
+===================+=================+
| 0-1               | 2               |
+-------------------+-----------------+
| 1-12              | 4               |
+-------------------+-----------------+
| 12-15             | 2               |
+-------------------+-----------------+
| 15-18             | 1               |
+-------------------+-----------------+
| 18-21             | 30              |
+-------------------+-----------------+


The ``LineNumberInfo`` structure has the following fields:


+------------------------------+----------------------------------------------+
| Field                        | Description                                  |
+==============================+==============================================+
| .. code-block:: cpp          | Opcode byte offset from the                  |
|                              | beginning of the method.                     |
|    unsigned int Offset       |                                              |
+------------------------------+----------------------------------------------+
| .. code-block:: cpp          | Matching source line number offset           |
|                              | (from beginning of source file).             |
|    unsigned int LineNumber   |                                              |
+------------------------------+----------------------------------------------+


Return Values
-------------


The return values are dependent on the particular ``iJIT_JVM_EVENT``.

