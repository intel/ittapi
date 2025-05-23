.. _user-defined-synchronization-api:

User-Defined Synchronization API
================================


Although Intel® VTune™ Profiler supports several Windows* OS and POSIX* APIs,
you may find it useful to define your own synchronization constructs. VTune
Profiler does not typically track the custom constructs that you create.
However, you can use the Synchronization API to collect statistical information
about the synchronization constructs you have defined.

The User-Defined Synchronization API is a per-thread function that works
in the resumed profiling state only.

Synchronization constructs may generally be modeled as a series of signals.
One or several threads may wait for a signal from another group of threads to
inform them to proceed with specific action. The synchronization API tracks
from the instant when a thread begins to wait for a signal and then notes the
arrival of the signal. This information can help you understand your code
better. This API uses memory handles along with a set of primitives to gather
statistics on the user-defined synchronization object.


.. note::


   The User-Defined Synchronization API works with the **Threading**
   analysis type.


Using User-Defined Synchronization API in Your Code
---------------------------------------------------


The following table describes the user-defined synchronization API primitives
that are available for use on Windows* and Linux* operating systems:


+------------------------------------+-------------------------------------------+
| Use This Primitive                 | To Do This                                |
+====================================+===========================================+
| .. code-block:: cpp                | Register the creation of a                |
|                                    | sync object using char or Unicode string. |
|     void                           |                                           |
|     __itt_sync_create(             |                                           |
|         void *addr,                |                                           |
|         const __itt_char *objtype, |                                           |
|         const __itt_char *objname, |                                           |
|         int attribute)             |                                           |
+------------------------------------+-------------------------------------------+
| .. code-block:: cpp                | Assign a name to a sync object using char |
|                                    | or Unicode string, after it was created.  |
|     void                           |                                           |
|     __itt_sync_rename(             |                                           |
|         void *addr,                |                                           |
|         const __itt_char *name)    |                                           |
+------------------------------------+-------------------------------------------+
| .. code-block:: cpp                | Track lifetime of the destroyed object.   |
|                                    |                                           |
|     void                           |                                           |
|     __itt_sync_destroy(            |                                           |
|         void *addr)                |                                           |
+------------------------------------+-------------------------------------------+
| .. code-block:: cpp                | Enter spin loop on user-defined           |
|                                    | sync object.                              |
|     void                           |                                           |
|     __itt_sync_prepare(            |                                           |
|         void *addr)                |                                           |
+------------------------------------+-------------------------------------------+
| .. code-block:: cpp                | Quit spin loop without acquiring          |
|                                    | spin object.                              |
|     void                           |                                           |
|     __itt_sync_cancel(             |                                           |
|         void *addr)                |                                           |
+------------------------------------+-------------------------------------------+
| .. code-block:: cpp                | Define successful spin loop completion    |
|                                    | (sync object acquired).                   |
|     void                           |                                           |
|     __itt_sync_acquired(           |                                           |
|         void *addr)                |                                           |
+------------------------------------+-------------------------------------------+
| .. code-block:: cpp                | Start sync object releasing code.         |
|                                    | This primitive is called                  |
|     void                           | before the lock release call.             |
|     __itt_sync_releasing(          |                                           |
|         void *addr)                |                                           |
+------------------------------------+-------------------------------------------+


Each API call has a single parameter called addr. The address is used to
differentiate between two or more distinct custom synchronization objects.
Each unique address enables VTune Profiler to track a separate custom object.
Therefore, to use the same custom object to protect access in different parts
of your code, use the same addr parameter around each API call that operates
on that object.

When properly embedded in your code, the primitives inform VTune Profiler
when the code attempts to perform some type of synchronization. Each prepare
primitive must be paired with a cancel or acquired primitive.

A synchronization construct you define may involve any number of
synchronization objects. Each synchronization object must be triggered off
of a unique memory handle, which the user-defined synchronization API uses
to track the object. Any number of synchronization objects may be tracked
simultaneously using the user-defined synchronization API, as long as each
object uses a unique memory pointer. This action is similar to modeling
objects in the WaitForMultipleObjects function in the Windows* OS API.

You can create more complex synchronization constructs out of a group of
synchronization objects. However, avoid interlacing different user-defined
synchronization constructs as this may cause incorrect behavior.


API Usage Tips
--------------

The user-defined synchronization API requires proper placement of the
primitives within your code. Follow these guidelines:

-  Put a prepare primitive immediately before the code that attempts to
   obtain access to a synchronization object.

-  Put either a cancel primitive or an acquired primitive immediately
   after your code is no longer waiting for a synchronization object.

-  Use the releasing primitive immediately before when the code signals
   that no synchronization object is held.

-  When using multiple prepare primitives to simulate any construct that waits
   for multiple objects, the result is determined by the last cancel or
   acquired primitive called for any object in the group of objects.


Key Considerations and Performance Impact:

-  The time between a prepare primitive and an acquired primitive may be
   considered as impact time.

-  The time between a prepare primitive and a cancel primitive is
   considered blocking time, even though the processor does not
   necessarily block.

-  Improper use of the user-defined synchronization API results in
   incorrect statistical data.


Usage Example: User-Defined Spin-Waits
--------------------------------------


The prepare API indicates to VTune Profiler that the current thread
is about to begin waiting for a signal on a memory location. This call
must occur before you invoke the user synchronization construct. The
prepare API must always be paired with a call to either the acquired or
cancel API.


This example shows the use of the prepare and acquired API in conjunction
with a user-defined spin-wait construct:


.. code-block:: cpp


   long spin = 1;
   
   __itt_sync_prepare((void *) &spin );
   while(ResourceBusy);
   // spin wait;
   __itt_sync_acquired((void *) &spin );


You may want to use the cancel API in scenarios where the current thread tests
the user synchronization construct and chooses to focus on a different task
instead of wait for a signal from another thread. See this example:


.. code-block:: cpp


   long spin = 1;
   
   __itt_sync_prepare((void *) &spin );
   while(ResourceBusy)
   {
       __itt_sync_cancel((void *) &spin );
       //
       // Do useful work
       //
       //  ...
       //
       //  Once done with the useful work, this construct will test the
       //  lock variable and try to acquire it again. Before this can
       //  be done, a call to the prepare API is required.
       //
      __itt_sync_prepare((void *) &spin );
   }
   __itt_sync_acquired((void *) &spin);


After you acquire a lock, you must call the releasing API before the
current thread releases the lock. The following example shows how to use
the releasing API:


.. code-block:: cpp


   long spin = 1;
   
   __itt_sync_releasing((void *) &spin );
   // Code here should free the resource


Usage Example: User-Defined Synchronized Critical Section
---------------------------------------------------------


This example shows how to create a critical section construct that can be
tracked using the user-defined synchronization API:


.. code-block:: cpp


   CSEnter()
   {
     __itt_sync_prepare((void*) &cs);
     while(LockIsUsed)
     {
       if(LockIsFree)
       {
       // Code to actually acquire the lock goes here
       __itt_sync_acquired((void*) &cs);
       }
       if(timeout)
       {
       __itt_sync_cancel((void*) &cs );
       }
     }
   }
   CSLeave()
   {
   if(LockIsMine)
       {
           __itt_sync_releasing((void*) &cs);
           // Code to actually release the lock goes here
       }
   }


This critical section example demonstrates how to use the user-defined
synchronization primitives. Note the following points:


-  Each prepare primitive is paired with an acquired primitive or a
   cancel primitive.

-  The prepare primitive is placed immediately before the user code
   begins waiting for the user lock.

-  The acquired primitive is placed immediately after the user code
   actually obtains the user lock.

-  The releasing primitive is placed before the user code actually
   releases the user lock. This ensures that another thread does not
   call the acquired primitive before VTune Profiler realizes that
   this thread has released the lock.


Usage Example: User-Level Synchronized Barrier
----------------------------------------------


You can use the synchronization API to model higher level constructs, like
barriers. This example shows how to create a barrier construct that you can
track using the synchronization API:


.. code-block:: cpp


   Barrier()
   {
       teamflag = false;
       __itt_sync_releasing((void *) &counter);
       InterlockedIncrement(&counter);  // Use the atomic increment primitive
       
       if( counter == thread count )
       {
           __itt_sync_acquired((void *) &counter);
           __itt_sync_releasing((void *) &teamflag);
           teamflag = true;
           counter = 0;
       }
       else
       {
           __ itt_sync_prepare((void *) &teamflag);
           // Wait for team flag
           __ itt_sync_acquired((void *) &teamflag);
       }
   }


Note the following points:


-  There are two synchronization objects in this barrier code. The
   counter object is used to do a gather-like signaling from all the
   threads to the final thread, indicating that each thread has entered
   the barrier.
   Once the last thread hits the barrier, the thread uses the
   teamflag object to signal to all the other threads that they may
   proceed.

-  A thread entering the barrier calls ``__itt_sync_releasing()`` to
   inform VTune Profiler that it is about to signal the last thread by
   incrementing counter

-  The last thread to enter the barrier calls ``__itt_sync_acquired()`` to
   inform VTune Profiler that it was successfully signaled by all the
   other threads.

-  The last thread to enter the barrier calls ``__itt_sync_releasing()`` to
   inform VTune Profiler that it is going to signal the barrier
   completion to all the other threads by setting teamflag.

-  With the exception of the last thread, every other thread calls the
   ``__itt_sync_prepare()`` to inform VTune Profiler that it is about to
   start waiting for the teamflag signal from the last thread.

-  Finally, before leaving the barrier, each thread calls the
   ``__itt_sync_acquired()`` primitive to inform VTune Profiler that it
   received the end-of-barrier signal successfully.

