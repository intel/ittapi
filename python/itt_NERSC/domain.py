from ittapi.native import Domain as _Domain

def domain_create(name=None):
    """
        In NERSC implementation, a capsule was being made in which a pointer for acccessing domain was being stored which can be accessed by name "itt.domain"
    """
    print("Calling NERSC domain")
    return _Domain(name)
