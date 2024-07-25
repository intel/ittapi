from types import ModuleType as _ModuleType
from unittest.mock import MagicMock as _MagicMock

ITTAPI_NATIVE_MODULE_NAME = 'ittapi.native'


class IttapiNativeMock(_ModuleType):
    def __init__(self):
        super().__init__(ITTAPI_NATIVE_MODULE_NAME)
        self.attrs = {
            'detach': _MagicMock(),
            'pause': _MagicMock(),
            'resume': _MagicMock(),
            'task_begin': _MagicMock(),
            'task_end': _MagicMock(),
            'task_begin_overlapped': _MagicMock(),
            'task_end_overlapped': _MagicMock(),
            'thread_set_name': _MagicMock(),
            'Domain': _MagicMock(),
            'Event': _MagicMock(),
            'Id': _MagicMock(),
            'StringHandle': _MagicMock(),
            # ittapi.compat
            'PT_Region': _MagicMock(),
            'pt_region_begin': _MagicMock(),
            'pt_region_end': _MagicMock(),
        }

    def __getattr__(self, item):
        return self.attrs.get(item)

    def attributes(self):
        return self.attrs
