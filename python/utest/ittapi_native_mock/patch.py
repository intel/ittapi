from functools import wraps as _wraps
from sys import modules as _modules

from .ittapi_native_mock import ITTAPI_NATIVE_MODULE_NAME


class _IttapiNativeAttributeMock:
    def __init__(self, name):
        self._name = name

    def __enter__(self):
        return self.__reset_attribute_mock()

    def __exit__(self, *args):
        self.__reset_attribute_mock()

    def __native_module_attribute(self):
        return _modules[ITTAPI_NATIVE_MODULE_NAME].attributes().get(self._name)

    def __reset_attribute_mock(self):
        attr = self.__native_module_attribute()
        attr.reset_mock()
        attr.side_effect = None
        return attr


class _IttapiNativePatch:
    def __init__(self, attr_name):
        self._attr_name = attr_name

    def __call__(self, func):
        def wrapper(*args, **kwargs):
            with _IttapiNativeAttributeMock(self._attr_name) as attr_mock:
                new_args = args + (attr_mock,)
                return func(*new_args, **kwargs)

        return _wraps(func)(wrapper)


def patch(attr_name):
    return _IttapiNativePatch(attr_name)
