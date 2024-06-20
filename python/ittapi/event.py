"""
event.py - Python module wrapper for ITT Event API
"""
from functools import partial as _partial

from ittapi.native import Event as _Event

from .region import _CallSite, _NamedRegion


class Event(_NamedRegion):
    """
    A class that represents Event.
    """
    def __init__(self, region=None):
        """
        Creates the instance of the class that represents ITT Event.
        :param region: a name of the event or a callable object (e.g. function) to wrap. If the callable object is
                       passed the name of this object is used as a name for the event.
        """
        self._event = None

        super().__init__(region, _partial(Event.__deferred_event_creation, self))

    def __deferred_event_creation(self, name) -> None:
        """Performs deferred creation of native Event."""
        self._event = _Event(name)

    def begin(self) -> None:
        """Marks the beginning of an event region."""
        self._event.begin()

    def end(self) -> None:
        """Marks the end of an event region."""
        self._event.end()


def event(region=None) -> Event:
    """
    Creates an Event instance.
    :param region: a name of the event or a callable object (e.g. function) to wrap. If the callable object is
                   passed the name of this object is used as a name for the event.
    :return: an Event instance
    """
    region = _CallSite(_CallSite.CallerFrame) if region is None else region
    return Event(region)
