#!/usr/bin/env python
from ittapi import itt_NERSC

# pylint: disable=C0411
from argparse import ArgumentParser
from vtune_tool import run_vtune_hotspot_collection
from workload import workload


domain = itt_NERSC.domain_create("collection_control_sample")

def run_sample():
    itt_NERSC.resume()
    def run_workload():
        itt_NERSC.task_begin(domain, "run_workload")
        workload()
        itt_NERSC.task_end(domain)

    run_workload()

    for i in range(4):
        if i%2 == 0:
            itt_NERSC.task_begin(domain, f"for loop iteration {i}")
            workload()
            itt_NERSC.task_end(domain)

    itt_NERSC.pause()

    itt_NERSC.task_begin(domain, "resumed region")
    workload()
    itt_NERSC.task_end(domain)


    itt_NERSC.resume()
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
