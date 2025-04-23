.. _domain-api:

Domain API
==========


A ``domain`` enables tagging trace data for different modules or
libraries in a program. Domains are specified by unique character
strings, for example ``TBB.Internal.Control``.


Each domain is represented by an opaque ``__itt_domain`` structure,
which you can use to tag each of the ITT API calls in your code.


You can selectively enable or disable specific domains in your
application, in order to filter the subsets of instrumentation that are
collected into the output trace capture file. Upon the first start, the
application registers its domains in a configuration file of the Intel®
GPA Graphics Monitor. By default, upon the first start of the
application all the domains are enabled. To enable or disable domains
for subsequent starts of the application, do the following before the
start:


#. Open the Graphics Monitor Launcher Screen.
#. Click the **Options** button.
#. Click the **Tracing** tab.
#. Turn on or off the **User Domains** toggle buttons.


The overhead of a disabled domain is a single if check.


**To create a domain, use the following primitives:**


``__itt_domain *ITTAPI__itt_domain_createA ( const char *name)``


``__itt_domain *ITTAPI __itt_domain_createW ( const wchar_t *name)``


Or use the macro in the following way:


``__itt_domain *ITTAPI __itt_domain_create (__TEXT("TBB.Internal.Control"))``


For a domain name, the URI naming style is recommended, for example,
com.my_company.my_application. The set of domains is expected to be
static over the application's execution time, therefore, there is no
mechanism to destroy a domain.


Any domain can be accessed by any thread in the process, regardless of
which thread created the domain. This call is thread-safe.


**Parameters of the primitives:**


.. list-table:: 

   * -     \ ``[in]``\    
     -     \ *name*\    
     -     Name of domain    


Usage Example
-------------


.. code:: cpp


   #include "ittnotify.h"
   __itt_domain* pD = __itt_domain_create(__TEXT("My Domain") );
    


