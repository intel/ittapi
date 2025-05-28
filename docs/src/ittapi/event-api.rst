.. _event-api:

Event API
=========


Use the event API to:

-  Observe when demarcated events occur in your application
-  Find out the time taken to execute demarcated regions of code.

Set annotations in the application to demarcate areas where events of
interest occur. After you run the analysis, you can see the events marked
in the Timeline pane.

The event API is a per-thread function that works in the resumed state. This
function does not work in the paused state.


.. note::


   -  On Windows\* OS platforms you can define Unicode to use a wide
      character version of APIs that pass strings. However, these
      strings are internally converted to ASCII strings.


   -  On Linux\* OS platforms only a single variant of the API exists.


+----------------------------------------------------------+--------------------------------------------------------------------------+
| Use This Primitive                                       | To Do This                                                               |
+==========================================================+==========================================================================+
| .. code-block:: cpp                                      | Create an event type with the specified name and length. This API        |
|                                                          | returns a handle to the event type. The handle should be passed into     |
|   __itt_event __itt_event_create(const __itt_char *name, | the following event start and event end APIs as a parameter. The         |
|   int namelen );                                         | namelen parameter refers to the name length in number of characters.     |
+----------------------------------------------------------+--------------------------------------------------------------------------+
| .. code-block:: cpp                                      | Call this API with the event type handle to register an instance of the  |
|                                                          | event. The Event start appears in the Timeline pane display as a tick    |
|   int __itt_event_start(__itt_event event);              | mark.                                                                    |
+----------------------------------------------------------+--------------------------------------------------------------------------+
| .. code-block:: cpp                                      | Call this API after a call to __itt_event_start() to show the event as a |
|                                                          | check mark with a duration line from start to end. If this API is not    |
|   int __itt_event_end(__itt_event event);                | called, the event appears in the Timeline pane as a single check mark.   |
+----------------------------------------------------------+--------------------------------------------------------------------------+


Usae Guidelines
---------------


-  An __itt_event_end() is always matched with the nearest preceding
   __itt_event_start(). Otherwise, the __itt_event_end() call is
   matched with the nearest unmatched __itt_event_start() preceding it.
   Any intervening events are nested.

-  You can nest user events of the same type or different types within each
   other. In the case of nested events, the time is considered to have
   been spent only in the most deeply nested user event region.

-  You can overlap different ITT API events. In the case of overlapping
   events, the time is considered to have been spent only in the event
   region with the later __itt_event_start(). Unmatched
   __itt_event_end() calls are ignored.


.. note::


   To see events and user tasks in your results, `create a custom
   analysis <custom-analysis.html>`__ (based
   on the pre-defined analysis you are interested in) and select the
   **Analyze user tasks, events and counters** checkbox in the analysis
   settings.


Usage Example: Creating and Marking Single Events
-------------------------------------------------


The \__itt_event_create API returns a new event handle that you can
subsequently use to mark user events with the \__itt_event_start API. In
this example, two event type handles are created and used to set the
start points for tracking two different types of events.


.. code:: cpp


   #include "ittnotify.h"


   __itt_event mark_event = __itt_event_create( "User Mark", 9 );
   __itt_event frame_event = __itt_event_create( "Frame Completed", 15 );
   ...
   __itt_event_start( mark_event );
   ...
   for( int f ; f<number_of_frames ; f++ ) {
     ...
     __itt_event_start( frame_event );
   }


Usage Example: Creating and Marking Event Regions
-------------------------------------------------


The \__itt_event_start API can be followed by an \__itt_event_end API to
define an event region, as in the following example:


.. code:: cpp


   #include "ittnotify.h"


   __itt_event render_event = __itt_event_create( "Rendering Phase", 15 );
   ...
   for( int f ; f<number_of_frames ; f++ ) {
     ...
     do_stuff_for_frame();
     ...
     __itt_event_start( render_event );
     ...
     do_rendering_for_frame();
     ...
     __itt_event_end( render_event );
     ...
   }

