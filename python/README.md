# ittapi

ittapi is a Python binding to Intel Instrumentation and Tracing Technology (ITT) API. It provides a convenient way to mark up the Python code for further performance analysis using performance analyzers from Intel like Intel VTune or others.

The ittapi package has following modules:
- ittapi module - The module consists of main ITT APIs and supports following ITT APIs:

       - Collection Control API
       - Domain API
       - Event API
       - Id API
       - String Handle API
       - Task API
       - Thread Naming API

- ittapi.compat module : The module is a wrapper to an earlier implementation of ITT API: (https://github.com/oleksandr-pavlyk/itt-python/tree/master). The idea is to allow users to effortlessly migrate to this new package without losing any existing functionality. The module is a wrapper to following ITT APIs:

       - Collection Control API
       - Domain API
       - Task API
       - Process Trace Region API


## Usage

### ittapi module

The main goal of the project is to provide the ability to instrument a Python code using ITT API in the Pythonic way.
ittapi provides wrappers that simplify markup of Python code.

```python
import ittapi

@ittapi.task
def workload():
  pass

workload()
```

`ittapi.task` can be used as a decorator. In this case, the name of a callable object (`workload` function in this
example) will be used as a name of the task and the task will be attributed to a default domain named 'ittapi'.
If you want to change the default name and/or other parameters for the task (e.g. task domain), you can pass
them as arguments to `ittapi.task`:

```python
import ittapi

@ittapi.task('My Task', domain='My Task Domain')
def workload():
  pass

workload()
```

Also, `ittapi.task` returns the object that can be used as a context manager:

```python
import ittapi

with ittapi.task():
    # some code here...
    pass
```

If the task name is not specified, the `ittapi.task` uses call site information (filename and line number) to give
the name to the task. A custom name for the task and other task parameters can be specified via arguments
for `ittapi.task` in the same way as for the decorator form.

### ittapi.compat module

#### Collection Control API
 
The `pause` and `resume` enables to focus the collection on a specific section of code while profiling, and start the application run with collection paused. 

The `detach` detaches the data collection. Application continues to work but no data is collected while profiling. 

       import ittapi.compat as itt
       # ... uninteresting code
       itt.resume()
       # ... very interesting code
       itt.pause()
       # ... boring stuff again
       itt.resume()
       # ... interesting code
       itt.detach()
       # ... uninteresting code like writing output

If the above code snipped is profiled with VTune or others, the execution begins with collection paused and only the interesting part of the code is profiled. 

#### Domain API

A domain enables tagging trace data for different modules or libraries in a program. 

       import ittapi.compat as itt

       domain = itt.domain_create("<Domain Name>")

Above code snippet creates a domain with the given `Domain name` and returns a Domain object created with the given name. If `Domain name` is left empty then default Domain object with name as `ittapi` will be returned.

#### Task API

       import ittapi.compat as itt

       domain = itt.domain_create("ittapi")
       itt.task_begin(domain, <Task Name>)
       # ... Task code
       itt.task_end(domain)

If the above code is profiled with the VTune or others, 
`task_begin` creates a task instance on a thread with name as `Task Name`. This becomes the current task instance for that thread. A call to `task_end` on the same thread ends the current task instance.


## Installation

ittapi package is available on PyPi and can be installed in the usual way for the supported configurations:

       pip install ittapi

## Build

The native part of ittapi module is written using C++20 standard, therefore you need a compiler that supports this
standard, for example GCC-10 for Linux and Visual Studio 2022 for Windows.

### Ubuntu 22.04

1. Install the compiler and Python utilities to build module:

       sudo apt install gcc g++ python3-pip

2. Clone the repository:

       git clone https://github.com/intel/ittapi.git

3. Build and install ittapi:

       cd python
       pip install .

### Windows 10/11

1. Install [Python 3.8+](https://www.python.org/downloads/) together with pip utility.

2. Install [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/).
     Make sure that "Desktop development with C++" workload is selected.

3. Clone the repository

       git clone https://github.com/intel/ittapi.git

4. Build and install ittapi

       cd python
       pip install .

## References

 - [Instrumentation and Tracing Technology APIs](https://www.intel.com/content/www/us/en/docs/vtune-profiler/user-guide/2023-0/instrumentation-and-tracing-technology-apis.html)
 - [Intel® VTune™ Profiler User Guide - Task Analysis](https://www.intel.com/content/www/us/en/docs/vtune-profiler/user-guide/2023-0/task-analysis.html)
 - [Intel® Graphics Performance Analyzers User Guide - Instrumentation and Tracing Technology API Support](https://www.intel.com/content/www/us/en/docs/gpa/user-guide/2022-4/instrumentation-and-tracing-technology-apis.html)
