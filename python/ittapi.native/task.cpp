#include "task.hpp"

#include <ittnotify.h>

#include "domain.hpp"
#include "id.hpp"
#include "string_handle.hpp"

namespace ittapi
{

PyObject* task_begin(PyObject* self, PyObject* args)
{
    PyObject* domain = nullptr;
    PyObject* name_string_handle = nullptr;
    PyObject* task_id = nullptr;
    PyObject* parent_id = nullptr;

    if (!PyArg_ParseTuple(args, "OO|OO", &domain, &name_string_handle, &task_id, &parent_id))
    {
        return nullptr;
    }

    Domain* domain_obj = domain_check(domain);
    if (domain_obj == nullptr)
    {
        return nullptr;
    }

    StringHandle* name_string_handle_obj = string_handle_check(name_string_handle);
    if (name_string_handle_obj == nullptr)
    {
        return nullptr;
    }

    __itt_id id = __itt_null;
    if (task_id && task_id != Py_None)
    {
        Id* task_id_obj = id_check(task_id);
        if (task_id_obj == nullptr)
        {
            return nullptr;
        }

        id = task_id_obj->id;
    }

    __itt_id p_id = __itt_null;
    if (parent_id && parent_id != Py_None)
    {
        Id* parent_id_obj = id_check(parent_id);
        if (parent_id_obj == nullptr)
        {
            return nullptr;
        }

        p_id = parent_id_obj->id;
    }

    __itt_task_begin(domain_obj->handle, id, p_id, name_string_handle_obj->handle);

    Py_RETURN_NONE;
}

PyObject* task_end(PyObject* self, PyObject* args)
{

    PyObject* domain = nullptr;

    if (!PyArg_ParseTuple(args, "O", &domain))
    {
        return nullptr;
    }

    Domain* domain_obj = domain_check(domain);
    if (domain_obj == nullptr)
    {
        return nullptr;
    }

    __itt_task_end(domain_obj->handle);

    Py_RETURN_NONE;
}

PyObject* task_begin_overlapped(PyObject* self, PyObject* args)
{
    PyObject* domain = nullptr;
    PyObject* name_string_handle = nullptr;
    PyObject* task_id = nullptr;
    PyObject* parent_id = nullptr;

    if (!PyArg_ParseTuple(args, "OOO|O", &domain, &name_string_handle, &task_id, &parent_id))
    {
        return nullptr;
    }

    Domain* domain_obj = domain_check(domain);
    if (domain_obj == nullptr)
    {
        return nullptr;
    }

    StringHandle* name_string_handle_obj = string_handle_check(name_string_handle);
    if (name_string_handle_obj == nullptr)
    {
        return nullptr;
    }

    Id* task_id_obj = id_check(task_id);
    if (task_id_obj == nullptr)
    {
        return nullptr;
    }

    __itt_id p_id = __itt_null;
    if (parent_id && parent_id != Py_None)
    {
        Id* parent_id_obj = id_check(parent_id);
        if (parent_id_obj == nullptr)
        {
            return nullptr;
        }

        p_id = parent_id_obj->id;
    }

    __itt_task_begin_overlapped(domain_obj->handle,
                                task_id_obj->id,
                                p_id,
                                name_string_handle_obj->handle);

    Py_RETURN_NONE;
}

PyObject* task_end_overlapped(PyObject* self, PyObject* args)
{

    PyObject* domain = nullptr;
    PyObject* task_id = nullptr;

    if (!PyArg_ParseTuple(args, "OO", &domain, &task_id))
    {
        return nullptr;
    }

    Domain* domain_obj = domain_check(domain);
    if (domain_obj == nullptr)
    {
        return nullptr;
    }

    Id* task_id_obj = id_check(task_id);
    if (task_id_obj == nullptr)
    {
        return nullptr;
    }

    __itt_task_end_overlapped(domain_obj->handle, task_id_obj->id);

    Py_RETURN_NONE;
}

} // namespace ittapi