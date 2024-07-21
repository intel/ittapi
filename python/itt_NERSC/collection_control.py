from ittapi.native import detach as _detach, pause as _pause, resume as _resume

def detach():
    """
    Detach collection of profiling data.
    """
    _detach()

def resume():
    """
    Resume collection of profiling data.
    """
    _resume()

def pause():
    """
    Pause collection of profiling data.
    """
    _pause()