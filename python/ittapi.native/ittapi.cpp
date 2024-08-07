#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "collection_control.hpp"
#include "domain.hpp"
#include "event.hpp"
#include "id.hpp"
#include "string_handle.hpp"
#include "task.hpp"
#include "thread_naming.hpp"
#include "pt_region.hpp"


namespace ittapi
{

/**
 Initialize ittapi.
 May be called multiple times, so avoid using static state.
 */
static int exec_ittapi_module(PyObject* module)
{
    static PyMethodDef ittapi_functions[] =
    {
        /* Collection Control API */
        {"pause",                 pause,                 METH_NOARGS,  "Pause data collection."},
        {"resume",                resume,                METH_NOARGS,  "Resume data collection."},
        {"detach",                detach,                METH_NOARGS,  "Detach data collection."},
        /* Thread Naming API */
        {"thread_set_name",       thread_set_name,       METH_O,       "Sets a name for current thread."},
        /* Task API */
        {"task_begin",            task_begin,            METH_VARARGS, "Marks the beginning of a task."},
        {"task_end",              task_end,              METH_VARARGS, "Marks the end of a task."},
        {"task_begin_overlapped", task_begin_overlapped, METH_VARARGS, "Marks the beginning of an overlapped task."},
        {"task_end_overlapped",   task_end_overlapped,   METH_VARARGS, "Marks the end of an overlapped task."},
        {"pt_region_begin",       pt_region_begin,       METH_VARARGS, "Marks the begining of a processor trace control region"},
        {"pt_region_end",         pt_region_end,         METH_VARARGS, "Marks the end of a processor trace control region"},
        /* marks end of array */
        { nullptr },
    };

    PyModule_AddFunctions(module, ittapi_functions);

    PyModule_AddStringConstant(module, "__author__", "Egor Suldin");
    PyModule_AddStringConstant(module, "__version__", "1.2.0");
    PyModule_AddIntConstant(module, "year", 2024);

    return 0;
}

static void destroy_ittapi_module(void*)
{
    __itt_release_resources();
}

PyMODINIT_FUNC PyInit_native()
{
    PyDoc_STRVAR(ittapi_doc, "The ittapi module.");

    static PyModuleDef_Slot ittapi_slots[] =
    {
        { Py_mod_exec, reinterpret_cast<void*>(exec_ittapi_module) },
        { Py_mod_exec, reinterpret_cast<void*>(exec_domain) },
        { Py_mod_exec, reinterpret_cast<void*>(exec_event) },
        { Py_mod_exec, reinterpret_cast<void*>(exec_id) },
        { Py_mod_exec, reinterpret_cast<void*>(exec_string_handle) },
        { Py_mod_exec, reinterpret_cast<void*>(exec_pt_region) },
        { 0, nullptr }
    };

    static PyModuleDef ittapi_def = {
        PyModuleDef_HEAD_INIT,
        "ittapi",
        ittapi_doc,
        0,                    /* m_size */
        nullptr,              /* m_methods */
        ittapi_slots,
        nullptr,              /* m_traverse */
        nullptr,              /* m_clear */
        destroy_ittapi_module, /* m_free */
    };

    return PyModuleDef_Init(&ittapi_def);
}

} // namespace ittapi
