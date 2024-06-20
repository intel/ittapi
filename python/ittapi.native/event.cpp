#include "event.hpp"

#include <structmember.h>

#include "string_handle.hpp"
#include "extensions/string.hpp"


namespace ittapi
{

template<typename T>
T* event_cast(Event* self);

template<>
PyObject* event_cast(Event* self)
{
    return reinterpret_cast<PyObject*>(self);
}

static PyObject* event_new(PyTypeObject* type, PyObject* args, PyObject* kwargs);
static void event_dealloc(PyObject* self);

static PyObject* event_repr(PyObject* self);
static PyObject* event_str(PyObject* self);

static PyObject* event_begin(PyObject* self, PyObject* args);
static PyObject* event_end(PyObject* self, PyObject* args);

static PyMemberDef event_attrs[] =
{
    {"name",  T_OBJECT_EX, offsetof(Event, name), READONLY, "a name of the event"},
    {nullptr},
};

static PyMethodDef event_methods[] =
{
    {"begin", event_begin, METH_NOARGS, "Marks the beginning of the event."},
    {"end", event_end, METH_NOARGS, "Marks the end of the event."},
    {nullptr},
};

PyTypeObject EventType =
{
    .ob_base              = PyVarObject_HEAD_INIT(nullptr, 0)
    .tp_name              = "ittapi.native.Event",
    .tp_basicsize         = sizeof(Event),
    .tp_itemsize          = 0,

    /* Methods to implement standard operations */
    .tp_dealloc           = event_dealloc,
    .tp_vectorcall_offset = 0,
    .tp_getattr           = nullptr,
    .tp_setattr           = nullptr,
    .tp_as_async          = nullptr,
    .tp_repr              = event_repr,

    /* Method suites for standard classes */
    .tp_as_number         = nullptr,
    .tp_as_sequence       = nullptr,
    .tp_as_mapping        = nullptr,

    /* More standard operations (here for binary compatibility) */
    .tp_hash              = nullptr,
    .tp_call              = nullptr,
    .tp_str               = event_str,
    .tp_getattro          = nullptr,
    .tp_setattro          = nullptr,

    /* Functions to access object as input/output buffer */
    .tp_as_buffer         = nullptr,

    /* Flags to define presence of optional/expanded features */
    .tp_flags             = Py_TPFLAGS_DEFAULT,

    /* Documentation string */
    .tp_doc               = "A class that represents a ITT event.",

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
    .tp_methods           = event_methods,
    .tp_members           = event_attrs,
    .tp_getset            = nullptr,

    /* Strong reference on a heap type, borrowed reference on a static type */
    .tp_base              = nullptr,
    .tp_dict              = nullptr,
    .tp_descr_get         = nullptr,
    .tp_descr_set         = nullptr,
    .tp_dictoffset        = 0,
    .tp_init              = nullptr,
    .tp_alloc             = nullptr,
    .tp_new               = event_new,

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

static PyObject* event_new(PyTypeObject* type, PyObject* args, PyObject* kwargs)
{
    Event* self = event_obj(type->tp_alloc(type, 0));

    if (self == nullptr)
    {
        return nullptr;
    }

    char name_key[] = { "name" };
    char* kwlist[] = { name_key, nullptr };

    PyObject* name = nullptr;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &name))
    {
        return nullptr;
    }

    if (name && PyUnicode_Check(name))
    {
        self->name = pyext::new_ref(name);
    }
    else if (name && Py_TYPE(name) == &StringHandleType)
    {
        self->name = pyext::new_ref(string_handle_obj(name)->str);
    }
    else
    {
        Py_DecRef(event_cast<PyObject>(self));

        PyErr_SetString(PyExc_TypeError, "The passed event name is not a valid instance of str or StringHandle.");
        return nullptr;
    }

    pyext::string name_str = pyext::string::from_unicode(self->name);
    if (name_str.c_str() == nullptr)
    {
        Py_DecRef(event_cast<PyObject>(self));
        return nullptr;
    }

#if defined(_WIN32)
    self->event = __itt_event_createW(name_str.c_str(), static_cast<int>(name_str.length()));
#else
    self->event = __itt_event_create(name_str.c_str(), static_cast<int>(name_str.length()));
#endif

    return event_cast<PyObject>(self);
}

static void event_dealloc(PyObject* self)
{
    Event* obj = event_obj(self);
    if (obj == nullptr)
    {
        return;
    }

    Py_XDECREF(obj->name);
}

static PyObject* event_repr(PyObject* self)
{
    Event* obj = event_check(self);
    if (obj == nullptr)
    {
        return nullptr;
    }

    return PyUnicode_FromFormat("%s('%U')", EventType.tp_name, obj->name);
}

static PyObject* event_str(PyObject* self)
{
    Event* obj = event_check(self);
    if (obj == nullptr)
    {
        return nullptr;
    }

    if (obj->name == nullptr)
    {
        PyErr_SetString(PyExc_AttributeError, "The name attribute has not been initialized.");
        return nullptr;
    }

    return pyext::new_ref(obj->name);
}

static PyObject* event_begin(PyObject* self, PyObject* args)
{
    Event* obj = event_check(self);
    if (obj == nullptr)
    {
        return nullptr;
    }

    __itt_event_start(obj->event);

    Py_RETURN_NONE;
}

static PyObject* event_end(PyObject* self, PyObject* args)
{
    Event* obj = event_check(self);
    if (obj == nullptr)
    {
        return nullptr;
    }

    __itt_event_end(obj->event);

    Py_RETURN_NONE;
}

Event* event_check(PyObject* self)
{
    if (self == nullptr || Py_TYPE(self) != &EventType)
    {
        PyErr_SetString(PyExc_TypeError, "The passed event is not a valid instance of Event type.");
        return nullptr;
    }

    return event_obj(self);
}

int exec_event(PyObject* module)
{
    return pyext::add_type(module, &EventType);
}

} // namespace ittapi
