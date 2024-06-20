from unittest import main as unittest_main, TestCase

from ittapi_native_mock import patch as ittapi_native_patch
import ittapi


class StringHandleTests(TestCase):
    @ittapi_native_patch('StringHandle')
    def test_string_handle_call(self, string_handle_mock):
        s = 'my string'
        ittapi.string_handle(s)
        string_handle_mock.assert_called_once_with(s)


if __name__ == '__main__':
    unittest_main()  # pragma: no cover
