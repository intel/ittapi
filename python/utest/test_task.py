from inspect import stack
from os.path import basename
from sys import version_info
from unittest import main as unittest_main, TestCase
from unittest.mock import call

from ittapi_native_mock import patch as ittapi_native_patch
import ittapi


class TaskCreationTests(TestCase):
    @ittapi_native_patch('Domain')
    @ittapi_native_patch('StringHandle')
    @ittapi_native_patch('Id')
    def test_task_creation_with_default_constructor(self, domain_mock, string_handle_mock, id_mock):
        domain_mock.return_value = 'ittapi'
        string_handle_mock.side_effect = lambda x: x
        id_mock.return_value = 1

        task = ittapi.task()
        caller = stack()[0]
        expected_name = f'{basename(caller.filename)}:{caller.lineno-1}'

        string_handle_mock.assert_called_once_with(expected_name)
        domain_mock.assert_called_once_with(None)

        self.assertEqual(task.name(), expected_name)
        self.assertEqual(task.domain(), domain_mock.return_value)
        self.assertEqual(task.id(), id_mock.return_value)
        self.assertIsNone(task.parent_id())

    @ittapi_native_patch('StringHandle')
    def test_task_creation_as_decorator_for_function(self, string_handle_mock):
        @ittapi.task
        def my_function():
            pass  # pragma: no cover

        string_handle_mock.assert_called_once_with(my_function.__qualname__)

    @ittapi_native_patch('StringHandle')
    def test_task_creation_as_decorator_with_empty_arguments_for_function(self, string_handle_mock):
        @ittapi.task()
        def my_function():
            pass  # pragma: no cover

        string_handle_mock.assert_called_with(my_function.__qualname__)

    @ittapi_native_patch('StringHandle')
    def test_task_creation_as_decorator_with_name_for_function(self, string_handle_mock):
        @ittapi.task('my function')
        def my_function():
            pass  # pragma: no cover

        string_handle_mock.assert_called_once_with('my function')

    @ittapi_native_patch('Domain')
    def test_task_creation_as_decorator_with_domain_for_function(self, domain_mock):
        @ittapi.task(domain='my domain')
        def my_function():
            pass  # pragma: no cover

        domain_mock.assert_called_once_with('my domain')

    @ittapi_native_patch('StringHandle')
    def test_task_creation_as_decorator_with_empty_args_and_name_for_function(self, string_handle_mock):
        @ittapi.task
        @ittapi.task('my function')
        def my_function():
            pass  # pragma: no cover

        expected_calls = [call('my function'),
                          call(my_function.__qualname__)]
        string_handle_mock.assert_has_calls(expected_calls)

    @ittapi_native_patch('Domain')
    @ittapi_native_patch('StringHandle')
    def test_task_creation_with_default_constructor_as_context_manager(self, domain_mock, string_handle_mock):
        caller = stack()[0]
        with ittapi.task():
            pass

        string_handle_mock.assert_called_once_with(f'{basename(caller.filename)}:{caller.lineno+1}')
        domain_mock.assert_called_once_with(None)

    @ittapi_native_patch('Domain')
    @ittapi_native_patch('StringHandle')
    def test_task_creation_with_name_and_domain_as_context_manager(self, domain_mock, string_handle_mock):
        with ittapi.task('my task', 'my domain'):
            pass

        string_handle_mock.assert_called_once_with('my task')
        domain_mock.assert_called_once_with('my domain')

    @ittapi_native_patch('Domain')
    @ittapi_native_patch('Id')
    @ittapi_native_patch('StringHandle')
    def test_task_creation_for_callable_object(self, domain_mock, id_mock, string_handle_mock):
        domain_mock.return_value = 'domain'
        string_handle_mock.side_effect = lambda x: x
        id_mock.return_value = 1

        class CallableClass:
            def __call__(self, *args, **kwargs):
                pass  # pragma: no cover

        task = ittapi.task(CallableClass())

        expected_name = f'{CallableClass.__name__}.__call__'
        string_handle_mock.assert_called_once_with(expected_name)

        self.assertEqual(task.name(), expected_name)
        self.assertEqual(task.domain(), domain_mock.return_value)
        self.assertEqual(task.id(), id_mock.return_value)
        self.assertIsNone(task.parent_id())

    @ittapi_native_patch('Domain')
    @ittapi_native_patch('Id')
    @ittapi_native_patch('StringHandle')
    def test_unnamed_task_creation_for_callable_object(self, domain_mock, id_mock, string_handle_mock):
        domain_mock.return_value = 'domain'
        string_handle_mock.side_effect = lambda x: x
        id_mock.return_value = 1

        class CallableClass:
            def __call__(self, *args, **kwargs):
                pass  # pragma: no cover

        caller = stack()[0]
        task = ittapi.task()
        task(CallableClass())

        expected_name = f'{CallableClass.__name__}.__call__'
        expected_calls = [
            call(f'{basename(caller.filename)}:{caller.lineno+1}'),
            call(expected_name)
        ]
        string_handle_mock.assert_has_calls(expected_calls)

        self.assertEqual(task.name(), expected_name)
        self.assertEqual(task.domain(), domain_mock.return_value)
        self.assertEqual(task.id(), id_mock.return_value)
        self.assertIsNone(task.parent_id())

    @ittapi_native_patch('StringHandle')
    def test_task_creation_for_method(self, string_handle_mock):
        class MyClass:
            @ittapi.task
            def my_method(self):
                pass  # pragma: no cover

        string_handle_mock.assert_called_once_with(f'{MyClass.my_method.__qualname__}')


class TaskPropertiesTest(TestCase):
    @ittapi_native_patch('Domain')
    @ittapi_native_patch('Id')
    @ittapi_native_patch('StringHandle')
    def test_task_properties(self, domain_mock, id_mock, string_handle_mock):
        domain_mock.side_effect = lambda x: x
        string_handle_mock.side_effect = lambda x: x
        id_mock.return_value = lambda x: x

        class CallableClass:
            def __call__(self, *args, **kwargs):
                pass  # pragma: no cover

        domain_name = 'my domain'
        task_id = 2
        parent_id = 1
        task = ittapi.task(CallableClass(), domain=domain_name, id=task_id, parent=parent_id)

        expected_name = f'{CallableClass.__name__}.__call__'
        string_handle_mock.assert_called_once_with(expected_name)

        self.assertEqual(task.name(), expected_name)
        self.assertEqual(task.domain(), domain_name)
        self.assertEqual(task.id(), task_id)
        self.assertEqual(task.parent_id(), parent_id)

        self.assertEqual(str(task), f"{{ name: '{str(expected_name)}', domain: '{str(domain_name)}',"
                                    f" id: {str(task_id)}, parent_id: {str(parent_id)} }}")

        self.assertEqual(repr(task), f'{task.__class__.__name__}({repr(expected_name)}, {repr(domain_name)},'
                                     f' {repr(task_id)}, {repr(parent_id)})')


class TaskExecutionTests(TestCase):
    @ittapi_native_patch('Domain')
    @ittapi_native_patch('Id')
    @ittapi_native_patch('StringHandle')
    @ittapi_native_patch('task_begin')
    @ittapi_native_patch('task_end')
    def test_task_for_function(self, domain_mock, id_mock, string_handle_mock, task_begin_mock, task_end_mock):
        domain_mock.return_value = 'domain_handle'
        string_handle_mock.return_value = 'string_handle'
        id_mock.return_value = 'id_handle'

        @ittapi.task
        def my_function():
            return 42

        domain_mock.assert_called_once_with(None)
        string_handle_mock.assert_called_once_with(my_function.__qualname__)
        id_mock.assert_called_once_with(domain_mock.return_value)

        self.assertEqual(my_function(), 42)

        task_begin_mock.assert_called_once_with(domain_mock.return_value, string_handle_mock.return_value,
                                                id_mock.return_value, None)
        task_end_mock.assert_called_once_with(domain_mock.return_value)

    @ittapi_native_patch('Domain')
    @ittapi_native_patch('Id')
    @ittapi_native_patch('StringHandle')
    @ittapi_native_patch('task_begin')
    @ittapi_native_patch('task_end')
    def test_nested_tasks_for_function(self, domain_mock, id_mock, string_handle_mock, task_begin_mock, task_end_mock):
        domain_mock.return_value = 'domain_handle'
        string_handle_mock.side_effect = lambda x: x
        id_mock.return_value = 'id_handle'

        @ittapi.task
        @ittapi.task('my function')
        def my_function():
            return 42

        expected_calls = [call('my function'),
                          call(my_function.__qualname__)]
        string_handle_mock.assert_has_calls(expected_calls)

        self.assertEqual(my_function(), 42)

        expected_calls = [call(domain_mock.return_value, my_function.__qualname__, id_mock.return_value, None),
                          call(domain_mock.return_value, 'my function', id_mock.return_value, None)]
        task_begin_mock.assert_has_calls(expected_calls)

        expected_calls = [call(domain_mock.return_value),
                          call(domain_mock.return_value)]
        task_end_mock.assert_has_calls(expected_calls)

    @ittapi_native_patch('Domain')
    @ittapi_native_patch('Id')
    @ittapi_native_patch('StringHandle')
    @ittapi_native_patch('task_begin')
    @ittapi_native_patch('task_end')
    def test_task_as_context_manager(self, domain_mock, id_mock, string_handle_mock, task_begin_mock, task_end_mock):
        domain_mock.return_value = 'domain_handle'
        string_handle_mock.side_effect = lambda x: x
        id_mock.return_value = 'id_handle'

        region_name = 'my region'
        with ittapi.task(region_name):
            pass

        domain_mock.assert_called_once_with(None)
        string_handle_mock.assert_called_once_with(region_name)
        id_mock.assert_called_once_with(domain_mock.return_value)

        task_begin_mock.assert_called_once_with(domain_mock.return_value, region_name, id_mock.return_value, None)
        task_end_mock.assert_called_once_with(domain_mock.return_value)

    @ittapi_native_patch('Domain')
    @ittapi_native_patch('Id')
    @ittapi_native_patch('StringHandle')
    @ittapi_native_patch('task_begin')
    @ittapi_native_patch('task_end')
    def test_task_for_callable_object(self, domain_mock, id_mock, string_handle_mock, task_begin_mock, task_end_mock):
        domain_mock.return_value = 'domain_handle'
        string_handle_mock.return_value = 'string_handle'
        id_mock.return_value = 'id_handle'

        class CallableClass:
            def __call__(self, *args, **kwargs):
                return 42

        callable_object = ittapi.task(CallableClass())
        string_handle_mock.assert_called_once_with(f'{CallableClass.__name__}.__call__')

        self.assertEqual(callable_object(), 42)

        task_begin_mock.assert_called_once_with(domain_mock.return_value, string_handle_mock.return_value,
                                                id_mock.return_value, None)
        task_end_mock.assert_called_once_with(domain_mock.return_value)

    def test_task_for_multiple_callable_objects(self):
        class CallableClass:
            def __call__(self, *args, **kwargs):
                pass

        task = ittapi.task()
        task(CallableClass())

        with self.assertRaises(RuntimeError) as context:
            task(CallableClass())

        self.assertEqual(str(context.exception), 'A custom name for a code region must be specified before'
                                                 ' _NamedRegion.__call__() can be called more than once.')

    def test_task_for_noncallable_object(self):
        with self.assertRaises(TypeError) as context:
            ittapi.task()(42)

        self.assertEqual(str(context.exception), 'Callable object is expected as a first argument.')

    @ittapi_native_patch('Domain')
    @ittapi_native_patch('Id')
    @ittapi_native_patch('StringHandle')
    @ittapi_native_patch('task_begin')
    @ittapi_native_patch('task_end')
    def test_task_for_method(self, domain_mock, id_mock, string_handle_mock, task_begin_mock, task_end_mock):
        domain_mock.return_value = 'domain_handle'
        string_handle_mock.side_effect = lambda x: x
        id_mock.return_value = 'id_handle'

        class MyClass:
            @ittapi.task
            def my_method(self):
                return 42

        string_handle_mock.assert_called_once_with(f'{MyClass.my_method.__qualname__}')

        my_object = MyClass()
        self.assertEqual(my_object.my_method(), 42)

        task_begin_mock.assert_called_once_with(domain_mock.return_value, f'{MyClass.my_method.__qualname__}',
                                                id_mock.return_value, None)
        task_end_mock.assert_called_once_with(domain_mock.return_value)

    @ittapi_native_patch('Domain')
    @ittapi_native_patch('Id')
    @ittapi_native_patch('StringHandle')
    @ittapi_native_patch('task_begin')
    @ittapi_native_patch('task_end')
    def test_task_for_class_method(self, domain_mock, id_mock, string_handle_mock, task_begin_mock, task_end_mock):
        domain_mock.return_value = 'domain_handle'
        string_handle_mock.side_effect = lambda x: x
        id_mock.return_value = 'id_handle'

        class MyClass:
            @classmethod
            @ittapi.task
            def my_class_method(cls):
                return 42

        string_handle_mock.assert_called_once_with(f'{MyClass.my_class_method.__qualname__}')

        self.assertEqual(MyClass.my_class_method(), 42)

        task_begin_mock.assert_called_once_with(domain_mock.return_value, f'{MyClass.my_class_method.__qualname__}',
                                                id_mock.return_value, None)
        task_end_mock.assert_called_once_with(domain_mock.return_value)

    @ittapi_native_patch('Domain')
    @ittapi_native_patch('Id')
    @ittapi_native_patch('StringHandle')
    @ittapi_native_patch('task_begin')
    @ittapi_native_patch('task_end')
    def test_task_for_static_method(self, domain_mock, id_mock, string_handle_mock, task_begin_mock, task_end_mock):
        domain_mock.return_value = 'domain_handle'
        string_handle_mock.side_effect = lambda x: x
        id_mock.return_value = 'id_handle'

        class MyClass:
            @staticmethod
            @ittapi.task
            def my_static_method():
                return 42

        string_handle_mock.assert_called_once_with(f'{MyClass.my_static_method.__qualname__}')

        self.assertEqual(MyClass.my_static_method(), 42)

        task_begin_mock.assert_called_once_with(domain_mock.return_value, f'{MyClass.my_static_method.__qualname__}',
                                                id_mock.return_value, None)
        task_end_mock.assert_called_once_with(domain_mock.return_value)

        task_begin_mock.reset_mock()
        task_end_mock.reset_mock()

        self.assertEqual(MyClass().my_static_method(), 42)

        task_begin_mock.assert_called_once_with(domain_mock.return_value, f'{MyClass.my_static_method.__qualname__}',
                                                id_mock.return_value, None)
        task_end_mock.assert_called_once_with(domain_mock.return_value)

    @ittapi_native_patch('Domain')
    @ittapi_native_patch('Id')
    @ittapi_native_patch('StringHandle')
    @ittapi_native_patch('task_begin')
    @ittapi_native_patch('task_end')
    def test_task_for_static_method_with_wrong_order_of_decorators(self, domain_mock, id_mock, string_handle_mock,
                                                                   task_begin_mock, task_end_mock):
        domain_mock.return_value = 'domain_handle'
        string_handle_mock.side_effect = lambda x: x
        id_mock.return_value = 'id_handle'

        class MyClass:
            @ittapi.task
            @staticmethod
            def my_static_method():
                return 42  # pragma: no cover

        if version_info >= (3, 10):
            string_handle_mock.assert_called_once_with(f'{MyClass.my_static_method.__qualname__}')

            self.assertEqual(MyClass().my_static_method(), 42)

            task_begin_mock.assert_called_once_with(domain_mock.return_value,
                                                    f'{MyClass.my_static_method.__qualname__}',
                                                    id_mock.return_value, None)
            task_end_mock.assert_called_once_with(domain_mock.return_value)

            task_begin_mock.reset_mock()
            task_end_mock.reset_mock()

            self.assertEqual(MyClass.my_static_method(), 42)

            task_begin_mock.assert_called_once_with(domain_mock.return_value,
                                                    f'{MyClass.my_static_method.__qualname__}',
                                                    id_mock.return_value, None)
            task_end_mock.assert_called_once_with(domain_mock.return_value)
        else:
            # @staticmethod decorator returns a descriptor which is not callable before Python 3.10
            # therefore, it cannot be traced. @staticmethod have to be always above ittapi decorators for Python 3.9 or
            # older. otherwise, the exception is thrown.
            with self.assertRaises(TypeError) as context:
                MyClass().my_static_method()

            self.assertEqual(str(context.exception), 'Callable object is expected to be passed.')

    def test_task_for_class_method_with_wrong_order_of_decorators(self):
        # @classmethod decorator returns a descriptor and the descriptor is not callable object,
        # therefore, it cannot be traced. @classmethod have to be always above ittapi decorators,
        # otherwise, the exception is thrown.
        class MyClass:
            @ittapi.task
            @classmethod
            def my_class_method(cls):
                return 42  # pragma: no cover

        with self.assertRaises(TypeError) as context:
            MyClass().my_class_method()

        self.assertEqual(str(context.exception), 'Callable object is expected to be passed.')

        with self.assertRaises(TypeError) as context:
            MyClass.my_class_method()

        self.assertEqual(str(context.exception), 'Callable object is expected to be passed.')

    @ittapi_native_patch('Domain')
    @ittapi_native_patch('Id')
    @ittapi_native_patch('StringHandle')
    @ittapi_native_patch('task_begin')
    @ittapi_native_patch('task_end')
    def test_task_for_function_raised_exception(self, domain_mock, id_mock, string_handle_mock,
                                                task_begin_mock, task_end_mock):
        domain_mock.return_value = 'domain_handle'
        string_handle_mock.return_value = 'string_handle'
        id_mock.return_value = 'id_handle'

        exception_msg = 'ValueError exception from my_function'

        @ittapi.task
        def my_function():
            raise ValueError(exception_msg)

        domain_mock.assert_called_once_with(None)
        string_handle_mock.assert_called_once_with(my_function.__qualname__)
        id_mock.assert_called_once_with(domain_mock.return_value)

        with self.assertRaises(ValueError) as context:
            my_function()

        self.assertEqual(str(context.exception), exception_msg)

        task_begin_mock.assert_called_once_with(domain_mock.return_value, string_handle_mock.return_value,
                                                id_mock.return_value, None)
        task_end_mock.assert_called_once_with(domain_mock.return_value)

    @ittapi_native_patch('Domain')
    @ittapi_native_patch('Id')
    @ittapi_native_patch('StringHandle')
    @ittapi_native_patch('task_begin')
    @ittapi_native_patch('task_end')
    def test_task_for_method_raised_exception(self, domain_mock, id_mock, string_handle_mock,
                                              task_begin_mock, task_end_mock):
        domain_mock.return_value = 'domain_handle'
        string_handle_mock.side_effect = lambda x: x
        id_mock.return_value = 'id_handle'

        exception_msg = 'ValueError exception from my_method'

        class MyClass:
            @ittapi.task
            def my_method(self):
                raise ValueError(exception_msg)

        string_handle_mock.assert_called_once_with(f'{MyClass.my_method.__qualname__}')

        with self.assertRaises(ValueError) as context:
            MyClass().my_method()

        self.assertEqual(str(context.exception), exception_msg)

        task_begin_mock.assert_called_once_with(domain_mock.return_value, f'{MyClass.my_method.__qualname__}',
                                                id_mock.return_value, None)
        task_end_mock.assert_called_once_with(domain_mock.return_value)

    @ittapi_native_patch('Domain')
    @ittapi_native_patch('Id')
    @ittapi_native_patch('StringHandle')
    @ittapi_native_patch('task_begin_overlapped')
    @ittapi_native_patch('task_end_overlapped')
    def test_overlapped_tasks(self, domain_mock, id_mock, string_handle_mock,
                              task_begin_overlapped_mock, task_end_overlapped_mock):
        domain_mock.return_value = 'domain_handle'
        string_handle_mock.side_effect = lambda x: x

        id_value = 0

        def id_generator(*args, **kwargs):  # pylint: disable=W0613
            nonlocal id_value
            id_value += 1
            return id_value

        id_mock.side_effect = id_generator

        overlapped_task_1_name = 'overlapped task 1'
        overlapped_task_2_name = 'overlapped task 2'

        overlapped_task_1 = ittapi.task(overlapped_task_1_name, overlapped=True)
        overlapped_task_1.begin()

        overlapped_task_2 = ittapi.task(overlapped_task_2_name, overlapped=True)
        overlapped_task_2.begin()

        overlapped_task_1.end()
        overlapped_task_2.end()

        expected_calls = [
            call(overlapped_task_1_name),
            call(overlapped_task_2_name)
        ]
        string_handle_mock.assert_has_calls(expected_calls)

        expected_calls = [
            call(domain_mock.return_value, overlapped_task_1_name, 1, None),
            call(domain_mock.return_value, overlapped_task_2_name, 2, None)
        ]
        task_begin_overlapped_mock.assert_has_calls(expected_calls)

        expected_calls = [
            call(domain_mock.return_value, 1),
            call(domain_mock.return_value, 2)
        ]
        task_end_overlapped_mock.assert_has_calls(expected_calls)


class NestedTaskCreationTests(TestCase):
    @ittapi_native_patch('Domain')
    @ittapi_native_patch('StringHandle')
    def test_task_creation_with_default_constructor(self, domain_mock, string_handle_mock):
        ittapi.nested_task()
        caller = stack()[0]
        string_handle_mock.assert_called_once_with(f'{basename(caller.filename)}:{caller.lineno-1}')
        domain_mock.assert_called_once_with(None)


class OverlappedTaskCreationTests(TestCase):
    @ittapi_native_patch('Domain')
    @ittapi_native_patch('StringHandle')
    def test_task_creation_with_default_constructor(self, domain_mock, string_handle_mock):
        ittapi.overlapped_task()
        caller = stack()[0]
        string_handle_mock.assert_called_once_with(f'{basename(caller.filename)}:{caller.lineno-1}')
        domain_mock.assert_called_once_with(None)


if __name__ == '__main__':
    unittest_main()  # pragma: no cover
