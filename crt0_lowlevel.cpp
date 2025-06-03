#include "crt0_common.hpp"

CRT_API int _fltused = 0;

CRT_API int CRT_CALL _CrtDbgReport(int _ReportType, char const *_FileName, int _Linenumber,
	char const *_ModuleName, char const *_Format, ...)
{
	return 0;
}

CRT_API int CRT_CALL _CrtDbgReportW(int _ReportType, wchar_t const *_FileName, int _LineNumber,
	wchar_t const *_ModuleName, wchar_t const *_Format, ...)
{
	return 0;
}