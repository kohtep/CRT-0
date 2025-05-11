#pragma once

// Inconsistent annotation for '...': this instance has no annotations.
#pragma warning(disable : 28251)

// The function does not provide a return value for all control paths.
#pragma warning(disable : 28196)

// '...': could be '0': this does not adhere to the specification for the function.
#pragma warning(disable : 6387)

// '...': not available as an intrinsic function
#pragma warning(disable : 4163)

// '...': This function or variable may be unsafe. Consider using '...' instead.
#pragma warning(disable : 4996)

// '...' : intrinsic function not declared
#pragma warning(disable : 4164)

#ifdef __cplusplus
	#define CRT_API extern "C"
	#define CRT_CALL __cdecl // __CRTDECL
#else
	#define CRT_API
	#define CRT_CALL __cdecl // __CRTDECL
#endif

#define CRT_NORETURN __declspec(noreturn)

#define CRT_NO_INTRINSIC(name) __pragma(function(name))

#include <stdint.h>