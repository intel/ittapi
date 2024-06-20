"""
domain.py - Python module wrapper for ITT Domain API
"""
from ittapi.native import Domain as _Domain


def domain(name=None):
    """
    Creates a domain with the given name.
    :param name: a name of the domain
    :return: a handle to the created domain with given name if the `name` is not None,
     otherwise, returns handle to default domain.
    """
    return _Domain(name)
