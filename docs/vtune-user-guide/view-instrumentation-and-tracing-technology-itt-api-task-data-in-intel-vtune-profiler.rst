.. _view-instrumentation-and-tracing-technology-itt-api-task-data-in-intel-vtune-profiler:

View Instrumentation and Tracing Technology (ITT) API Task Data in Intel® VTune™ Profiler
=========================================================================================


User task and API data can be visualized in Intel® VTune™ Profiler
performance analysis results.


After you have added basic annotations to your application to control
performance data collection, you can view these annotations in the Intel
VTune Profiler timeline. All supported instrumentation and tracing
technology (ITT) API tasks can be visualized in VTune Profiler.


Use the following steps to include ITT API tasks in your performance
analysis collection:


#. 


   .. container::
      :name: LI_0A72C46E11A64608A344009FE3604A58


      Click the |image1| (standalone GUI)/|image2| (Visual Studio IDE)
      **Configure Analysis** button on the Intel® VTune™ Profiler
      toolbar.


      The **Configure Analysis** window opens.


#. Set up the analysis target in the **WHERE** and **WHAT** panes.


#. 


   .. container::
      :name: LI_235EFD2738DB45079C9BA80E38C3EB1B


      From **HOW** pane, click the |image3| Browse button and select an
      analysis type. For more information about each analysis type, see
      `Performance Analysis
      Setup <analyze-performance.html>`__.


#. Select the **Analyze user tasks, events, and counters** checkbox to
   view the API tasks, counters, and events that you added to your
   application code.


   .. note::


      In some cases, the **Analyze user tasks, events, and counters**
      checkbox is in the expandable **Details** section. To enable the
      checkbox, use the **Copy** button at the top of the tab to create
      an editable version of the analysis type configuration. For more
      information, see `Custom
      Analysis <custom-analysis.html>`__ .


#. Click the |image4| Start button to `run the
   analysis <control-data-collection.html>`__.


After collection completes, the analysis results appear in a viewpoint
specific to the analysis type selected. The API data collected is
available in the following locations:


-  


   .. container::
      :name: LI_5EF5EAB8A8BB4C07BDB94F5795C0709A


      Timeline view: Each API type appears differently on the timeline
      view. In the example below, the code was instrumented with the
      task API, frame API, event API, and collection control API. Tasks
      appear as yellow bars on the task thread. Frames appear at the top
      of the timeline in pink. Events appear on the appropriate thread
      as a triangle at the event time. Collection control events span
      the entire timeline. Hover over a task, frame, or event to view
      the type of API task.


      .. image:: GUID-EDB262AE-A77B-442B-9592-1EFA506C7128-low.png


-  


   .. container::
      :name: LI_A7EED03FA1734E9897CB44961C34A900


      Grid view: Set the **Grouping** to **Task Domain / Task Type /
      Function / Call Stack** or **Task Type / Function / Call Stack**
      to view task data in the grid pane.


      .. image:: GUID-1ABE0C25-5938-4D25-8D9E-F2004C907256-low.png


-  


   .. container::
      :name: LI_DF180FC79C644C788EF43E19B028ED7C


      Platform tab: Individual tasks are available in a larger view on
      the **Platform** tab. Hover over a task to get more information.


      .. image:: GUID-FCF7E148-45E3-4669-9D09-091ADEA45BBA-low.png


.. |image1| image:: GUID-594B6013-D1E9-4593-B9EC-85791339769D-low.png
.. |image2| image:: GUID-3AFC2DCE-141C-4709-9F29-7D143D40EEB9-low.png
.. |image3| image:: GUID-682C1D59-20E8-4428-AF7A-23727182579B-low.png
.. |image4| image:: GUID-05002AF5-830E-43B6-9CA1-A5015F87329D-low.png

