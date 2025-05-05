.. _frame-api:

Frame API
=========


Use the frame API to insert calls to the desired places in your code and
analyze performance per frame, where frame is the time period between
frame begin and end points. When frames are displayed in Intel® VTune™
Profiler, they are displayed in a separate track, so they provide a way
to visually separate this data from normal task data.


Frame API is a per-process function that works in resumed state. This
function does not work in paused state.


You can run the frame analysis to:


-  Analyze Windows OS game applications that use DirectX\* rendering.
-  Analyze graphical applications performing repeated calculations.
-  Analyze transaction processing on a per transaction basis to discover
   input cases that cause bad performance.


Frames represent a series of non-overlapping regions of Elapsed time.
Frames are global in nature and not connected with any specific thread.
ITT APIs that enable analyzing code frames and presenting the analysis
data.


Adding Frame API to Your Code
-----------------------------


Create a domain instance with the ``__itt_domain_create()`` function:


.. code:: c

   __itt_domain *ITTAPI__itt_domain_create ( const char *name );


For a domain name, the URI naming style is recommended, for example,
com.my_company.my_application. The set of domains is expected to be
static over the application's execution time, therefore, there is no
mechanism to destroy a domain. Any domain can be accessed by any
thread in the process, regardless of which thread created the domain.
This call is thread-safe.


Define the beginning of the frame instance. An ``__itt_frame_begin_v3``
call must be paired with an ``__itt_frame_end_v3`` call:


.. code:: c
   
   
   void __itt_frame_begin_v3(const __itt_domain *domain, __itt_id *id);


Successive calls to ``__itt_frame_begin_v3`` with the same ID are
ignored until a call to ``__itt_frame_end_v3`` with the same ID.


.. list-table:: 
   :header-rows: 0

   * -     \ ``[in]``\    
     -     \ *domain*\    
     -     The domain for this frame instance    
   * -     \ ``[in]``\    
     -     \ *id*\    
     -     The instance ID for this frame instance. Can be NULL,
           in which case the next call to ``__itt_frame_end_v3``
           with NULL as the *id* parameter designates the end
           of the frame.


Define the end of the frame instance. A ``__itt_frame_end_v3`` call must
be paired with a ``__itt_frame_begin_v3`` call. The first call to
``__itt_frame_end_v3`` with a given ID ends the frame. Successive calls
with the same ID are ignored, as are calls that do not have a matching
``__itt_frame_begin_v3`` call:


.. code:: c


   void __itt_frame_end_v3(const __itt_domain *domain, __itt_id *id);


.. list-table:: 
   :header-rows: 0

   * -     \ ``[in]``\    
     -     \ *domain*\    
     -     The domain for this frame instance    
   * -     \ ``[in]``\    
     -     \ *id*\    
     -     The instance ID for this frame instance, or NULL for the
           current instance    


.. note::


   The analysis types based on the hardware event-based sampling
   collection are limited to 64 distinct frame domains.


Guidelines for Frame API Usage
------------------------------


-  Use the frame API to denote the frame begin point and end point.
   Consider a frame as the time period between frame begin and end
   points.
-  VTune Profiler does not attribute the time/samples between
   ``__itt_frame_end_v3()`` and ``__itt_frame_begin_v3()`` to any
   program unit and displays it as ``[Unknown]`` in the viewpoint.
-  If there are consecutive ``__itt_frame_begin_v3`` calls in the same
   domain, treat it as a ``__itt_frame_end_v3``/``__itt_frame_begin_v3``
   pair.
-  Recursive/nested/overlapping frames for the same domain are not
   allowed.
-  The ``__itt_frame_begin_v3()`` and ``__itt_frame_end_v3()`` calls can
   be made from different threads.
-  The recommended maximum rate for calling the frame API is 1000 frames
   per second. A higher rate may result in large product memory
   consumption and slow finalization.


Usage Example
-------------


The following example uses the frame API to capture the Elapsed times
for the specified code sections.


.. code:: c


   #include "ittnotify.h"

   __itt_domain* pD = __itt_domain_create( L"My Domain" );

   pD->flags = 1; /* enable domain */

   for (int i = 0; i < getItemCount(); ++i)
   {
     __itt_frame_begin_v3(pD, NULL);
     do_foo();
     __itt_frame_end_v3(pD, NULL);
   }

   //...

   __itt_frame_begin_v3(pD, NULL);
   do_foo_1();
   __itt_frame_end_v3(pD, NULL);

   //...

   __itt_frame_begin_v3(pD, NULL);
   do_foo_2();
   __itt_frame_end_v3(pD, NULL);

