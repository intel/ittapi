.. _ijit_isprofilingactive:

iJIT_IsProfilingActive
======================


Returns the current mode of the agent.


Syntax
------


iJIT_IsProfilingActiveFlags JITAPI iJIT IsProfilingActive ( void )


Description
-----------


The ``iJIT_IsProfilingActive`` function returns the current mode of the
agent.


Input Parameters
----------------


None


Return Values
-------------


``iJIT_SAMPLING_ON``, indicating that agent is running, or
``iJIT_NOTHING_RUNNING`` if no agent is running.

