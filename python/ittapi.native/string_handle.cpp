#include "string_handle.hpp"

#include <structmember.h>

#include "extensions/string.hpp"


namespace ittapi
{

template<typename T>
T* string_handle_cast(StringHandle* self);

template<>
PyObject* string_handle_cast(StringHandle* self)
{
    return reinterpret_cast<PyObject*>(self);
}

static PyObject* string_handle_new(PyTypeObject* type, PyObject* args, PyObject* kwargs);
static void string_handle_dealloc(PyObject* self);

static PyObject* string_handle_repr(PyObject* self);
static PyObject* string_handle_str(PyObject* self);

static PyMemberDef string_handle_attrs[] =
{
    {"_str",  T_OBJECT, offsetof(StringHandle, str), READONLY, "a string for which the handle has been created"},
    {nullptr},
};

PyTypeObject StringHandleType =
{
    .ob_base              = PyVarObject_HEAD_INIT(nullptr, 0)
    .tp_name              = "ittapi.native.StringHandle",
    .tp_basicsize         = sizeof(StringHandle),
    .tp_itemsize          = 0,

    /* Methods to implement standard operations */
    .tp_dealloc           = string_handle_dealloc,
    .tp_vectorcall_offset = 0,
    .tp_getattr           = nullptr,
    .tp_setattr           = nullptr,
    .tp_as_async          = nullptr,
    .tp_repr              = string_handle_repr,

    /* Method suites for standard classes */
    .tp_as_number         = nullptr,
    .tp_as_sequence       = nullptr,
    .tp_as_mapping        = nullptr,

    /* More standard operations (here for binary compatibility) */
    .tp_hash              = nullptr,
    .tp_call              = nullptr,
    .tp_str               = string_handle_str,
    .tp_getattro          = nullptr,
    .tp_setattro          = nullptr,

    /* Functions to access object as input/output buffer */
    .tp_as_buffer         = nullptr,

    /* Flags to define presence of optional/expanded features */
    .tp_flags             = Py_TPFLAGS_DEFAULT,

    /* Documentation string */
    .tp_doc               = "A class that represents a ITT string handle.",

    /* Assigned meaning in release 2.0 call function for all accessible objects */
    .tp_traverse          = nullptr,

    /* Delete references to contained objects */
    .tp_clear             = nullptr,

    /* Assigned meaning in release 2.1 rich comparisons */
    .tp_richcompare       = nullptr,

    /* weak reference enabler */
    .tp_weaklistoffset    = 0,

    /* Iterators */
    .tp_iter              = nullptr,
    .tp_iternext          = nullptr,

    /* Attribute descriptor and subclassing stuff */
    .tp_methods           = nullptr,
    .tp_members           = string_handle_attrs,
    .tp_getset            = nullptr,

    /* Strong reference on a heap type, borrowed reference on a static type */
    .tp_base              = nullptr,
    .tp_dict              = nullptr,
    .tp_descr_get         = nullptr,
    .tp_descr_set         = nullptr,
    .tp_dictoffset        = 0,
    .tp_init              = nullptr,
    .tp_alloc             = nullptr,
    .tp_new               = string_handle_new,

    /* Low-level free-memory routine */
    .tp_free              = nullptr,

    /* For PyObject_IS_GC */
    .tp_is_gc             = nullptr,
    .tp_bases             = nullptr,

    /* method resolution order */
    .tp_mro               = nullptr,
    .tp_cache             = nullptr,
    .tp_subclasses        = nullptr,
    .tp_weaklist          = nullptr,
    .tp_del               = nullptr,

    /* Type attribute cache version tag. Added in version 2.6 */
    .tp_version_tag       = 0,

    .tp_finalize          = nullptr,
    .tp_vectorcall        = nullptr,
};

static PyObject* string_handle_new(PyTypeObject* type, PyObject* args, PyObject* kwargs)
{
    StringHandle* self = string_handle_obj(type->tp_alloc(type, 0));

    if (self == nullptr)
    {
        return nullptr;
    }

    char str_key[] = { "str" };
    char* kwlist[] = { str_key, nullptr };

    PyObject* str = nullptr;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &str))
    {
        return nullptr;
    }

    if (str && PyUnicode_Check(str))
    {
        self->str = pyext::new_ref(str);
    }
    else
    {
        Py_DecRef(string_handle_cast<PyObject>(self));

        PyErr_SetString(PyExc_TypeError, "The passed string to create string handle is not a valid instance of str.");
        return nullptr;
    }

    pyext::string str_wrapper = pyext::string::from_unicode(self->str);
    if (str_wrapper.c_str() == nullptr)
    {
        Py_DecRef(string_handle_cast<PyObject>(self));
        return nullptr;
    }

#if defined(_WIN32)
    self->handle = __itt_string_handle_createW(str_wrapper.c_str());
#else
    self->handle = __itt_string_handle_create(str_wrapper.c_str());
#endif

    return string_handle_cast<PyObject>(self);
}

static void string_handle_dealloc(PyObject* self)
{
    if (self == nullptr)
    {
        return;
    }

    StringHandle* obj = string_handle_obj(self);
    Py_XDECREF(obj->str);
}

static PyObject* string_handle_repr(PyObject* self)
{
    StringHandle* obj = string_handle_check(self);
    if (obj == nullptr)
    {
        return nullptr;
    }

    if (obj->str == nullptr)
    {
        PyErr_SetString(PyExc_AttributeError, "The str attribute has not been initialized.");
        return nullptr;
    }

    return PyUnicode_FromFormat("%s('%U')", StringHandleType.tp_name, obj->str);
}

static PyObject* string_handle_str(PyObject* self)
{
    StringHandle* obj = string_handle_check(self);
    if (obj == nullptr)
    {
        return nullptr;
    }

    if (obj->str == nullptr)
    {
        PyErr_SetString(PyExc_AttributeError, "The str attribute has not been initialized.");
        return nullptr;
    }

    return pyext::new_ref(obj->str);
}

StringHandle* string_handle_check(PyObject* self)
{
    if (self == nullptr || Py_TYPE(self) != &StringHandleType)
    {
        PyErr_SetString(PyExc_TypeError, "The passed string handle is not a valid instance of StringHandle.");
        return nullptr;
    }

    return string_handle_obj(self);
}

int exec_string_handle(PyObject* module)
{
    return pyext::add_type(module, &StringHandleType);
}

} // namespace ittapi
