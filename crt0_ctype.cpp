#include <Windows.h>

#include "crt0_common.hpp"

CRT_NO_INTRINSIC(isalnum);
CRT_NO_INTRINSIC(isalpha);
CRT_NO_INTRINSIC(iscntrl);
CRT_NO_INTRINSIC(isdigit);
CRT_NO_INTRINSIC(isgraph);
CRT_NO_INTRINSIC(islower);
CRT_NO_INTRINSIC(ispunct);
CRT_NO_INTRINSIC(isspace);
CRT_NO_INTRINSIC(isupper);
CRT_NO_INTRINSIC(isxdigit);
CRT_NO_INTRINSIC(tolower);
CRT_NO_INTRINSIC(toupper);

CRT_API int CRT_CALL isalnum(int c)
{
	return ((c >= '0' && c <= '9') ||
		(c >= 'A' && c <= 'Z') ||
		(c >= 'a' && c <= 'z'));
}

CRT_API int CRT_CALL isalpha(int c)
{
	return ((c >= 'A' && c <= 'Z') ||
		(c >= 'a' && c <= 'z'));
}

CRT_API int CRT_CALL iscntrl(int c)
{
	return (c >= 0 && c <= 0x1F) || (c == 0x7F);
}

CRT_API int CRT_CALL isdigit(int c)
{
	return (c >= '0' && c <= '9');
}

CRT_API int CRT_CALL isgraph(int c)
{
	return (c > 0x20 && c <= 0x7E);
}

CRT_API int CRT_CALL islower(int c)
{
	return (c >= 'a' && c <= 'z');
}

CRT_API int CRT_CALL ispunct(int c)
{
	return ((c >= 0x21 && c <= 0x2F) ||
		(c >= 0x3A && c <= 0x40) ||
		(c >= 0x5B && c <= 0x60) ||
		(c >= 0x7B && c <= 0x7E));
}

CRT_API int CRT_CALL isspace(int c)
{
	return (c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v');
}

CRT_API int CRT_CALL isupper(int c)
{
	return (c >= 'A' && c <= 'Z');
}

CRT_API int CRT_CALL isxdigit(int c)
{
	return ((c >= '0' && c <= '9') ||
		(c >= 'A' && c <= 'F') ||
		(c >= 'a' && c <= 'f'));
}

CRT_API int CRT_CALL tolower(int c)
{
	if (c >= 'A' && c <= 'Z') return c + 32;
	return c;
}

CRT_API int CRT_CALL toupper(int c)
{
	if (c >= 'a' && c <= 'z') return c - 32;
	return c;
}