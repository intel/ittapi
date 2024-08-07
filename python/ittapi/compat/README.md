# ittapi.compat

The module provides API of an earlier version of Python bindings: ([itt-python](https://github.com/oleksandr-pavlyk/itt-python/)). The idea is to allow users to effortlessly migrate to the `ittapi` package without losing any existing functionality. The module exposes the following ITT APIs:
 - Collection Control API
 - Domain API
 - Task API
 - Process Trace Region API


## Usage

### ittapi.compat module

#### Collection Control API
 
The `pause` and `resume` enables to focus the collection on a specific section of code while profiling, and start the application run with collection paused. 

The `detach` detaches the data collection. Application continues to work but no data is collected while profiling. 

```python
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
```

If the above code snipped is profiled with VTune or others, the execution begins with collection paused and only the interesting part of the code is profiled. 

#### Domain API

A domain enables tagging trace data for different modules or libraries in a program. 

```python
import ittapi.compat as itt

domain = itt.domain_create("<Domain Name>")
```

Above code snippet creates a domain with the given `Domain name` and returns a Domain object created with the given name. If `Domain name` is left empty then default Domain object with name as `ittapi` will be returned.

#### Task API

```python
import ittapi.compat as itt

domain = itt.domain_create("ittapi")
itt.task_begin(domain, <Task Name>)
# ... Task code
itt.task_end(domain)
```

If the above code is profiled with the VTune or others, 
`task_begin` creates a task instance on a thread with name as `Task Name`. This becomes the current task instance for that thread. A call to `task_end` on the same thread ends the current task instance.
