"""
pt_region.py - Python module wrapper for process trace control region(PT_Region)
"""

from ittapi.native import PT_Region as _PT_Region

def pt_region_create(name):
    """
    This function saves a region name marked with Intel PT API and returns a handle for the created region with the given name.
    :param name: a name for the region, this name can't be empty
    :return: a handle for the created region with the given name
    """
    return _PT_Region(name)
