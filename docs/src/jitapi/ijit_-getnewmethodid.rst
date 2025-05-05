.. _ijit_-getnewmethodid:

iJIT\_ GetNewMethodID
=====================


Generates a new unique method ID.


Syntax
------

.. code:: cpp


    unsigned int iJIT_GetNewMethodID(void);


Description
-----------


The\ ``iJIT_GetNewMethodID`` function generates new method ID upon each
call. Use this API to obtain unique and valid method IDs for methods or
traces reported to the agent if you do not have your own mechanism to
generate unique method IDs.


Input Parameters
----------------


None


Return Values
-------------


A new unique method ID. When out of unique method IDs, this API function
returns 0.

