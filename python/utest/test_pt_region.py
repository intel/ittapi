from unittest import main as unittest_main, TestCase
from ittapi_native_mock import patch as ittapi_native_patch
import ittapi

class PT_RegionTests(TestCase):
    @ittapi_native_patch('PT_Region')
    def test_pt_region_create_call(self, pt_region_mock):
        s='my region'
        ittapi.pt_region_create(s)
        pt_region_mock.assert_called_once_with(s)
    
    @ittapi_native_patch('PT_Region')
    @ittapi_native_patch('pt_region_begin')
    @ittapi_native_patch('pt_region_end')
    def test_pt_region_begin_call(self, pt_region_mock, pt_region_begin_mock, pt_region_end_mock):
        region_name='my region'
        pt_region=ittapi.pt_region_create(region_name)
        pt_region_mock.assert_called_once_with(region_name)

        ittapi.pt_region_begin(pt_region)
        pt_region_begin_mock.assert_called_once_with(pt_region)
        
        ittapi.pt_region_end(pt_region)
        pt_region_end_mock.assert_called_once_with(pt_region)

    @ittapi_native_patch('StringHandle')
    @ittapi_native_patch('PT_Region')
    @ittapi_native_patch('pt_region_begin')
    @ittapi_native_patch('pt_region_end')
    def test_pt_region_context_manager_call(self, string_handle_mock, pt_region_mock, pt_region_begin_mock, pt_region_end_mock):
        string_handle_mock.side_effect = lambda x: x
        
        region='my region'
        
        ptRegion=ittapi.pt_region(region)
        with ptRegion:
            pass
        string_handle_mock.assert_called_once_with(region)
        pt_region_mock.assert_called_once_with(region)
        pt_region_begin_mock.assert_called_once_with(ptRegion.get_pt_region())
        pt_region_end_mock.assert_called_once_with(ptRegion.get_pt_region())
    
    @ittapi_native_patch('StringHandle')
    @ittapi_native_patch('PT_Region')
    @ittapi_native_patch('pt_region_begin')
    @ittapi_native_patch('pt_region_end')
    def test_pt_region_decorator_call(self, string_handle_mock, pt_region_mock, pt_region_begin_mock, pt_region_end_mock):
        string_handle_mock.side_effect = lambda x: x
        
        region='my region'
        
        ptRegion=ittapi.pt_region(region)

        @ptRegion
        def func():
            pass
        
        func()

        string_handle_mock.assert_called_once_with(region)
        pt_region_mock.assert_called_once_with(region)
        pt_region_begin_mock.assert_called_once_with(ptRegion.get_pt_region())
        pt_region_end_mock.assert_called_once_with(ptRegion.get_pt_region())

if __name__ == '__main__':
    unittest_main() # pragma: no cover


