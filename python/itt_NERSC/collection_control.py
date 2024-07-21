from ittapi.native import detach as _detach, pause as _pause, resume as _resume

def detach():
    print("Calling NERSC detach")
    _detach()

def resume():
    print("Calling NERSC resume")
    _resume()

def pause():
    print("Calling NERSC pause")
    _pause()