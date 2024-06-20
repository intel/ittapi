#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <ittnotify.h>

#include "extensions/python.hpp"


namespace ittapi
{

struct Id
{
	PyObject_HEAD
	PyObject* domain;
	__itt_id id;
};

extern PyTypeObject IdType;

inline Id* id_obj(PyObject* self);
Id* id_check(PyObject* self);
int exec_id(PyObject* module);


/* Implementation of inline functions */
Id* id_obj(PyObject* self)
{
	return pyext::pyobject_cast<Id>(self);
}

} // namespace ittapi
