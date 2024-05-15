from sys import modules

from .patch import patch
from .pyitt_native_mock import PYITT_NATIVE_MODULE_NAME
from .pyitt_native_mock import PyittNativeMock


modules[PYITT_NATIVE_MODULE_NAME] = PyittNativeMock()
