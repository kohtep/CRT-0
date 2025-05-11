#include "crt0_common.hpp"

#include <exception>

namespace std
{
	void __cdecl _Xlength_error(char const *message)
	{
		;
	}

#if !_HAS_EXCEPTIONS
	void(__cdecl *_Raise_handler)(const stdext::exception &) = nullptr;
#endif
}


namespace std
{
	const nothrow_t nothrow;
}

int __cdecl _purecall()
{
	return 0;
}

void __cdecl _invalid_parameter(
	wchar_t const *const expression,
	wchar_t const *const function_name,
	wchar_t const *const file_name,
	unsigned int   const line_number,
	uintptr_t      const reserved)
{
	;
}

void __cdecl _invalid_parameter_noinfo()
{
	;
}

__declspec(noreturn) void __cdecl _invalid_parameter_noinfo_noreturn()
{
	;
}

__declspec(noreturn) void __cdecl _invoke_watson(
	wchar_t const *const expression,
	wchar_t const *const function_name,
	wchar_t const *const file_name,
	unsigned int   const line_number,
	uintptr_t      const reserved)
{
	;
}