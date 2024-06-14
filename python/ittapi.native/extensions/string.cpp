#include "string.hpp"


namespace ittapi
{
namespace pyext
{

string::~string()
{
	if (m_is_owner)
	{
		PyMem_Free(const_cast<pointer>(m_str));
	}
}

string string::from_unicode(PyObject* str)
{
	if (!PyUnicode_Check(str))
	{
		return string(nullptr, false);
	}

#if defined(_WIN32)
	pointer str_ptr = PyUnicode_AsWideCharString(str, nullptr);
	const bool is_owner = true;
#else
	const_pointer str_ptr = PyUnicode_AsUTF8(str);
	const bool is_owner = false;
#endif

	return string(str_ptr, is_owner);
}

} // namespace pyext
} // namespace ittapi
