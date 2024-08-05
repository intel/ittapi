"""
pt_region.py - Python module wrapper for process trace control region(PT_Region)
"""

from ittapi.native import PT_Region as _PT_Region, pt_region_begin as _pt_region_begin, pt_region_end as _pt_region_end

from .region import _CallSite, _NamedRegion

class PT_Region(_NamedRegion):
    """
        A class that represents common functionality depending upon the usage of ITT processor trace region.
    """
    def __init__(self, region=None):
        """
            Creates the instance of the class that represents ITT processor trace region
            :param region: a name for pt_region or a callable object (e.g. function) to wrap. If the callable object is passed 
                        then the name of this object is used as a name for the region.
        """
        super().__init__(region)

        self._region=_PT_Region(self._name)
    
    def __str__(self):
        return (f"{{ region name: '{str(self._name)}' }}")

    def __repr__(self):
        return (f'{self.__class__.__name__}({repr(self._name)})')

    def begin(self):
        """Marks the begining of a pt_region."""
        _pt_region_begin(self._region)
    
    def end(self):
        """Marks the end of a pt_region."""
        _pt_region_end(self._region)
    
    def get_pt_region(self):
        return self._region

def pt_region(pt_region=None):
    """
    Creates a PT_Region instance with the given arguments.
    :param pt_region: a name of the pt_region or a callable object
    :return: a PT_Region instance
    """
    region=_CallSite(_CallSite.CallerFrame) if pt_region is None else pt_region
    return PT_Region(region)