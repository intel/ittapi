#include "collection_control.hpp"

#include <ittnotify.h>

#include "string_handle.hpp"


namespace ittapi
{

PyObject* thread_set_name(PyObject* self, PyObject* name)
{
    if (Py_TYPE(name) == &StringHandleType)
    {
        name = string_handle_obj(name)->str;
    }
    else if (!PyUnicode_Check(name))
    {
        PyErr_SetString(PyExc_TypeError, "The passed thread name is not a valid instance of str or StringHandle.");
        return nullptr;
    }

#if defined(_WIN32)
    wchar_t* name_wstr = PyUnicode_AsWideCharString(name, nullptr);
    if (name_wstr == nullptr)
    {
        return nullptr;
    }

    __itt_thread_set_nameW(name_wstr);
    PyMem_Free(name_wstr);
#else
    const char* name_str = PyUnicode_AsUTF8(name);
    if (name_str == nullptr)
    {
        return nullptr;
    }

    __itt_thread_set_name(name_str);
#endif

    Py_RETURN_NONE;
}

} // namespace ittapi