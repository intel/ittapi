"""
Python binding to Intel Instrumentation and Tracing Technology (ITT) API.

This module provides a convenient way to mark up the Python code for further performance analysis using performance
analyzers from Intel like Intel VTune or others.
"""

from ittapi.native import Domain, Id, StringHandle
from ittapi.native import task_begin, task_end, task_begin_overlapped, task_end_overlapped
from .collection_control import detach, pause, resume, active_region, paused_region, ActiveRegion, PausedRegion
from .event import event, Event
from .domain import domain
from .id import id
from .string_handle import string_handle
from .task import NestedTask, OverlappedTask, task, nested_task, overlapped_task
from .thread_naming import thread_set_name
from .pt_region import pt_region
