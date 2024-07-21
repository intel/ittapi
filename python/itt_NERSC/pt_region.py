"""
pt_region.py - Python module wrapper for process trace control region(PT_Region) in ITT
"""

from ittapi.native import PT_Region as _PT_Region

def pt_region_create(name):
    """
        Will create a PT Region
    """
    print("Creating new PT region")
    return _PT_Region(name)