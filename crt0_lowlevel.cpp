#include "crt0_common.hpp"

CRT_API int _fltused = 0;

CRT_API int CRT_CALL _CrtDbgReport(int _ReportType, char const *_FileName, int _Linenumber,
	char const *_ModuleName, char const *_Format, ...)
{
	return 0;
}