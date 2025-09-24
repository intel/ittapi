.. _load-module-api:

Load Module API
===============


Use the Load Module API in your code to analyze a module that
was loaded in an alternate location, and cannot otherwise be tracked by
Intel® VTune™ Profiler. For example, you could use the Load Module API to analyze code
that is typically executed in an isolated environment that contains
no visibility of the code. Use this API to explicitly set the
module location in an address space for analysis by VTune Profiler.


+-----------------------------------------------+------------------------------------------------------------------------+
| Use This Primitive                            | To Do This                                                             |
+===============================================+========================================================================+
| .. code-block:: cpp                           | Call this function after the relocation of a module. Provide the new   |
|                                               | start and end addresses for the module and the full path to the module |
|   void __itt_module_loadW(void* start_addr,   | on the local drive.                                                    |
|        void* end_addr,                        |                                                                        |
|        const wchar_t* path);                  |                                                                        |
+-----------------------------------------------+------------------------------------------------------------------------+
| .. code-block:: cpp                           | Call this function after the relocation of a module. Provide the new   |
|                                               | start and end addresses for the module and the full path to the module |
|   void __itt_module_loadA(void* start_addr,   | on the local drive.                                                    |
|        void* end_addr,                        |                                                                        |
|        const char* path);                     |                                                                        |
+-----------------------------------------------+------------------------------------------------------------------------+
| .. code-block:: cpp                           | Call this function after the relocation of a module. Provide the new   |
|                                               | start and end addresses for the module and the full path to the module |
|   void __itt_module_load(void* start_addr,    | on the local drive.                                                    |
|        void* end_addr,                        |                                                                        |
|        const char* path);                     |                                                                        |
+-----------------------------------------------+------------------------------------------------------------------------+


Usage Example
-------------


.. code-block:: cpp


   #include "ittnotify.h"

   __itt_module_load(relocatedBaseModuleAddress, relocatedEndModuleAddress, "/path/to/dynamic/library.so");

