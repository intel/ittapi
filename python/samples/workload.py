from math import sin, cos


def workload():
    v = 1.0
    for _ in range(500000):
        v += sin(v) * cos(v)
    return v
