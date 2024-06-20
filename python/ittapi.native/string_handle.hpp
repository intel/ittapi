#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <ittnotify.h>

#include "extensions/python.hpp"


namespace ittapi
{

struct StringHandle
{
	PyObject_HEAD
	PyObject* str;
	__itt_string_handle* handle;
};

extern PyTypeObject StringHandleType;

inline StringHandle* string_handle_obj(PyObject* self);
StringHandle* string_handle_check(PyObject* self);
int exec_string_handle(PyObject* module);


/* Implementation of inline functions */
StringHandle* string_handle_obj(PyObject* self)
{
	return pyext::pyobject_cast<StringHandle>(self);
}

} // namespace ittapi