.. _user-defined-synchronization-api:

User-Defined Synchronization API
================================


While the Intel® VTune™ Profiler supports a significant portion of the
Windows\* OS and POSIX\* APIs, it is often useful for you to define your
own synchronization constructs. Any specially built constructs that you
create are not normally tracked by the VTune Profiler. However, the
VTune Profiler includes the synchronization API to help you gather
statistical information related to user-defined synchronization
constructs.


The User-Defined Synchronization API is a per-thread function that works
in resumed state. This function does not work in paused state.


Synchronization constructs may generally be modeled as a series of
signals. One thread or many threads may wait for a signal from another
group of threads telling them they may proceed with some action. By
tracking when a thread begins waiting for a signal, and then noting when
the signal occurs, the synchronization API can take a user-defined
synchronization object and give you an understanding of your code. The
API uses memory handles along with a set of primitives to gather
statistics on the user-defined synchronization object.


.. note::


   The User-Defined Synchronization API works with the **Threading**
   analysis type.


-  `Using User-Defined Synchronization API in Your
   Code <#USING_USER-DEFINED_SYNC>`__


-  `Usage Example: User-Defined
   Spin-Waits <#EXAMPLE_USER-DEFINED_SPIN-WAITS>`__


-  `Usage Example: User-Defined Synchronized Critical
   Section <#EXAMPLE_USER-DEFINED_CRITICAL_SECTION>`__


-  `Usage Example: User-Level Synchronized
   Barrier <#EXAMPLE_SYNCHRONIZED_BARRIER>`__


Using User-Defined Synchronization API in Your Code
---------------------------------------------------


The following table describes the user-defined synchronization API
primitives, available for use on Windows\* and Linux\* operating
systems:


.. list-table:: 
   :header-rows: 0

   * -     Use This Primitive    
     -     To Do This    
   * -     \ ``void                          __itt_sync_create (void *addr, const __itt_char *objtype, const __itt_char *objname, int attribute)``\    
     -      Register the creation of a sync object using char or Unicode string.    
   * -  ``void                        __itt_sync_rename (void *addr, const __itt_char *name)``
     -      Assign a name to a sync object using char or Unicode string, after it was created.    
   * -     \ ``void                          __itt_sync_destroy (void *addr)``\    
     -     Track lifetime of the destroyed object.    
   * -     \ ``void                         __itt_sync_prepare (void *addr)``\    
     -      Enter spin loop on user-defined sync object.    
   * -     \ ``void                         __itt_sync_cancel (void *addr)``\    
     -      Quit spin loop without acquiring spin object.    
   * -     \ ``void                         __itt_sync_acquired (void *addr)``\    
     -      Define successful spin loop completion (sync object acquired).    
   * -     \ ``void                          __itt_sync_releasing (void *addr)``\    
     -      Start sync object releasing code. This primitive is called before the lock release call.    




Each API call has a single parameter, addr. The address, not the value,
is used to differentiate between two or more distinct custom
synchronization objects. Each unique address enables the VTune Profiler
to track a separate custom object. Therefore, to use the same custom
object to protect access in different parts of your code, use the same
addr parameter around each.


When properly embedded in your code, the primitives tell the VTune
Profiler when the code is attempting to perform some type of
synchronization. Each prepare primitive must be paired with a cancel or
acquired primitive.


Each user-defined synchronization construct may involve any number of
synchronization objects. Each synchronization object must be triggered
off of a unique memory handle, which the user-defined synchronization
API uses to track the object. Any number of synchronization objects may
be tracked at one time using the user-defined synchronization API, as
long as each object uses a unique memory pointer. You can think of this
as modeling objects similar to the WaitForMultipleObjects function in
the Windows\* OS API. You can create more complex synchronization
constructs out of a group of synchronization objects; however, it is not
advisable to interlace different user-defined synchronization constructs
as this results in incorrect behavior.


API Usage Tips
--------------


The user-defined synchronization API requires proper placement of the
primitives within your code. Appropriate usage of the user-defined
synchronization API can be accomplished by following these guidelines:


-  Put a prepare primitive immediately before the code that attempts to
   obtain access to a synchronization object.


-  Put either a cancel primitive or an acquired primitive immediately
   after your code is no longer waiting for a synchronization object.


-  The releasing primitive should be used immediately before the code
   signals that it no longer holds a synchronization object.


-  When using multiple prepare primitives to simulate any construct that
   waits for multiple objects, the last individual cancel or acquired
   primitive on an object related to the group of prepare primitives
   determines if the behavior of the construct is a cancel or acquired
   respectively.


-  The time between a prepare primitive and an acquired primitive may be
   considered impact time


-  The time between a prepare primitive and a cancel primitive is
   considered blocking time, even though the processor does not
   necessarily block.


-  Improper use of the user-defined synchronization API results in
   incorrect statistical data.


Usage Example: User-Defined Spin-Waits
--------------------------------------


The prepare API indicates to the VTune Profiler that the current thread
is about to begin waiting for a signal on a memory location. This call
must occur before you invoke the user synchronization construct. The
prepare API must always be paired with a call to either the acquired or
cancel API.


The following code snippet shows the use of the prepare and acquired API
used in conjunction with a user-defined spin-wait construct:


.. code:: cpp


   long spin = 1;
   . . . .
   . . . .
   __itt_sync_prepare((void *) &spin );
   while(ResourceBusy);
   // spin wait;
   __itt_sync_acquired((void *) &spin );


Using the cancel API may be applicable to other scenarios where the
current thread tests the user synchronization construct and decides to
do something useful instead of waiting for a signal from another thread.
See the following code example:


.. code:: cpp


   long spin = 1;
   . . . .
   . . . .
   __itt_sync_prepare((void *) &spin );
   while(ResourceBusy)
   {
       __itt_sync_cancel((void *) &spin );
       

       //
       // Do useful work
       //
       . . . . .
       . . . . .
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


.. code:: cpp


   long spin = 1;
   . . . .
   . . . .
   __itt_sync_releasing((void *) &spin );
   // Code here should free the resource


Usage Example: User-Defined Synchronized Critical Section
---------------------------------------------------------


The following code snippet shows how to create a critical section
construct that can be tracked using the user-defined synchronization
API:


.. code:: cpp


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


This simple critical section example demonstrates how to use the
user-defined synchronization primitives. When looking at this example,
note the following points:


-  Each prepare primitive is paired with an acquired primitive or a
   cancel primitive.


-  The prepare primitive is placed immediately before the user code
   begins waiting for the user lock.


-  The acquired primitive is placed immediately after the user code
   actually obtains the user lock.


-  The releasing primitive is placed before the user code actually
   releases the user lock. This ensures that another thread does not
   call the acquired primitive before the VTune Profiler realizes that
   this thread has released the lock.


Usage Example: User-Level Synchronized Barrier
----------------------------------------------


Higher level constructs, such as barriers, are also easy to model using
the synchronization API. The following code snippet shows how to create
a barrier construct that can be tracked using the synchronization API:


.. code:: cpp


   Barrier()
   {
       teamflag = false;
       __itt_sync_releasing((void *) &counter);
       InterlockedIncrement(&counter);  // use the atomic increment primitive appropriate to your OS and compiler


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
           Wait for team flag
           __ itt_sync_acquired((void *) &teamflag);
       }
   }


When looking at this example, note the following points:


-  There are two synchronization objects in this barrier code. The
   counter object is used to do a gather-like signaling from all the
   threads to the final thread indicating that each thread has entered
   the barrier. Once the last thread hits the barrier it uses the
   teamflag object to signal all the other threads that they may
   proceed.


-  As each thread enters the barrier it calls \__itt_sync_releasing to
   tell the VTune Profiler that it is about to signal the last thread by
   incrementing counter


-  The last thread to enter the barrier calls \__itt_sync_acquired to
   tell the VTune Profiler that it was successfully signaled by all the
   other threads.


-  The last thread to enter the barrier calls \__itt_sync_releasing to
   tell the VTune Profiler that it is going to signal the barrier
   completion to all the other threads by setting teamflag


-  Each thread, except the last, calls the \__itt_sync_prepare primitive
   to tell the VTune Profiler that it is about to start waiting for the
   teamflag signal from the last thread.


-  Finally, before leaving the barrier, each thread calls the
   \__itt_sync_acquired primitive to tell the VTune Profiler that it
   successfully received the end-of-barrier signal.

