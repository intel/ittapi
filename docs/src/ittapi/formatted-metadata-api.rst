.. _formatted-metadata-api:

Formatted Metadata API
======================


Use the Formatted Metadata API to attach formatted string data to tasks with efficient string
formatting capabilities. This API extends the basic Metadata API by providing printf-style formatting functionality.

The Formatted Metadata API is a per-thread function that works in the resumed profiling state only.


API Functions
-------------

**To add formatted metadata to the current task, use the following primitive:**

.. code-block:: cpp

   void __itt_formatted_metadata_add(const __itt_domain *domain,
                                    __itt_string_handle *format_handle,
                                    ...);


**Parameters of the primitive:**

+--------+------------------------------+----------------------------------------------------+
| Type   | Parameter                    | Description                                        |
+========+==============================+====================================================+
| [in]   | .. code-block:: cpp          | Metadata domain                                    |
|        |                              |                                                    |
|        |    __itt_domain* domain      |                                                    |
+--------+------------------------------+----------------------------------------------------+
| [in]   | .. code-block:: cpp          | String handle containing the format string with    |
|        |                              | printf-style format specifiers                     |
|        |    __itt_string_handle*      |                                                    |
|        |    format_handle             |                                                    |
+--------+------------------------------+----------------------------------------------------+
| [in]   | .. code-block:: cpp          | Variable arguments corresponding to format         |
|        |                              | specifiers in the format string                    |
|        |    ...                       |                                                    |
+--------+------------------------------+----------------------------------------------------+


Metadata Visualization and Analysis in VTune
--------------------------------------------

Formatted metadata provides several benefits for visualization and analysis in Intel® VTune™ Profiler:

- **Timeline tooltips**: Metadata appears in tooltips when hovering over tasks in the timeline view,
  providing contextual runtime information.
- **Bottom-up view grouping**: Formatted metadata can be used as a grouping criteria in the bottom-up view,
  allowing you to organize and analyze tasks based on their metadata values.
- **Custom grouping**: Users can group tasks by their metadata values to identify patterns and performance
  characteristics across different execution contexts.
- **Square bracket notation for format specifiers**: When you include format specifiers in square brackets
  (e.g., ``[%s]``) within your format string, VTune treats these as special grouping identifiers.


Usage Example
-------------

.. code-block:: cpp

   #include "ittnotify.h"

   __itt_domain* domain = __itt_domain_create("FileProcessor");
   __itt_string_handle* operation_format = __itt_string_handle_create("Operation: [%s] on file %s");
   __itt_string_handle* performance_format = __itt_string_handle_create("Performance: %d bytes in %.2f ms");

   void process_file(const char* filename) {
       __itt_task_begin(domain, __itt_null, __itt_null, __itt_string_handle_create("process_file"));
       
       __itt_formatted_metadata_add(domain, operation_format, "file_processing", filename);
       
       __itt_task_begin(domain, __itt_null, __itt_null, __itt_string_handle_create("read_file"));
       
       int bytes_read = 1024;
       double read_time = 15.5;
       __itt_formatted_metadata_add(domain, performance_format, bytes_read, read_time);
       
       __itt_task_end(domain);
       
       __itt_task_begin(domain, __itt_null, __itt_null, __itt_string_handle_create("transform_data"));
       
       __itt_formatted_metadata_add(domain, operation_format, "data_transform", filename);
       
       __itt_task_end(domain);
       __itt_task_end(domain);
   }

   int main() {
       process_file("document.txt");
       process_file("image.jpg");
       return 0;
   }


Usage Guidelines
----------------

- Format strings support standard printf-style format specifiers (``%s``, ``%d``, ``%f``, etc.)
- Metadata is associated with the currently running task at the time of the API call
- **Limit to one metadata call per task** - making multiple calls to ``__itt_formatted_metadata_add`` for the same task may result in incorrect processing
- For optimal performance, limit the frequency and size of metadata additions
- Format specifiers in square brackets (e.g., ``[%s]``) create additional grouping options in VTune analysis views
- Function arguments are processed during the API calls
- Maximum length of a string argument is 256 symbols


.. note::

   The Formatted Metadata API works starting with VTune 2025.7 release.