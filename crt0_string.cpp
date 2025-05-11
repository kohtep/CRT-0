#include "crt0_common.hpp"

#include <Windows.h>
#include <corecrt_wtime.h>

CRT_NO_INTRINSIC(strcmp);
CRT_NO_INTRINSIC(strcpy);
CRT_NO_INTRINSIC(strlen);

CRT_API const char *CRT_CALL strchr(const char *lpString, int ch)
{
	while (*lpString != '\0')
	{
		if (*lpString == ch)
			return const_cast<char *>(lpString);

		++lpString;
	}

	if (ch == '\0')
		return const_cast<char *>(lpString);

	return nullptr;
}

CRT_API int CRT_CALL strcmp(const char *lpStr1, const char *lpStr2)
{
	while (*lpStr1 && (*lpStr1 == *lpStr2))
	{
		++lpStr1;
		++lpStr2;
	}
	return static_cast<unsigned char>(*lpStr1) - static_cast<unsigned char>(*lpStr2);
}

CRT_API int CRT_CALL strcoll(const char *lpStr1, const char *lpStr2)
{
	while (*lpStr1 && (*lpStr1 == *lpStr2))
	{
		++lpStr1;
		++lpStr2;
	}
	return static_cast<unsigned char>(*lpStr1) - static_cast<unsigned char>(*lpStr2);
}

CRT_API char *CRT_CALL strcpy(char *lpDest, const char *lpSrc)
{
	char *lpResult = lpDest;

	while ((*lpDest++ = *lpSrc++) != '\0')
	{
		;
	}

	return lpResult;
}

char *strncpy(char *lpDest, const char *lpSrc, size_t dwMaxLen)
{
	char *lpResult = lpDest;
	size_t len = 0;

	while (len < dwMaxLen && lpSrc[len] != '\0')
	{
		lpDest[len] = lpSrc[len];
		++len;
	}

	lpDest[len] = '\0';
	return lpResult;
}

CRT_API char *CRT_CALL strerror(int errnum)
{
	static char buffer[128];

	if (!FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, errnum, 0, buffer, sizeof(buffer), nullptr))
	{
		buffer[0] = 'U';
		buffer[1] = 'n';
		buffer[2] = 'k';
		buffer[3] = 'n';
		buffer[4] = 'o';
		buffer[5] = 'w';
		buffer[6] = 'n';
		buffer[7] = ' ';
		buffer[8] = 'e';
		buffer[9] = 'r';
		buffer[10] = 'r';
		buffer[11] = 'o';
		buffer[12] = 'r';
		buffer[13] = '\0';
	}

	return buffer;
}

CRT_API size_t CRT_CALL strlen(const char *lpString)
{
	const char *p = lpString;
	while (*p) ++p;
	return static_cast<size_t>(p - lpString);
}

CRT_API size_t CRT_CALL strftime(char *str, size_t maxsize, const char *format, const struct tm *timeptr)
{
	if (!str || !format || !timeptr || maxsize == 0)
		return 0;

	SYSTEMTIME st;
	st.wYear = static_cast<WORD>(timeptr->tm_year + 1900);
	st.wMonth = static_cast<WORD>(timeptr->tm_mon + 1);
	st.wDay = static_cast<WORD>(timeptr->tm_mday);
	st.wHour = static_cast<WORD>(timeptr->tm_hour);
	st.wMinute = static_cast<WORD>(timeptr->tm_min);
	st.wSecond = static_cast<WORD>(timeptr->tm_sec);
	st.wMilliseconds = 0;

	wchar_t wformat[128];
	wchar_t wbuffer[256];
	char result[256];

	int len_format = MultiByteToWideChar(CP_ACP, 0, format, -1, wformat, 128);
	if (len_format == 0)
		return 0;

	int date_len = GetDateFormatW(LOCALE_USER_DEFAULT, 0, &st, wformat, wbuffer, 256);
	if (date_len == 0)
		return 0;

	int time_len = GetTimeFormatW(LOCALE_USER_DEFAULT, 0, &st, nullptr, wbuffer + date_len - 1, 256 - date_len + 1);
	if (time_len == 0)
		return 0;

	int final_len = WideCharToMultiByte(CP_ACP, 0, wbuffer, -1, result, static_cast<int>(sizeof(result)), nullptr, nullptr);
	if (final_len == 0 || static_cast<size_t>(final_len) > maxsize)
		return 0;

	strncpy(str, result, maxsize);
	return strlen(str);
}

CRT_API int CRT_CALL strncmp(const char *lpStr1, const char *lpStr2, size_t dwCount)
{
	while (dwCount && *lpStr1 && (*lpStr1 == *lpStr2))
	{
		++lpStr1;
		++lpStr2;
		--dwCount;
	}

	if (dwCount == 0)
		return 0;

	return static_cast<unsigned char>(*lpStr1) - static_cast<unsigned char>(*lpStr2);
}

CRT_API const char *CRT_CALL strpbrk(const char *str, const char *accept)
{
	for (; *str; ++str)
	{
		for (const char *a = accept; *a; ++a)
		{
			if (*str == *a)
				return const_cast<char *>(str);
		}
	}

	return nullptr;
}

CRT_API const char *CRT_CALL strrchr(const char *str, int ch)
{
	const char *last = nullptr;
	while (*str)
	{
		if (*str == ch)
			last = str;

		++str;
	}

	return const_cast<char *>(last);
}

CRT_API size_t CRT_CALL strspn(const char *str, const char *accept)
{
	size_t count = 0;
	while (str[count])
	{
		const char *a = accept;
		bool found = false;
		while (*a)
		{
			if (str[count] == *a)
			{
				found = true;
				break;
			}
			++a;
		}

		if (!found)
			break;

		++count;
	}
	return count;
}

CRT_API const char *CRT_CALL strstr(const char *haystack, const char *needle)
{
	if (!*needle)
		return haystack;

	for (; *haystack; ++haystack)
	{
		const char *h = haystack;
		const char *n = needle;

		while (*h && *n && *h == *n)
		{
			++h;
			++n;
		}

		if (!*n)
			return haystack;
	}

	return nullptr;
}

CRT_API double CRT_CALL strtod(const char *str, char **endptr)
{
	while (*str == ' ' || *str == '\t' || *str == '\n')
		++str;

	bool neg = false;
	if (*str == '-')
	{
		neg = true;
		++str;
	}
	else if (*str == '+')
	{
		++str;
	}

	double result = 0.0;

	while (*str >= '0' && *str <= '9')
	{
		result = result * 10.0 + (*str - '0');
		++str;
	}

	if (*str == '.')
	{
		++str;
		double frac = 0.0;
		double scale = 1.0;

		while (*str >= '0' && *str <= '9')
		{
			frac = frac * 10.0 + (*str - '0');
			scale *= 10.0;
			++str;
		}

		result += frac / scale;
	}

	if (*str == 'e' || *str == 'E')
	{
		++str;
		bool exp_neg = false;
		if (*str == '-')
		{
			exp_neg = true;
			++str;
		}
		else if (*str == '+')
		{
			++str;
		}

		int exp = 0;
		while (*str >= '0' && *str <= '9')
		{
			exp = exp * 10 + (*str - '0');
			++str;
		}

		double factor = 1.0;
		while (exp > 0)
		{
			factor *= 10.0;
			--exp;
		}

		if (exp_neg)
			result /= factor;
		else
			result *= factor;
	}

	if (endptr)
		*endptr = const_cast<char *>(str);

	return neg ? -result : result;
}