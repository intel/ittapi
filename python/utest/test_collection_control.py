from unittest import main as unittest_main, TestCase

from ittapi_native_mock import patch as ittapi_native_patch
import ittapi


class DirectCollectionControlTests(TestCase):
    @ittapi_native_patch('detach')
    def test_detach_call(self, detach_mock):
        ittapi.collection_control.detach()
        detach_mock.assert_called_once()

    @ittapi_native_patch('pause')
    def test_pause_call(self, pause_mock):
        ittapi.collection_control.pause()
        pause_mock.assert_called_once()

    @ittapi_native_patch('resume')
    def test_resume_call(self, resume_mock):
        ittapi.collection_control.resume()
        resume_mock.assert_called_once()


class ActiveRegionTests(TestCase):
    @ittapi_native_patch('pause')
    @ittapi_native_patch('resume')
    def test_active_region_as_decorator(self, pause_mock, resume_mock):
        @ittapi.active_region
        def my_function():
            return 42

        self.assertEqual(my_function(), 42)
        resume_mock.assert_called_once()
        pause_mock.assert_called_once()

    @ittapi_native_patch('pause')
    @ittapi_native_patch('resume')
    def test_active_region_as_context_manager(self, pause_mock, resume_mock):
        with ittapi.active_region():
            pass

        resume_mock.assert_called_once()
        pause_mock.assert_called_once()

    @ittapi_native_patch('pause')
    @ittapi_native_patch('resume')
    def test_active_region_with_manual_activation(self, pause_mock, resume_mock):
        region = ittapi.active_region()

        region.activator.deactivate()
        with region:
            pass

        resume_mock.assert_not_called()
        pause_mock.assert_not_called()

        region.activator.activate()
        with region:
            pass

        resume_mock.assert_called_once()
        pause_mock.assert_called_once()

    @ittapi_native_patch('pause')
    @ittapi_native_patch('resume')
    def test_active_region_with_custom_activator(self, pause_mock, resume_mock):
        for i in range(4):
            with ittapi.active_region(activator=lambda: i % 2):  # pylint: disable=W0640
                pass

        self.assertEqual(resume_mock.call_count, 2)
        self.assertEqual(pause_mock.call_count, 2)

    @ittapi_native_patch('pause')
    @ittapi_native_patch('resume')
    def test_active_region_as_decorator_without_activator(self, pause_mock, resume_mock):
        @ittapi.active_region(activator=None)
        def my_function():
            return 42

        self.assertEqual(my_function(), 42)
        resume_mock.assert_called_once()
        pause_mock.assert_called_once()


class PausedRegionTests(TestCase):
    @ittapi_native_patch('pause')
    @ittapi_native_patch('resume')
    def test_paused_region_as_decorator(self, pause_mock, resume_mock):
        @ittapi.paused_region
        def my_function():
            return 42

        self.assertEqual(my_function(), 42)
        resume_mock.assert_called_once()
        pause_mock.assert_called_once()

    @ittapi_native_patch('pause')
    @ittapi_native_patch('resume')
    def test_paused_region_as_context_manager(self, pause_mock, resume_mock):
        with ittapi.paused_region():
            pass

        resume_mock.assert_called_once()
        pause_mock.assert_called_once()

    @ittapi_native_patch('pause')
    @ittapi_native_patch('resume')
    def test_paused_region_with_manual_activation(self, pause_mock, resume_mock):
        region = ittapi.paused_region()

        region.activator.deactivate()
        with region:
            pass

        resume_mock.assert_not_called()
        pause_mock.assert_not_called()

        region.activator.activate()
        with region:
            pass

        resume_mock.assert_called_once()
        pause_mock.assert_called_once()

    @ittapi_native_patch('pause')
    @ittapi_native_patch('resume')
    def test_paused_region_with_custom_activator(self, pause_mock, resume_mock):
        for i in range(4):
            with ittapi.paused_region(activator=lambda: i % 2):  # pylint: disable=W0640
                pass

        self.assertEqual(resume_mock.call_count, 2)
        self.assertEqual(pause_mock.call_count, 2)

    @ittapi_native_patch('pause')
    @ittapi_native_patch('resume')
    def test_paused_region_as_decorator_without_activator(self, pause_mock, resume_mock):
        @ittapi.paused_region(activator=None)
        def my_function():
            return 42

        self.assertEqual(my_function(), 42)
        resume_mock.assert_called_once()
        pause_mock.assert_called_once()


if __name__ == '__main__':
    unittest_main()  # pragma: no cover
