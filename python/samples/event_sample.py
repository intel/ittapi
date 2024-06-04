#!/usr/bin/env python
import ittapi

# pylint: disable=C0411
from argparse import ArgumentParser
from vtune_tool import run_vtune_hotspot_collection
from workload import workload


def run_sample():
    # ittapi.event can be used as decorator
    @ittapi.event
    def my_function_1():
        workload()

    # the list of arguments can be empty
    @ittapi.event()
    def my_function_2():
        workload()

    # or you can specify the name of the event and other parameters
    @ittapi.event('my function 3')
    def my_function_3():
        workload()

    @ittapi.event
    @ittapi.event('my function 4')
    def my_function_4():
        workload()

    # also, ittapi.event can be used as a context manager
    with ittapi.event():
        workload()
    # in this form you also can specify the name, the domain and other parameters in the same way
    with ittapi.event('my event'):
        workload()

    my_function_1()
    my_function_2()
    my_function_3()
    my_function_4()

    # example for overlapped events
    overlapped_event_1 = ittapi.event('overlapped event 1')
    overlapped_event_1.begin()
    workload()
    overlapped_event_2 = ittapi.event('overlapped event 2')
    overlapped_event_2.begin()
    workload()
    overlapped_event_1.end()
    workload()
    overlapped_event_2.end()

    # example with callable object
    class CallableClass:
        def __call__(self, *args, **kwargs):  # pylint: disable=W0621
            workload()

    callable_object = ittapi.event(CallableClass())
    callable_object()


# pylint: disable=R0801
if __name__ == '__main__':
    parser = ArgumentParser(description='The sample that demonstrates the use of wrappers for the Event API.')
    parser.add_argument('--run-sample',
                        help='Runs code that uses wrappers for the Event API.',
                        action='store_true')
    args = parser.parse_args()

    if args.run_sample:
        run_sample()
    else:
        run_vtune_hotspot_collection(['python', __file__, '--run-sample'])
