.. _load-module-api:

Load Module API
===============


You can use the Load Module API in your code to analyze a module that
was loaded in an alternate location that cannot otherwise be tracked by
Intel VTune Profiler. For example, this would allow you to analyze code
that is typically executed in an isolated environment to which there is
no visibility of the code. This API allows you to explicitly set the
module location in an address space for analysis by VTune Profiler.


.. list-table:: 
   :header-rows: 0

   * -     Use This Primitive    
     -     To Do This    
   * -     \ ``void                   __itt_module_loadW (void* start_addr,void* end_addr, const wchar_t* path)``\    
     -      Call this function after the relocation of a module. Provide the new start and end addresses for the module and the full path to the module on the local drive.    
   * -     \ ``void                   __itt_module_loadA(void* start_addr, void* end_addr, const char* path)``\    
     -      Call this function after the relocation of a module. Provide the new start and end addresses for the module and the full path to the module on the local drive.    
   * -     \ ``void                   __itt_module_load(void* start_addr, void* end_addr, const char* path)``\    
     -      Call this function after the relocation of a module. Provide the new start and end addresses for the module and the full path to the module on the local drive.    




Usage Example
-------------


.. code::


   #include "ittnotify.h"
   __itt_module_load(relocatedBaseModuleAddress, relocatedEndModuleAddress, '/some/path/to/dynamic/library.so');

