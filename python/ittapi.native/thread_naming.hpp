#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>


namespace pyitt
{

PyObject* thread_set_name(PyObject* self, PyObject* args);

} // namespace pyitt