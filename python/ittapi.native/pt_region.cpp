#include "pt_region.hpp"

#include <structmember.h>

#include "string_handle.hpp"
#include "extensions/string.hpp"

namespace ittapi
{

template<typename T>
T* pt_region_cast(PT_Region* self);

template <>
PyObject* pt_region_cast(PT_Region* self)
{
    return reinterpret_cast<PyObject*>(self);
}

static PyObject* pt_region_new(PyTypeObject* type, PyObject* args, PyObject* kwargs);
static void pt_region_dealloc(PyObject* self);

static PyObject* pt_region_repr(PyObject* self);
static PyObject* pt_region_str(PyObject* self);

static PyMemberDef pt_region_attrs[] =
{
    {"name",  T_OBJECT, offsetof(PT_Region, name), READONLY, "a pt_region name"},
    {nullptr},
};

PyTypeObject PT_RegionType =
{
    .ob_base              = PyVarObject_HEAD_INIT(nullptr, 0)
    .tp_name              = "ittapi.native.PT_Region",
    .tp_basicsize         = sizeof(PT_Region),
    .tp_itemsize          = 0,

    /* Methods to implement standard operations */
    .tp_dealloc           = pt_region_dealloc,
    .tp_vectorcall_offset = 0,
    .tp_getattr           = nullptr,
    .tp_setattr           = nullptr,
    .tp_as_async          = nullptr,
    .tp_repr              = pt_region_repr,

    /* Method suites for standard classes */
    .tp_as_number         = nullptr,
    .tp_as_sequence       = nullptr,
    .tp_as_mapping        = nullptr,

    /* More standard operations (here for binary compatibility) */
    .tp_hash              = nullptr,
    .tp_call              = nullptr,
    .tp_str               = pt_region_str,
    .tp_getattro          = nullptr,
    .tp_setattro          = nullptr,

    /* Functions to access object as input/output buffer */
    .tp_as_buffer         = nullptr,

    /* Flags to define presence of optional/expanded features */
    .tp_flags             = Py_TPFLAGS_DEFAULT,

    /* Documentation string */
    .tp_doc               = "A class that represents a __itt_pt_region(__itt_pt_region -> unsigned char)",

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
    .tp_members           = pt_region_attrs,
    .tp_getset            = nullptr,

    /* Strong reference on a heap type, borrowed reference on a static type */
    .tp_base              = nullptr,
    .tp_dict              = nullptr,
    .tp_descr_get         = nullptr,
    .tp_descr_set         = nullptr,
    .tp_dictoffset        = 0,
    .tp_init              = nullptr,
    .tp_alloc             = nullptr,
    .tp_new               = pt_region_new,

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

static PyObject* pt_region_new(PyTypeObject* type, PyObject* args, PyObject* kwargs)
{
    PT_Region* self = pt_region_obj(type->tp_alloc(type,0));
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
        Py_DecRef(pt_region_cast<PyObject>(self));

        PyErr_SetString(PyExc_TypeError, "The passed string to create pt_region is not a valid instance of neither str nor StringHandle");
        return nullptr;
    }

    pyext::string name_str = pyext::string::from_unicode(self->name);
    if (name_str.c_str() == nullptr)
    {
        Py_DecRef(pt_region_cast<PyObject>(self));
        return nullptr;
    }

#if defined(_WIN32)
    self->handle=__itt_pt_region_createW(name_str.c_str());
#else
    self->handle=__itt_pt_region_create(name_str.c_str());
#endif

    return pt_region_cast<PyObject>(self);
}

static void pt_region_dealloc(PyObject* self)
{
    if (self == nullptr)
    {
        return;
    }

    PT_Region* obj = pt_region_obj(self);
    Py_XDECREF(obj->name);
}

static PyObject* pt_region_repr(PyObject* self)
{
    PT_Region* obj = pt_region_check(self);
    if (obj == nullptr)
    {
        return nullptr;
    }

    if (obj->name == nullptr)
    {
        PyErr_SetString(PyExc_AttributeError, "The name attribute has not been initialized.");
        return nullptr;
    }

    return PyUnicode_FromFormat("%s('%U')", PT_RegionType.tp_name, obj->name);
}

static PyObject* pt_region_str(PyObject* self)
{
    PT_Region* obj = pt_region_check(self);
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

PT_Region* pt_region_check(PyObject* self)
{
    if (self == nullptr || Py_TYPE(self) != &PT_RegionType)
    {
        PyErr_SetString(PyExc_TypeError, "The passed pt_region is not a valid instance of PT_Region type.");
        return nullptr;
    }

    return pt_region_obj(self);
}

int exec_pt_region(PyObject* module)
{
    return pyext::add_type(module, &PT_RegionType);
}

PyObject* pt_region_begin(PyObject* self, PyObject* args)
{
    PyObject* pt_region = nullptr;
    if (!PyArg_ParseTuple(args, "O", &pt_region))
    {
        return nullptr;
    }

    PT_Region *pt_region_obj = pt_region_check(pt_region);
    if (pt_region_obj == nullptr)
    {
        return nullptr;
    }

    __itt_mark_pt_region_begin(pt_region_obj->handle);

    Py_RETURN_NONE;
}

PyObject* pt_region_end(PyObject* self, PyObject* args)
{
    PyObject* pt_region = nullptr;
    if (!PyArg_ParseTuple(args, "O", &pt_region))
    {
        return nullptr;
    }

    PT_Region *pt_region_obj = pt_region_check(pt_region);
    if (pt_region_obj == nullptr)
    {
        return nullptr;
    }

    __itt_mark_pt_region_end(pt_region_obj->handle);

    Py_RETURN_NONE;
}

} // namespace ittapi
