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
collected into the output trace capture file. To disable a domain set
its flag field to 0 value. This disables tracing for a particular domain
while keeping the rest of the code unmodified. The overhead of a
disabled domain is a single if check.


**To create a domain, use the following primitives:**


``__itt_domain *ITTAPI__itt_domain_create ( const char *name)``


For a domain name, the URI naming style is recommended, for example,
com.my_company.my_application. The set of domains is expected to be
static over the application's execution time, therefore, there is no
mechanism to destroy a domain.


Any domain can be accessed by any thread in the process, regardless of
which thread created the domain. This call is thread-safe.


**Parameters of the primitives:**


.. list-table:: 
   :header-rows: 0

   * -     \ ``[in]``\    
     -     \ *name*\    
     -     Name of domain    




Usage Example
-------------


.. code:: cpp


   #include "ittnotify.h"


   __itt_domain* pD = __itt_domain_create(L"My Domain" ); 
   pD->flags = 0; /* disable domain */

