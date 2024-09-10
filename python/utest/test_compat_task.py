from unittest import main as unittest_main, TestCase
from ittapi_native_mock import patch as ittapi_native_patch
import ittapi.compat
import ittapi

class TaskTests(TestCase):
    @ittapi_native_patch('Domain')
    @ittapi_native_patch('StringHandle')
    @ittapi_native_patch('task_begin')
    @ittapi_native_patch('task_end')
    def test_task_collection_control_call(self, domain_mock, task_name_mock, task_begin_mock, task_end_mock):
        domain_name='my domain'
        task_name_strType='my task'

        domain=ittapi.compat.domain_create(domain_name)
        domain_mock.assert_called_once_with(domain_name)
        
        ittapi.compat.task_begin(domain, task_name_strType)
        task_name_mock.assert_called_once_with(task_name_strType)
        task_begin_mock.assert_called_once_with(domain, ittapi.StringHandle(task_name_strType))
               
        ittapi.compat.task_end(domain)
        task_end_mock.assert_called_once_with(domain)


if __name__ == '__main__':
    unittest_main() # pragma: no cover
