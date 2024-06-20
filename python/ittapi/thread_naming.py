"""
thread_naming.py - Python module wrapper for ITT Thread Naming API
"""
from ittapi.native import thread_set_name as _thread_set_name


def thread_set_name(name: str):
    """
    Sets a thread name of calling thread.
    :param name: the thread name
    """
    _thread_set_name(name)
