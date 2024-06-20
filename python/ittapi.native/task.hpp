#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>


namespace ittapi
{

PyObject* task_begin(PyObject* self, PyObject* args);
PyObject* task_end(PyObject* self, PyObject* args);
PyObject* task_begin_overlapped(PyObject* self, PyObject* args);
PyObject* task_end_overlapped(PyObject* self, PyObject* args);

} // namespace ittapi
