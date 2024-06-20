#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>


namespace ittapi
{
namespace pyext
{

template<typename T>
T* pyobject_cast(PyObject* self)
{
	return reinterpret_cast<T*>(self);
}

inline PyObject* new_ref(PyObject* obj);
inline PyObject* xnew_ref(PyObject* obj);

int add_type(PyObject* module, PyTypeObject* type);


/* Implementation of inline functions */
PyObject* new_ref(PyObject* obj)
{
	Py_INCREF(obj);
	return obj;
}

PyObject* xnew_ref(PyObject* obj)
{
	Py_XINCREF(obj);
	return obj;
}

} // namespace pyext
} // namespace ittapi