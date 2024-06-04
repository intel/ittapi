"""
task.py - Python module wrapper for ITT Task API
"""
from ittapi.native import task_begin as _task_begin, task_end as _task_end
from ittapi.native import task_begin_overlapped as _task_begin_overlapped, task_end_overlapped as _task_end_overlapped

from .domain import domain as _domain
from .id import id as _id
from .region import _CallSite, _NamedRegion


class _Task(_NamedRegion):
    """
    An abstract base class that provides common functionality for subtypes that represent ITT Tasks.
    """
    def __init__(self, task=None, domain=None, id=None, parent=None) -> None:
        """
        Creates the instance of the class that represents ITT task.
        :param task: a name of the task or a callable object (e.g. function) to wrap. If the callable object is passed
                     the name of this object is used as a name for the task.
        :param domain: a task domain
        :param id: a task id
        :param parent: a parent task or an id of the parent
        """
        super().__init__(task)

        self._domain = self.__get_task_domain(domain)
        self._id = self.__get_task_id(id, self._domain)
        self._parent_id = self.__get_parent_id(parent)

    def __str__(self) -> str:
        return (f"{{ name: '{str(self._name)}', domain: '{str(self._domain)}',"
                f" id: {str(self._id)}, parent_id: {str(self._parent_id)} }}")

    def __repr__(self) -> str:
        return (f'{self.__class__.__name__}({repr(self._name)}, {repr(self._domain)},'
                f' {repr(self._id)}, {repr(self._parent_id)})')

    def domain(self):
        """Returns the domain of the task."""
        return self._domain

    def id(self):
        """Returns the id of the task."""
        return self._id

    def parent_id(self):
        """Returns the parent id for the task."""
        return self._parent_id

    def begin(self) -> None:
        """Marks the beginning of a task."""
        raise NotImplementedError()

    def end(self) -> None:
        """Marks the end of a task."""
        raise NotImplementedError()

    @staticmethod
    def __get_task_domain(original_domain):
        """Returns task domain"""
        if original_domain is None or isinstance(original_domain, str):
            return _domain(original_domain)

        return original_domain

    @staticmethod
    def __get_task_id(original_id, domain):
        """Returns task id for specified domain"""
        return _id(domain) if original_id is None else original_id

    @staticmethod
    def __get_parent_id(original_parent):
        """Returns parent id"""
        return original_parent.id() if isinstance(original_parent, task.__class__) else original_parent


class NestedTask(_Task):
    """
    A class that represents nested tasks.

    Nested tasks implicitly support a concept of embedded execution. This means that the call end() finalizes the
    most recent begin() call of the same or another nested task.
    """
    def begin(self) -> None:
        """Marks the beginning of a task."""
        _task_begin(self._domain, self._name, self._id, self._parent_id)

    def end(self) -> None:
        """Marks the end of a task."""
        _task_end(self._domain)


def nested_task(task=None, domain=None, id=None, parent=None):
    """
    Creates a nested task instance with the given arguments.
    :param task: a name of the task or a callable object
    :param domain: a task domain
    :param id: a task id
    :param parent: a parent task or an id of the parent
    :return: an instance of NestedTask
    """
    task = _CallSite(_CallSite.CallerFrame) if task is None else task
    return NestedTask(task, domain, id, parent)


class OverlappedTask(_Task):
    """
    A class that represents overlapped tasks.

    Execution regions of overlapped tasks may intersect.
    """
    def begin(self) -> None:
        """Marks the beginning of a task."""
        _task_begin_overlapped(self._domain, self._name, self._id, self._parent_id)

    def end(self) -> None:
        """Marks the end of a task."""
        _task_end_overlapped(self._domain, self._id)


def overlapped_task(task=None, domain=None, id=None, parent=None):
    """
    Creates an overlapped task instance with the given arguments.
    :param task: a name of the task or a callable object
    :param domain: a task domain
    :param id: a task id
    :param parent: a parent task or an id of the parent
    :return: an instance of OverlappedTask
    """
    task = _CallSite(_CallSite.CallerFrame) if task is None else task
    return OverlappedTask(task, domain, id, parent)


def task(task=None, domain=None, id=None, parent=None, overlapped=False):
    """
    Creates a task instance with the given arguments.
    :param task: a name of the task or a callable object
    :param domain: a task domain
    :param id: a task id
    :param parent: a parent task or an id of the parent
    :param overlapped: determines if the created task should be an instance of OverlappedTask class
                       or NestedTask class
    :return: a task instance
    """
    task = _CallSite(_CallSite.CallerFrame) if task is None else task
    return OverlappedTask(task, domain, id, parent) if overlapped else NestedTask(task, domain, id, parent)
