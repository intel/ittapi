#include "python.hpp"


namespace ittapi
{
namespace pyext
{

int add_type(PyObject* module, PyTypeObject* type)
{
	if (PyType_Ready(type) < 0)
	{
		return -1;
	}

	const char* name = _PyType_Name(type);

	Py_INCREF(type);
	return PyModule_AddObject(module, name, _PyObject_CAST(type));
}

} // namespace pyext
} // namespace ittapi