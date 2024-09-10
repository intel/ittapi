from unittest import main as unittest_main, TestCase
from ittapi_native_mock import patch as ittapi_native_patch
import ittapi.compat

class PT_RegionTests(TestCase):
    @ittapi_native_patch('PT_Region')
    def test_pt_region_create_call(self, pt_region_mock):
        s='my region'
        ittapi.compat.pt_region_create(s)
        pt_region_mock.assert_called_once_with(s)
    
    @ittapi_native_patch('PT_Region')
    @ittapi_native_patch('pt_region_begin')
    @ittapi_native_patch('pt_region_end')
    def test_pt_region_begin_call(self, pt_region_mock, pt_region_begin_mock, pt_region_end_mock):
        region_name='my region'
        pt_region=ittapi.compat.pt_region_create(region_name)
        pt_region_mock.assert_called_once_with(region_name)

        ittapi.compat.pt_region_begin(pt_region)
        pt_region_begin_mock.assert_called_once_with(pt_region)
        
        ittapi.compat.pt_region_end(pt_region)
        pt_region_end_mock.assert_called_once_with(pt_region)

if __name__ == '__main__':
    unittest_main() # pragma: no cover
