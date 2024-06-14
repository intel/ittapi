#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <ittnotify.h>

#include "extensions/python.hpp"


namespace ittapi
{

struct Domain
{
	PyObject_HEAD
	PyObject* name;
	__itt_domain* handle;
};

extern PyTypeObject DomainType;

inline Domain* domain_obj(PyObject* self);
Domain* domain_check(PyObject* self);
int exec_domain(PyObject* module);


/* Implementation of inline functions */
Domain* domain_obj(PyObject* self)
{
	return pyext::pyobject_cast<Domain>(self);
}

} // namespace ittapi
