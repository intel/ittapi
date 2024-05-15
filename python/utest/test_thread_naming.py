from unittest import main as unittest_main, TestCase

from pyitt_native_mock import patch as pyitt_native_patch
import pyitt


class ThreadNamingTests(TestCase):
    @pyitt_native_patch('thread_set_name')
    def test_string_handle_call(self, thread_set_name_mock):
        name = 'my thread'
        pyitt.thread_set_name(name)
        thread_set_name_mock.assert_called_once_with(name)


if __name__ == '__main__':
    unittest_main()  # pragma: no cover
