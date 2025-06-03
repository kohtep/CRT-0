#include "crt0_common.hpp"

#include <Windows.h>
#include <time.h>

#define SECONDS_FROM_1601_TO_1970 11644473600LL
#define TICKS_PER_SECOND 10000000LL

static struct tm g_gmtime_tm = {};
static struct tm g_localtime_tm = {};

static void FileTimeToTm(const FILETIME &ft, struct tm *out)
{
	SYSTEMTIME st;
	FileTimeToSystemTime(&ft, &st);

	out->tm_sec = st.wSecond;
	out->tm_min = st.wMinute;
	out->tm_hour = st.wHour;
	out->tm_mday = st.wDay;
	out->tm_mon = st.wMonth - 1;
	out->tm_year = st.wYear - 1900;
	out->tm_wday = st.wDayOfWeek;
	out->tm_yday = 0;
	out->tm_isdst = -1;
}

static void TmToSystemTime(const struct tm *in, SYSTEMTIME *out)
{
	out->wSecond = static_cast<WORD>(in->tm_sec);
	out->wMinute = static_cast<WORD>(in->tm_min);
	out->wHour = static_cast<WORD>(in->tm_hour);
	out->wDay = static_cast<WORD>(in->tm_mday);
	out->wMonth = static_cast<WORD>(in->tm_mon + 1);
	out->wYear = static_cast<WORD>(in->tm_year + 1900);
	out->wMilliseconds = 0;
}

CRT_API double CRT_CALL _difftime64(__time64_t time1, __time64_t time0)
{
	return static_cast<double>(time1 - time0);
}

CRT_API struct tm *CRT_CALL _gmtime64(const __time64_t *time)
{
	if (!time)
		return nullptr;

	FILETIME ft;
	ULARGE_INTEGER ull;
	ull.QuadPart = (static_cast<ULONGLONG>(*time) + SECONDS_FROM_1601_TO_1970) * TICKS_PER_SECOND;
	ft.dwLowDateTime = ull.LowPart;
	ft.dwHighDateTime = ull.HighPart;

	FileTimeToTm(ft, &g_gmtime_tm);
	return &g_gmtime_tm;
}

CRT_API struct tm *CRT_CALL _localtime64(const __time64_t *time)
{
	if (!time)
		return nullptr;

	FILETIME ft_utc, ft_local;
	ULARGE_INTEGER ull;
	ull.QuadPart = (static_cast<ULONGLONG>(*time) + SECONDS_FROM_1601_TO_1970) * TICKS_PER_SECOND;
	ft_utc.dwLowDateTime = ull.LowPart;
	ft_utc.dwHighDateTime = ull.HighPart;

	SYSTEMTIME st_utc, st_local;
	FileTimeToSystemTime(&ft_utc, &st_utc);
	SystemTimeToTzSpecificLocalTime(nullptr, &st_utc, &st_local);
	SystemTimeToFileTime(&st_local, &ft_local);

	FileTimeToTm(ft_local, &g_localtime_tm);
	return &g_localtime_tm;
}

CRT_API __time64_t CRT_CALL _mktime64(struct tm *t)
{
	if (!t)
		return -1;

	SYSTEMTIME st;
	TmToSystemTime(t, &st);

	FILETIME ft;
	if (!SystemTimeToFileTime(&st, &ft))
		return -1;

	ULARGE_INTEGER ull;
	ull.LowPart = ft.dwLowDateTime;
	ull.HighPart = ft.dwHighDateTime;

	return static_cast<__time64_t>(ull.QuadPart / TICKS_PER_SECOND - SECONDS_FROM_1601_TO_1970);
}

CRT_API __time64_t CRT_CALL _time64(__time64_t *t)
{
	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);

	ULARGE_INTEGER ull;
	ull.LowPart = ft.dwLowDateTime;
	ull.HighPart = ft.dwHighDateTime;

	__time64_t time_val = static_cast<__time64_t>(ull.QuadPart / TICKS_PER_SECOND - SECONDS_FROM_1601_TO_1970);
	if (t)
		*t = time_val;

	return time_val;
}

CRT_API clock_t CRT_CALL clock()
{
	ULONGLONG ms = GetTickCount64();
	return static_cast<clock_t>(ms * CLOCKS_PER_SEC / 1000);
}

CRT_API long long CRT_CALL _Query_perf_counter()
{
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return counter.QuadPart;
}

CRT_API long long CRT_CALL _Query_perf_frequency()
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	return freq.QuadPart;
}

CRT_API long long CRT_CALL _Xtime_get_ticks()
{
	FILETIME ft;
	ULARGE_INTEGER uli;

	GetSystemTimePreciseAsFileTime(&ft);
	uli.LowPart = ft.dwLowDateTime;
	uli.HighPart = ft.dwHighDateTime;

	return (long long)(uli.QuadPart - 116444736000000000ULL);
}