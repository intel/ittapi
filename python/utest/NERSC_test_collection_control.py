from unittest import main as unittest_main, TestCase
from ittapi_native_mock import patch as ittapi_native_patch
from ittapi import itt_NERSC

class CollectionControlTests(TestCase):
    @ittapi_native_patch('resume')
    def test_resume_call(self, resume_mock):
        itt_NERSC.resume()
        resume_mock.assert_called_once_with()

    @ittapi_native_patch('pause')
    def test_pause_call(self, pause_mock):
        itt_NERSC.pause()
        pause_mock.assert_called_once_with()

    @ittapi_native_patch('detach')
    def test_detach_call(self, detach_mock):
        itt_NERSC.detach()
        detach_mock.assert_called_once_with()

if __name__ == '__main__':
    unittest_main() # pragma: no cover


