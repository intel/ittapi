from unittest import main as unittest_main, TestCase

from pyitt_native_mock import patch as pyitt_native_patch
import ittapi


class StringHandleTests(TestCase):
    @pyitt_native_patch('StringHandle')
    def test_string_handle_call(self, string_handle_mock):
        s = 'my string'
        ittapi.string_handle(s)
        string_handle_mock.assert_called_once_with(s)


if __name__ == '__main__':
    unittest_main()  # pragma: no cover
