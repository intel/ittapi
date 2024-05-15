#!/usr/bin/env python
import pyitt

# pylint: disable=C0411
from argparse import ArgumentParser
from vtune_tool import run_vtune_hotspot_collection
from workload import workload


def run_sample():
    # pyitt.task can be used as decorator
    @pyitt.task
    def my_function_1():
        workload()

    # the list of arguments can be empty
    @pyitt.task()
    def my_function_2():
        workload()

    # or you can specify the name of the task and other parameters
    @pyitt.task('my function 3')
    def my_function_3():
        workload()

    # like domain
    @pyitt.task(domain='my domain')
    def my_function_4():
        workload()

    @pyitt.task
    @pyitt.task('my function 5')
    def my_function_5():
        workload()

    # also, pyitt.task can be used as a context manager
    with pyitt.task():
        workload()
    # in this form you also can specify the name, the domain and other parameters in the same way
    with pyitt.task('my task', 'my domain'):
        workload()

    my_function_1()
    my_function_2()
    my_function_3()
    my_function_4()
    my_function_5()

    # example for overlapped tasks
    overlapped_task_1 = pyitt.task('overlapped task 1', overlapped=True)
    overlapped_task_1.begin()
    workload()
    overlapped_task_2 = pyitt.task('overlapped task 2', overlapped=True)
    overlapped_task_2.begin()
    workload()
    overlapped_task_1.end()
    workload()
    overlapped_task_2.end()

    # example with callable object
    class CallableClass:
        def __call__(self, *args, **kwargs):  # pylint: disable=W0621
            workload()

    callable_object = pyitt.task(CallableClass())
    callable_object()


# pylint: disable=R0801
if __name__ == '__main__':
    parser = ArgumentParser(description='The sample that demonstrates the use of wrappers for the Task API.')
    parser.add_argument('--run-sample',
                        help='Runs code that uses wrappers for the Task API.',
                        action='store_true')
    args = parser.parse_args()

    if args.run_sample:
        run_sample()
    else:
        run_vtune_hotspot_collection(['python', __file__, '--run-sample'])
