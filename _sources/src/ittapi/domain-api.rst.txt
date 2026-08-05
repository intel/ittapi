.. _domain-api:

Domain API
==========


A ``domain`` enables you to tag trace data for different modules or
libraries in a program. You specify domains using unique character
strings.

Each domain is represented by an opaque ``__itt_domain`` structure,
which you can use to tag each of the ITT API calls in your code.

You can selectively enable or disable specific domains in your
application in order to filter the subsets of instrumentation that are
collected into the output trace capture file.

To disable a domain, set its flag field to 0. This action disables tracing
for a particular domain without affecting other code portions. The overhead
of a disabled domain is a single ``if`` check.


**To create a domain, use the following primitives:**

.. code:: cpp


   __itt_domain *ITTAPI__itt_domain_create ( const char *name);


To create a domain name, use the URI naming convention. For example,
"com.my_company.my_application" is an acceptable format for a domain name.
The set of domains is expected to be static over the execution time of the
application. Therefore, there is no mechanism to destroy a domain.

Any thread in the process can access any domain in the code, regardless of
the thread that created the domain. This call is thread-safe.


**Parameters of the primitives:**


+--------+--------------------------+-------------------+
| Type   | Parameter                | Description       |
+========+==========================+===================+
| [in]   | .. code-block:: cpp      | Name of domain    |
|        |                          |                   |
|        |    name                  |                   |
+--------+--------------------------+-------------------+


Usage Example
-------------


.. code:: cpp


   #include "ittnotify.h"


   __itt_domain* pD = __itt_domain_create(L"My Domain" ); 
   pD->flags = 0; /* disable domain */

