#include "collection_control.hpp"

#include <ittnotify.h>


namespace ittapi
{

PyObject* pause(PyObject* self, PyObject* Py_UNUSED(args))
{
    Py_BEGIN_ALLOW_THREADS;
    __itt_pause();
    Py_END_ALLOW_THREADS;
    Py_RETURN_NONE;
}

PyObject* resume(PyObject* self, PyObject* Py_UNUSED(args))
{
    Py_BEGIN_ALLOW_THREADS;
    __itt_resume();
    Py_END_ALLOW_THREADS;
    Py_RETURN_NONE;
}

PyObject* detach(PyObject* self, PyObject* Py_UNUSED(args))
{
    Py_BEGIN_ALLOW_THREADS;
    __itt_detach();
    Py_END_ALLOW_THREADS;
    Py_RETURN_NONE;
}

} // namespace ittapi