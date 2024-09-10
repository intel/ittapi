from unittest import main as unittest_main, TestCase
from ittapi_native_mock import patch as ittapi_native_patch
import ittapi.compat


class DomainTests(TestCase):
    @ittapi_native_patch('Domain')
    def test_domain_call_with_name(self, domain_mock):
        name = 'my domain'
        ittapi.compat.domain_create(name)
        domain_mock.assert_called_once_with(name)


if __name__ == '__main__':
    unittest_main()  # pragma: no cover
