#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>


namespace ittapi
{

PyObject* pause(PyObject* self, PyObject* args);
PyObject* resume(PyObject* self, PyObject* args);
PyObject* detach(PyObject* self, PyObject* args);

} // namespace ittapi