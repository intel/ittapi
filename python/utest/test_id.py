from unittest import main as unittest_main, TestCase

from ittapi_native_mock import patch as ittapi_native_patch
import ittapi


class IdTests(TestCase):
    @ittapi_native_patch('Id')
    def test_id_call(self, id_mock):
        domain = 'my domain'
        ittapi.id(domain)
        id_mock.assert_called_once_with(domain)


if __name__ == '__main__':
    unittest_main()  # pragma: no cover
