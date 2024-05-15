"""
id.py - Python module wrapper for ITT ID API
"""
from pyitt.native import Id as _Id


def id(domain):
    """
    Creates a unique identifier.
    :param domain: a domain that controls the creation of the identifier
    :return: an instance of the identifier
    """
    return _Id(domain)
