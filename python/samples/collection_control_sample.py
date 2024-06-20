#!/usr/bin/env python
import ittapi

# pylint: disable=C0411
from argparse import ArgumentParser
from vtune_tool import run_vtune_hotspot_collection
from workload import workload


def run_sample():
    @ittapi.active_region
    @ittapi.task
    def run_workload():
        workload()

    run_workload()

    for i in range(4):
        with ittapi.active_region(activator=lambda: i % 2):  # pylint: disable=W0640
            with ittapi.task(f'for loop iteration {i}'):
                workload()

    ittapi.collection_control.resume()

    with ittapi.task('resumed region'):
        workload()

    with ittapi.paused_region():
        with ittapi.task('paused region'):
            workload()


# pylint: disable=R0801
if __name__ == '__main__':
    parser = ArgumentParser(
        description='The sample that demonstrates the use of wrappers for the Collection Control API.'
    )
    parser.add_argument('--run-sample',
                        help='Runs code that uses wrappers for Collection Control API.',
                        action='store_true')
    args = parser.parse_args()

    if args.run_sample:
        run_sample()
    else:
        run_vtune_hotspot_collection(['python', __file__, '--run-sample'],
                                     ['-start-paused'])
