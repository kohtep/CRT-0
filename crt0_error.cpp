#include "crt0_common.hpp"

CRT_API int *CRT_CALL _errno()
{
	static int errnum = 0;
	return &errnum;
}