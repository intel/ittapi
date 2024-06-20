#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <ittnotify.h>

#include "extensions/python.hpp"


namespace ittapi
{

struct Event
{
	PyObject_HEAD
	PyObject* name;
	__itt_event event;
};

extern PyTypeObject EventType;

inline Event* event_obj(PyObject* self);
Event* event_check(PyObject* self);
int exec_event(PyObject* module);


/* Implementation of inline functions */
Event* event_obj(PyObject* self)
{
	return pyext::pyobject_cast<Event>(self);
}

} // namespace ittapi
