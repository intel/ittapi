#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <ittnotify.h>

#include "extensions/python.hpp"

namespace ittapi
{

struct PT_Region
{
    PyObject_HEAD
    PyObject* name;
    __itt_pt_region handle;
};

extern PyTypeObject PT_RegionType;

inline PT_Region* pt_region_obj(PyObject* self);
PT_Region* pt_region_check(PyObject* self);
int exec_pt_region(PyObject* module);

/* Implementation of inline functions */
PT_Region* pt_region_obj(PyObject* self)
{
	return pyext::pyobject_cast<PT_Region>(self);
}

PyObject* pt_region_begin(PyObject* self, PyObject* args);
PyObject* pt_region_end(PyObject* self, PyObject* args);

} // namespace ittapi
