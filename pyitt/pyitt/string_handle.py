"""
string_handle.py - Python module wrapper for ITT String Handle API
"""
from pyitt.native import StringHandle as _StringHandle


def string_handle(string: str):
    """
    Creates a handle for a string.
    :param string: a string
    :return: the handle to the given string
    """
    return _StringHandle(string)
