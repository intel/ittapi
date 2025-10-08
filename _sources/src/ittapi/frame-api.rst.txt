.. _frame-api:

Frame API
=========


Use the frame API to insert calls to the desired places in your code and
analyze performance per frame. A frame is defined as the time period between
the frame begin and frame end points. Frames display in Intel® VTune™
Profiler as a separate track, so they provide a way
to visually separate this data from typical task data.

The frame API is a per-process function that works in the resumed state. This
function does not work in the paused state.


Run the frame analysis to:

-  Analyze Windows OS game applications that use DirectX\* rendering.
-  Analyze graphical applications that perform repeated calculations.
-  Analyze transaction processing on a per-transaction basis to discover
   input cases that cause bad performance.


Frames represent a series of non-overlapping regions of Elapsed time.
Frames are global in nature and not connected with any specific thread.
ITT APIs that enable analyzing code frames and presenting the analysis
data.


Include Frame API to Your Code
------------------------------


Create a domain instance with the ``__itt_domain_create()`` function:


.. code:: cpp

   __itt_domain *ITTAPI__itt_domain_create ( const char *name );


Follow the URI naming style to create domain names. For example,
"com.my_company.my_application" is an acceptable format. The set of domains is
expected to be static over the execution time of the application. Therefore,
there is no mechanism to destroy a domain.

Any thread in the process can access any domain, irrespective of the thread
that created the domain. This call is thread-safe.


Define the beginning of the frame instance. An ``__itt_frame_begin_v3``
call must be paired with an ``__itt_frame_end_v3`` call:


.. code:: cpp
   
   
   void __itt_frame_begin_v3(const __itt_domain *domain, __itt_id *id);


Successive calls to ``__itt_frame_begin_v3`` with the same ID are
ignored until a call to ``__itt_frame_end_v3`` with the same ID.


+--------+----------------------+----------------------------------------------------------------+
| Type   | Parameter            | Description                                                    |
+========+======================+================================================================+
| [in]   | .. code-block:: cpp  | The domain for this frame instance                             |
|        |                      |                                                                |
|        |    domain            |                                                                |
+--------+----------------------+----------------------------------------------------------------+
| [in]   | .. code-block:: cpp  | The instance ID for this frame instance. Can be NULL, in which |
|        |                      | case the next call to ``__itt_frame_end_v3`` with NULL as the  |
|        |    id                | *id* parameter designates the end of the frame.                |
+--------+----------------------+----------------------------------------------------------------+


Define the end of the frame instance. A ``__itt_frame_end_v3`` call must
be paired with a ``__itt_frame_begin_v3`` call. The first call to
``__itt_frame_end_v3`` with a given ID ends the frame. Successive calls
with the same ID are ignored, as are calls that do not have a matching
``__itt_frame_begin_v3`` call:


.. code:: cpp


   void __itt_frame_end_v3(const __itt_domain *domain, __itt_id *id);


+--------+----------------------+--------------------------------------------------+
| Type   | Parameter            | Description                                      |
+========+======================+==================================================+
| [in]   | .. code-block:: cpp  | The domain for this frame instance               |
|        |                      |                                                  |
|        |    domain            |                                                  |
+--------+----------------------+--------------------------------------------------+
| [in]   | .. code-block:: cpp  | The instance ID for this frame instance, or NULL |
|        |                      | for the current instance                         |
|        |    id                |                                                  |
+--------+----------------------+--------------------------------------------------+


.. note::


   The analysis types based on the hardware event-based sampling
   collection are limited to 64 distinct frame domains.


Usage Guidelines
----------------


-  Use the frame API to denote the frame begin point and end point.
   Consider a frame as the time period between frame begin and end
   points.
-  VTune Profiler does not attribute the time/samples between
   ``__itt_frame_end_v3()`` and ``__itt_frame_begin_v3()`` to any
   program unit. In the viewpoint for the analysis, this information
   displays as ``[Unknown]``.
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


.. code:: cpp


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

