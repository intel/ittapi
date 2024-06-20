from sys import modules

from .patch import patch
from .ittapi_native_mock import ITTAPI_NATIVE_MODULE_NAME
from .ittapi_native_mock import IttapiNativeMock


modules[ITTAPI_NATIVE_MODULE_NAME] = IttapiNativeMock()
