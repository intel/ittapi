from ittapi.native import task_begin as _task_begin, task_end as _task_end

from ittapi.native import StringHandle as _StringHandle

def task_begin(domain, task_name:str):
    """
    Marks the beginning of a task.
    """
    _task_begin(domain, _StringHandle(task_name))

def task_end(domain):
    """
    Marks the ending of task.
    """
    _task_end(domain)
