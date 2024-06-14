from inspect import stack
from os.path import basename
from sys import version_info
from unittest import main as unittest_main, TestCase
from unittest.mock import call

from ittapi_native_mock import patch as ittapi_native_patch
import ittapi


class EventCreationTests(TestCase):
    @ittapi_native_patch('Event')
    @ittapi_native_patch('StringHandle')
    def test_event_creation_with_default_constructor(self, event_mock, string_handle_mock):
        string_handle_mock.side_effect = lambda x: x

        event = ittapi.event()
        caller = stack()[0]
        expected_name = f'{basename(caller.filename)}:{caller.lineno-1}'

        event_mock.assert_not_called()

        event.begin()

        event_mock.assert_called_once_with(expected_name)
        self.assertEqual(event.name(), expected_name)

    @ittapi_native_patch('Event')
    @ittapi_native_patch('StringHandle')
    def test_event_creation_as_decorator_for_function(self, event_mock, string_handle_mock):
        string_handle_mock.side_effect = lambda x: x

        @ittapi.event
        def my_function():
            pass  # pragma: no cover

        event_mock.assert_called_once_with(my_function.__qualname__)

    @ittapi_native_patch('Event')
    @ittapi_native_patch('StringHandle')
    def test_event_creation_as_decorator_with_empty_arguments_for_function(self, event_mock, string_handle_mock):
        string_handle_mock.side_effect = lambda x: x

        @ittapi.event()
        def my_function():
            pass  # pragma: no cover

        event_mock.assert_called_with(my_function.__qualname__)

    @ittapi_native_patch('Event')
    @ittapi_native_patch('StringHandle')
    def test_event_creation_as_decorator_with_name_for_function(self, event_mock, string_handle_mock):
        string_handle_mock.side_effect = lambda x: x

        @ittapi.event('my function')
        def my_function():
            pass  # pragma: no cover

        event_mock.assert_called_once_with('my function')

    @ittapi_native_patch('Event')
    @ittapi_native_patch('StringHandle')
    def test_event_creation_as_decorator_with_empty_args_and_name_for_function(self, event_mock, string_handle_mock):
        string_handle_mock.side_effect = lambda x: x

        @ittapi.event
        @ittapi.event('my function')
        def my_function():
            pass  # pragma: no cover

        expected_calls = [call('my function'),
                          call(my_function.__qualname__)]
        event_mock.assert_has_calls(expected_calls)

    @ittapi_native_patch('Event')
    @ittapi_native_patch('StringHandle')
    def test_event_creation_with_default_constructor_as_context_manager(self, event_mock, string_handle_mock):
        string_handle_mock.side_effect = lambda x: x

        caller = stack()[0]
        with ittapi.event():
            pass

        event_mock.assert_called_once_with(f'{basename(caller.filename)}:{caller.lineno+1}')

    @ittapi_native_patch('Event')
    @ittapi_native_patch('StringHandle')
    def test_event_creation_with_name_and_domain_as_context_manager(self, event_mock, string_handle_mock):
        string_handle_mock.side_effect = lambda x: x

        with ittapi.event('my event'):
            pass

        event_mock.assert_called_once_with('my event')

    @ittapi_native_patch('Event')
    @ittapi_native_patch('StringHandle')
    def test_event_creation_for_callable_object(self, event_mock, string_handle_mock):
        string_handle_mock.side_effect = lambda x: x

        class CallableClass:
            def __call__(self, *args, **kwargs):
                pass  # pragma: no cover

        event = ittapi.event(CallableClass())

        expected_name = f'{CallableClass.__name__}.__call__'
        event_mock.assert_called_once_with(expected_name)

        self.assertEqual(event.name(), expected_name)

    @ittapi_native_patch('Event')
    @ittapi_native_patch('StringHandle')
    def test_unnamed_event_creation_for_callable_object(self, event_mock, string_handle_mock):
        string_handle_mock.side_effect = lambda x: x

        class CallableClass:
            def __call__(self, *args, **kwargs):
                pass  # pragma: no cover

        event = ittapi.event()
        event(CallableClass())

        expected_name = f'{CallableClass.__name__}.__call__'
        event_mock.assert_called_once_with(expected_name)
        self.assertEqual(event.name(), expected_name)

    @ittapi_native_patch('Event')
    @ittapi_native_patch('StringHandle')
    def test_event_creation_for_method(self, event_mock, string_handle_mock):
        string_handle_mock.side_effect = lambda x: x

        class MyClass:
            @ittapi.event
            def my_method(self):
                pass  # pragma: no cover

        event_mock.assert_called_once_with(f'{MyClass.my_method.__qualname__}')


class EventPropertiesTest(TestCase):
    @ittapi_native_patch('Event')
    @ittapi_native_patch('StringHandle')
    def test_event_properties(self, event_mock, string_handle_mock):
        string_handle_mock.side_effect = lambda x: x

        class CallableClass:
            def __call__(self, *args, **kwargs):
                pass  # pragma: no cover

        event = ittapi.event(CallableClass())

        expected_name = f'{CallableClass.__name__}.__call__'
        event_mock.assert_called_once_with(expected_name)

        self.assertEqual(event.name(), expected_name)

        self.assertEqual(str(event), expected_name)
        self.assertEqual(repr(event), f'{event.__class__.__name__}(\'{expected_name}\')')


class EventExecutionTests(TestCase):
    @ittapi_native_patch('Event')
    @ittapi_native_patch('StringHandle')
    def test_event_for_function(self, event_mock, string_handle_mock):
        string_handle_mock.return_value = 'string_handle'

        @ittapi.event
        def my_function():
            return 42

        string_handle_mock.assert_called_once_with(my_function.__qualname__)
        event_mock.assert_called_once_with(string_handle_mock.return_value)

        self.assertEqual(my_function(), 42)

        expected_calls = [call().begin(),
                          call().end()]
        event_mock.assert_has_calls(expected_calls)

    @ittapi_native_patch('Event')
    @ittapi_native_patch('StringHandle')
    def test_nested_events_for_function(self, event_mock, string_handle_mock):
        string_handle_mock.side_effect = lambda x: x

        @ittapi.event
        @ittapi.event('my function')
        def my_function():
            return 42

        expected_calls = [call('my function'),
                          call(my_function.__qualname__)]
        string_handle_mock.assert_has_calls(expected_calls)
        event_mock.assert_has_calls(expected_calls)

        self.assertEqual(my_function(), 42)

        expected_calls = [call().begin(),
                          call().begin(),
                          call().end(),
                          call().end()]
        event_mock.assert_has_calls(expected_calls)

    @ittapi_native_patch('Event')
    @ittapi_native_patch('StringHandle')
    def test_event_as_context_manager(self, event_mock, string_handle_mock):
        string_handle_mock.side_effect = lambda x: x

        region_name = 'my region'
        with ittapi.event(region_name):
            pass

        string_handle_mock.assert_called_once_with(region_name)
        event_mock.assert_called_once_with(region_name)

        expected_calls = [call().begin(),
                          call().end()]
        event_mock.assert_has_calls(expected_calls)

    @ittapi_native_patch('Event')
    @ittapi_native_patch('StringHandle')
    def test_event_for_callable_object(self, event_mock, string_handle_mock):
        string_handle_mock.return_value = 'string_handle'

        class CallableClass:
            def __call__(self, *args, **kwargs):
                return 42

        callable_object = ittapi.event(CallableClass())
        string_handle_mock.assert_called_once_with(f'{CallableClass.__name__}.__call__')
        event_mock.assert_called_once_with(string_handle_mock.return_value)

        self.assertEqual(callable_object(), 42)

        expected_calls = [call().begin(),
                          call().end()]
        event_mock.assert_has_calls(expected_calls)

    def test_event_for_multiple_callable_objects(self):
        class CallableClass:
            def __call__(self, *args, **kwargs):
                pass  # pragma: no cover

        event = ittapi.event()
        event(CallableClass())

        with self.assertRaises(RuntimeError) as context:
            event(CallableClass())

        self.assertEqual(str(context.exception), 'A custom name for a code region must be specified before'
                                                 ' _NamedRegion.__call__() can be called more than once.')

    def test_event_for_noncallable_object(self):
        with self.assertRaises(TypeError) as context:
            ittapi.event()(42)

        self.assertEqual(str(context.exception), 'Callable object is expected as a first argument.')

    @ittapi_native_patch('Event')
    @ittapi_native_patch('StringHandle')
    def test_event_for_method(self, event_mock, string_handle_mock):
        string_handle_mock.side_effect = lambda x: x

        class MyClass:
            @ittapi.event
            def my_method(self):
                return 42

        string_handle_mock.assert_called_once_with(f'{MyClass.my_method.__qualname__}')
        event_mock.assert_called_once_with(f'{MyClass.my_method.__qualname__}')

        my_object = MyClass()
        self.assertEqual(my_object.my_method(), 42)

        expected_calls = [call().begin(),
                          call().end()]
        event_mock.assert_has_calls(expected_calls)

    @ittapi_native_patch('Event')
    @ittapi_native_patch('StringHandle')
    def test_event_for_class_method(self, event_mock, string_handle_mock):
        string_handle_mock.side_effect = lambda x: x

        class MyClass:
            @classmethod
            @ittapi.event
            def my_class_method(cls):
                return 42

        string_handle_mock.assert_called_once_with(f'{MyClass.my_class_method.__qualname__}')
        event_mock.assert_called_once_with(f'{MyClass.my_class_method.__qualname__}')

        self.assertEqual(MyClass.my_class_method(), 42)

        expected_calls = [call().begin(),
                          call().end()]
        event_mock.assert_has_calls(expected_calls)

    @ittapi_native_patch('Event')
    @ittapi_native_patch('StringHandle')
    def test_event_for_static_method(self, event_mock, string_handle_mock):
        string_handle_mock.side_effect = lambda x: x

        class MyClass:
            @staticmethod
            @ittapi.event
            def my_static_method():
                return 42

        string_handle_mock.assert_called_once_with(f'{MyClass.my_static_method.__qualname__}')
        event_mock.assert_called_once_with(f'{MyClass.my_static_method.__qualname__}')

        self.assertEqual(MyClass.my_static_method(), 42)
        self.assertEqual(MyClass().my_static_method(), 42)

        expected_calls = [call().begin(),
                          call().end(),
                          call().begin(),
                          call().end()]
        event_mock.assert_has_calls(expected_calls)

    @ittapi_native_patch('Event')
    @ittapi_native_patch('StringHandle')
    def test_event_for_static_method_with_wrong_order_of_decorators(self, event_mock, string_handle_mock):
        string_handle_mock.side_effect = lambda x: x

        class MyClass:
            @ittapi.event
            @staticmethod
            def my_static_method():
                return 42  # pragma: no cover

        if version_info >= (3, 10):
            string_handle_mock.assert_called_once_with(f'{MyClass.my_static_method.__qualname__}')
            event_mock.assert_called_once_with(f'{MyClass.my_static_method.__qualname__}')

            self.assertEqual(MyClass().my_static_method(), 42)
            self.assertEqual(MyClass.my_static_method(), 42)

            expected_calls = [call().begin(),
                              call().end(),
                              call().begin(),
                              call().end()]
            event_mock.assert_has_calls(expected_calls)
        else:
            # @staticmethod decorator returns a descriptor which is not callable before Python 3.10
            # therefore, it cannot be traced. @staticmethod have to be always above ittapi decorators for Python 3.9 or
            # older. otherwise, the exception is thrown.
            with self.assertRaises(TypeError) as context:
                MyClass().my_static_method()

            self.assertEqual(str(context.exception), 'Callable object is expected to be passed.')

    def test_event_for_class_method_with_wrong_order_of_decorators(self):
        # @classmethod decorator returns a descriptor and the descriptor is not callable object,
        # therefore, it cannot be traced. @classmethod have to be always above ittapi decorators,
        # otherwise, the exception is thrown.
        class MyClass:
            @ittapi.event
            @classmethod
            def my_class_method(cls):
                return 42  # pragma: no cover

        with self.assertRaises(TypeError) as context:
            MyClass().my_class_method()

        self.assertEqual(str(context.exception), 'Callable object is expected to be passed.')

        with self.assertRaises(TypeError) as context:
            MyClass.my_class_method()

        self.assertEqual(str(context.exception), 'Callable object is expected to be passed.')

    @ittapi_native_patch('Event')
    @ittapi_native_patch('StringHandle')
    def test_event_for_function_raised_exception(self, event_mock, string_handle_mock):
        string_handle_mock.return_value = 'string_handle'

        exception_msg = 'ValueError exception from my_function'

        @ittapi.event
        def my_function():
            raise ValueError(exception_msg)

        string_handle_mock.assert_called_once_with(my_function.__qualname__)
        event_mock.assert_called_once_with(string_handle_mock.return_value)

        with self.assertRaises(ValueError) as context:
            my_function()

        self.assertEqual(str(context.exception), exception_msg)

        expected_calls = [call().begin(),
                          call().end()]
        event_mock.assert_has_calls(expected_calls)

    @ittapi_native_patch('Event')
    @ittapi_native_patch('StringHandle')
    def test_event_for_method_raised_exception(self, event_mock, string_handle_mock):
        string_handle_mock.side_effect = lambda x: x

        exception_msg = 'ValueError exception from my_method'

        class MyClass:
            @ittapi.event
            def my_method(self):
                raise ValueError(exception_msg)

        string_handle_mock.assert_called_once_with(f'{MyClass.my_method.__qualname__}')
        event_mock.assert_called_once_with(f'{MyClass.my_method.__qualname__}')

        with self.assertRaises(ValueError) as context:
            MyClass().my_method()

        self.assertEqual(str(context.exception), exception_msg)

        expected_calls = [call().begin(),
                          call().end()]
        event_mock.assert_has_calls(expected_calls)


if __name__ == '__main__':
    unittest_main()  # pragma: no cover
