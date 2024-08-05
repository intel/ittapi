# ittapi

ittapi is a Python binding to Intel Instrumentation and Tracing Technology (ITT) API. It provides a convenient way
to mark up the Python code for further performance analysis using performance analyzers from Intel like Intel VTune
or others.

NOTE: For compatibility and smooth migration with earlier versions of Python bindings([itt-python](https://github.com/oleksandr-pavlyk/itt-python/)), please replace `import itt` with `import ittapi.compat as itt`. For more details about `ittapi.compat`, visit [ittapi.compat page](./ittapi/compat/README.md).
       
ittapi supports following ITT APIs:
 - Collection Control API
 - Domain API
 - Event API
 - Id API
 - String Handle API
 - Task API
 - Thread Naming API

## Usage

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
