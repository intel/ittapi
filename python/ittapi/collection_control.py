"""
collection_control.py - Python module wrapper for ITT Collection Control API
"""
from ittapi.native import detach as _detach, pause as _pause, resume as _resume

from .region import _Region


class _CollectionRegion(_Region):
    """
    An abstract base class that provides common functionality for subclasses that represent paused/resumed collection
    regions.
    """
    def __init__(self, func=None, activator=None):
        """
        Creates a collection region.
        :param func: a callable object that represents the collection region, e.g. function.
        :param activator: a callable object that determines if the region is active or not. The callable object should
                          not take any arguments and should return True if the region is active, otherwise should return
                          False. If the region is active, _CollectionRegion performs _begin()/_end() calls to inform
                          subclasses about the start and end of the region. Otherwise, it does nothing.
        """
        super().__init__(func)
        self.activator = activator
        self.__is_paired_call_needed = False

    def _begin(self):
        raise NotImplementedError()

    def _end(self):
        raise NotImplementedError()

    def begin(self):
        """Marks the beginning of a collection region."""
        if callable(self.activator):
            activator_state = self.activator()
            self.__is_paired_call_needed = activator_state

            if activator_state:
                self._begin()
        else:
            self.__is_paired_call_needed = True
            self._begin()

    def end(self):
        """Marks the end of a collection region."""
        if self.__is_paired_call_needed:
            self._end()


def detach() -> None:
    """Detach collection of profiling data."""
    _detach()


def pause() -> None:
    """Pause collection of profiling data."""
    _pause()


def resume() -> None:
    """Resume collection of profiling data."""
    _resume()


class ManualCollectionRegionActivator:
    """
    A class that provides ability to activate/deactivate paused/resumed regions.
    """
    INACTIVE = 0
    ACTIVE = 1

    def __init__(self, state=ACTIVE):
        """
        Creates an activator.
        :param state: sets the initial state of activator.
        """
        self._state = state

    def __call__(self):
        return self._state == self.ACTIVE

    def activate(self):
        """Activates the region."""
        self._state = self.ACTIVE

    def deactivate(self):
        """Deactivates the region."""
        self._state = self.INACTIVE


class ActiveRegion(_CollectionRegion):
    """
    A class that represents resumed collection region.

    It allows to collect profiling only for this region. The collection of profiling data have to be run in
    Start Paused mode.
    """
    def __init__(self, func=None, activator=ManualCollectionRegionActivator()):
        """
        Creates an instance of the class that represents resumed collection region.
        :param func: a callable object that represents the collection region, e.g. function.
        :param activator: a callable object that determines if the region is active or not. The callable object should
                          not take any arguments and should return True if the region is active, otherwise should return
                          False. If the region is active, a call of begin() method of the instance will resume
                          the collection of profiling data and a call of end() method will pause the collection again.
                          Otherwise, these calls do nothing.
        """
        super().__init__(func, activator)

    def _begin(self):
        resume()

    def _end(self):
        pause()


def active_region(func=None, activator=ManualCollectionRegionActivator()):
    """
    Creates a resumed collection region with the given arguments.
    :param func: a callable object that represents the collection region, e.g. function.
    :param activator: a callable object that determines if the region is active or not. The callable object should
                      not take any arguments and should return True if the region is active, otherwise should return
                      False. If the region is active, a call of begin() method of the instance will resume
                      the collection of profiling data and a call of end() method will pause the collection again.
                      Otherwise, these calls do nothing.
    :return: an instance of ActiveRegion.
    """
    return ActiveRegion(func, activator)


class PausedRegion(_CollectionRegion):
    """
    A class that represents paused collection region.

    An instance of this class allows to disable the collection of profiling data for the code region that is not
    interested.
    """
    def __init__(self, func=None, activator=ManualCollectionRegionActivator()):
        """
        Creates an instance of the class that represents paused collection region.
        :param func: a callable object that represents the collection region, e.g. function.
        :param activator: a callable object that determines if the region is active or not. The callable object should
                          not take any arguments and should return True if the region is active, otherwise should return
                          False. If the region is active, a call of begin() method for the instance will pause
                          the collection of profiling data and a call of end() method will resume the collection again.
                          Otherwise, these calls do nothing.
        """
        super().__init__(func, activator)

    def _begin(self):
        pause()

    def _end(self):
        resume()


def paused_region(func=None, activator=ManualCollectionRegionActivator()):
    """
    Creates a paused collection region with the given arguments.
    :param func: a callable object that represents the collection region, e.g. function.
    :param activator: a callable object that determines if the region is active or not. The callable object should
                      not take any arguments and should return True if the region is active, otherwise should return
                      False. If the region is active, a call of begin() method for the instance will pause
                      the collection of profiling data and a call of end() method will resume the collection again.
                      Otherwise, these calls do nothing.
    :return: an instance of PausedRegion.
    """
    return PausedRegion(func, activator)
