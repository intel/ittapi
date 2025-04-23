.. _compile-and-link-with-ittapi:

*****************************
Compile and Link with ITT API
*****************************

    Before instrumenting your application, configure your build system to be able to reach the API headers and libraries:

        *   Add <ittapi_dir>\include to your INCLUDE path
        *   Add <ittapi_dir>\build_<target_platform>\<target_bits>\bin to your LIBRARIES path

Include the ITT API Header in Your Application
==============================================

    Add the following #include statements to every source file that you want to instrument:

    .. code-block:: c++
        
        #include <ittnotify.h>
    
    The ittnotify.h header contains definitions of ITT API routines and important macros which provide the correct logic of API invocation from a user application.
     
    
    The ITT API is designed to incur almost zero overhead when tracing is disabled. But if you need fully zero overhead, you can compile out all ITT API calls from your application by defining the INTEL_NO_ITTNOTIFY_API macro in your project at compile time, either on the compiler command line, or in your source file, prior to including the ittnotify.h file.

Link the libittnotify.lib Static Library to Your Application
============================================================

    You need to link the static library, libittnotify.lib, to your application. If tracing is enabled, this static library loads the ITT API data collector and forwards ITT API instrumentation data to Intel Analyzer.If tracing is disabled, the static library ignores ITT API calls, providing nearly zero instrumentation overhead.


    If you do not want to update your version of Visual Studio, you must link the libittnotify.lib static library to your application.

UNICODE Support
===============

    All ITT API functions with string arguments follow the Windows OS UNICODE convention, the actual function names are suffixed with A for the ASCII API and W for the UNICODE API. For example:

    .. code-block:: c++

        __itt_domain_createA( const char* );
        __itt_domain_createW( const wchar_t* );

    To facilitate cross-platform code development, the ITT API features the C macro __itt_domain_create. If a UNICODE macro is defined when compiling on a Windows OS, the C macro resolves into __itt_domain_createW, otherwise it resolves into __itt_domain_createA.

Conditional Compilation
=======================

    For best performance in the release version of your code, use conditional compilation to turn off annotations. Define the macro INTEL_NO_ITTNOTIFY_API before you include ittnotify.h during compilation to eliminate all __itt_* functions from your code.

    You can also remove the static library from the linking stage by defining this macro.
