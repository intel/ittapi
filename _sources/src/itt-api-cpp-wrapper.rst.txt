.. _itt-api-cpp-wrapper:

ITT API C++ Wrapper
===================

A modern, header-only C++ wrapper for the `ITT API <https://github.com/intel/ittapi>`_
instrumentation library. The wrapper provides RAII-based scoped helpers and type-safe
C++ abstractions over the existing C API.

Supported APIs
--------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - API
     - C++ Wrapper
   * - String Handle
     - ``ittapi::StringHandle``
   * - Domain
     - ``ittapi::Domain``
   * - Task
     - ``ittapi::ScopedTask``, ``Domain::task_begin()`` / ``Domain::task_end()``
   * - Region
     - ``ittapi::ScopedRegion``
   * - Frame
     - ``ittapi::ScopedFrame``
   * - Collection Control
     - ``ittapi::pause()``, ``ittapi::resume()``, ``ittapi::ScopedPause``
   * - Thread Naming
     - ``ittapi::set_thread_name()``

Requirements
------------

- C++17 or later
- The existing ``ittnotify`` static C-library

Including the Wrapper
---------------------

Use the umbrella header to get the full API:

.. code-block:: cpp

   #include <ittapi.hpp>

Or include individual headers:

.. code-block:: cpp

   #include <ittapi_domain.hpp>
   #include <ittapi_task.hpp>
   #include <ittapi_collection_control.hpp>

Example: Task Instrumentation
-----------------------------

.. code-block:: cpp

   #include <ittapi.hpp>

   #include <chrono>
   #include <thread>

   int main() {
       ittapi::set_thread_name("main");
       ittapi::Domain domain{"example.task"};
       ittapi::StringHandle task_name{"process"};

       ittapi::pause();
       ittapi::resume();

       {
           auto task = domain.task(task_name);
           std::this_thread::sleep_for(std::chrono::milliseconds(10));
       }

       return 0;
   }

Linking
-------

CMake Consumer
^^^^^^^^^^^^^^

.. code-block:: cmake

   find_package(ittapi CONFIG REQUIRED)
   target_link_libraries(my_app PRIVATE ittapi::cxx)

The ``ittapi::cxx`` target is an ``INTERFACE`` library that transitively links the
existing ``ittnotify`` static library and adds the C++ wrapper include directories.

Manual GCC/G++ (Linux)
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

   g++ -std=c++17 -O2 \
       -I<ittapi-install-prefix>/include \
       app.cpp \
       <ittapi-install-prefix>/lib/libittnotify.a \
       -ldl -pthread \
       -o app

Building with CMake
-------------------

From the repository root:

.. code-block:: bash

   cmake -B build -DCMAKE_BUILD_TYPE=Release -DITT_API_CPP_SUPPORT=ON
   cmake --build build

The ``ITT_API_CPP_SUPPORT`` option is ``OFF`` by default.

You can also build with the build script:

.. code-block:: bash

   python buildall.py --cpp

Running Tests
-------------

Tests are registered inside the ``cpp/`` subdirectory. After building:

.. code-block:: bash

   ctest --test-dir build/cpp --output-on-failure

Performance Tips
----------------

- **Pre-create** ``StringHandle`` **objects** for task/region names used in hot paths.
  The ``StringHandle`` overloads pass a raw pointer with no locking — this is the
  zero-overhead path.

  .. code-block:: cpp

     // Do this once at startup:
     ittapi::StringHandle name{"compute"};

     // Then in hot code:
     auto task = domain.task(name);  // no allocation, no lock

- **The** ``string_view`` **overloads** are convenient but allocate a ``std::string``
  on cache miss and acquire a lock in the C library. Use them for setup or infrequent
  tasks, not tight loops.
- **Create** ``Domain`` **objects once** and reuse them. Domain creation is a global
  lookup — store them as class members or globals, not as function locals called
  repeatedly.
- **Use overlapped tasks** (with IDs) only when you need tasks that end out of order.
  Stack-based tasks (without IDs) are simpler and carry less internal state.

API Reference
-------------

Free Functions
^^^^^^^^^^^^^^

- ``ittapi::pause()`` — Pause collection.
- ``ittapi::resume()`` — Resume collection.
- ``ittapi::set_thread_name(std::string_view name)`` — Set the current thread's name.

Classes
^^^^^^^

ittapi::StringHandle
""""""""""""""""""""

Lightweight wrapper around ``__itt_string_handle*``.

.. code-block:: cpp

   ittapi::StringHandle h{"my_handle"};
   h.valid(); // true if handle was created
   h.get();   // underlying __itt_string_handle*

ittapi::Domain
""""""""""""""

Lightweight wrapper around ``__itt_domain*`` with convenience factories.

.. code-block:: cpp

   ittapi::Domain d{"my.domain"};
   auto task   = d.task("task_name");     // returns ScopedTask (RAII)
   auto region = d.region("region_name"); // returns ScopedRegion
   auto frame  = d.frame();               // returns ScopedFrame

   d.task_begin("work");                  // manual task begin (simple stack-based)
   d.task_end();                          // manual task end

   __itt_id id = __itt_id_make(nullptr, 1);
   d.task_begin("overlapped", id, __itt_null);  // manual task begin (overlapped)
   d.task_end(id);                              // manual task end by ID

ittapi::ScopedTask
""""""""""""""""""

RAII wrapper for task begin/end. Use ``domain.overlapped_task()`` to create
an overlapped task (tasks that can end in any order). Each overlapped task
instance gets a unique ``__itt_id`` derived from its object address. The
``id()`` method returns this ID.

.. code-block:: cpp

   // Simple task
   {
       auto task = domain.task("work");
       // ... do work ...
       task.end();     // optional early end (idempotent)
   }                   // destructor ends task if still active

Overlapped tasks — use ``overlapped_task()``, optionally pass a parent ID:

.. code-block:: cpp

   {
       auto parent = domain.overlapped_task("parent");               // overlapped, no parent
       auto child  = domain.overlapped_task("child", parent.id());   // overlapped, child of parent

       parent.end();  // end parent while child is still running
   }                  // child ends here via destructor

Overlapped tasks with explicit IDs (advanced):

.. code-block:: cpp

   {
       __itt_id parent_id = __itt_id_make(nullptr, 1);
       auto parent = domain.task("parent", parent_id, __itt_null);

       __itt_id child_id = __itt_id_make(nullptr, 2);
       auto child = domain.task("child", child_id, parent_id);

       parent.end();  // end parent while child is still running
   }                  // child ends here via destructor

For manual (non-RAII) control:

.. code-block:: cpp

   domain.task_begin("work");
   // ... do work ...
   domain.task_end();

   // Or with overlapped tasks:
   __itt_id id = __itt_id_make(nullptr, 1);
   domain.task_begin("overlapped_work", id, __itt_null);
   // ... do work ...
   domain.task_end(id);

ittapi::ScopedRegion
""""""""""""""""""""

RAII wrapper for region begin/end.

ittapi::ScopedFrame
"""""""""""""""""""

RAII wrapper for frame begin/end. Supports explicit timestamp submission.

.. code-block:: cpp

   ittapi::ScopedFrame::submit(domain.get(), begin_ts, end_ts);

ittapi::ScopedPause
"""""""""""""""""""

RAII wrapper for pause/resume. Constructor pauses, destructor resumes.

.. code-block:: cpp

   {
       ittapi::ScopedPause sp;
       // collection is paused
       sp.resume_now();  // optional early resume
   }
