#include "crt0_common.hpp"

#include <Windows.h>
#include <corecrt_wtime.h>

CRT_NO_INTRINSIC(strcmp);
CRT_NO_INTRINSIC(strcpy);
CRT_NO_INTRINSIC(strlen);
CRT_NO_INTRINSIC(wcslen);
CRT_NO_INTRINSIC(wcscpy);

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

CRT_API char *CRT_CALL strncpy(char *lpDest, const char *lpSrc, size_t dwMaxLen)
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

CRT_API int CRT_CALL strncpy_s(char *_Destination, rsize_t _SizeInBytes, char const *_Source, rsize_t _MaxCount)
{
	if (!_Destination || !_Source || _SizeInBytes == 0)
		return 1;

	rsize_t i = 0;

	while (i < _MaxCount && i < _SizeInBytes - 1 && _Source[i] != '\0')
	{
		_Destination[i] = _Source[i];
		++i;
	}

	if (i < _SizeInBytes)
		_Destination[i] = '\0';
	else if (_SizeInBytes > 0)
		_Destination[_SizeInBytes - 1] = '\0';

	return 0;
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

CRT_API char *CRT_CALL strtok(char *str, const char *delim)
{
	static char *current;
	if (str)
		current = str;

	if (!current)
		return 0;

	while (*current)
	{
		const char *d = delim;
		while (*d)
		{
			if (*current == *d)
				break;
			++d;
		}
		if (!*d)
			break;
		++current;
	}

	if (!*current)
		return 0;

	char *token_start = current;

	while (*current)
	{
		const char *d = delim;
		while (*d)
		{
			if (*current == *d)
				break;
			++d;
		}
		if (*d)
		{
			*current = '\0';
			++current;
			return token_start;
		}
		++current;
	}

	return token_start;
}

CRT_API int CRT_CALL atoi(const char *str)
{
	int result = 0;
	int sign = 1;

	while (*str == ' ' || *str == '\t' || *str == '\n')
		++str;

	if (*str == '-')
	{
		sign = -1;
		++str;
	}
	else if (*str == '+')
		++str;

	while (*str >= '0' && *str <= '9')
	{
		result = result * 10 + (*str - '0');
		++str;
	}

	return result * sign;
}

CRT_API char *CRT_CALL strtok_s(char *str, const char *delim, char **context)
{
	if (!str)
		str = *context;

	if (!str)
		return nullptr;

	while (*str && strchr(delim, *str))
		++str;

	if (*str == '\0')
	{
		*context = nullptr;
		return nullptr;
	}

	char *token_start = str;

	while (*str && !strchr(delim, *str))
		++str;

	if (*str)
	{
		*str = '\0';
		*context = str + 1;
	}
	else
	{
		*context = nullptr;
	}

	return token_start;
}

CRT_API size_t CRT_CALL wcslen(const wchar_t *s)
{
	if (!s)
		return 0;

	const wchar_t *p = s;
	while (*p) ++p;
	return static_cast<size_t>(p - s);
}

CRT_API errno_t CRT_CALL wcscpy_s(wchar_t *dst, size_t size, const wchar_t *src)
{
	if (!dst || !src || size == 0)
		return EINVAL;

	size_t i = 0;
	while (src[i] != 0)
	{
		if (i >= size - 1)
		{
			dst[0] = 0;
			return ERANGE;
		}
		dst[i] = src[i];
		++i;
	}

	dst[i] = 0;
	return 0;
}

CRT_API errno_t CRT_CALL wcscat_s(wchar_t *dst, size_t size, const wchar_t *src)
{
	if (!dst || !src || size == 0)
		return EINVAL;

	size_t len = wcslen(dst);
	size_t i = 0;

	while (src[i] != 0)
	{
		if (len + i >= size - 1)
		{
			dst[0] = 0;
			return ERANGE;
		}
		dst[len + i] = src[i];
		++i;
	}

	dst[len + i] = 0;
	return 0;
}

CRT_API errno_t CRT_CALL wcsncpy_s(wchar_t *dst, size_t size, const wchar_t *src, size_t count)
{
	if (!dst || !src || size == 0)
		return EINVAL;

	size_t i = 0;
	while (i < count && src[i] != 0)
	{
		if (i >= size - 1)
		{
			dst[0] = 0;
			return ERANGE;
		}
		dst[i] = src[i];
		++i;
	}

	if (i >= size)
	{
		dst[0] = 0;
		return ERANGE;
	}

	dst[i] = 0;
	return 0;
}

CRT_API unsigned long long CRT_CALL strtoull(const char *_String, char **_EndPtr, int _Radix)
{
	const char *p = _String;
	unsigned long long result = 0;

	while (*p == ' ' || *p == '\t')
		++p;

	if (*p == '+' || *p == '-')
		++p;

	if ((_Radix == 0 || _Radix == 16) && p[0] == '0' && (p[1] == 'x' || p[1] == 'X'))
	{
		_Radix = 16;
		p += 2;
	}
	else if (_Radix == 0 && p[0] == '0')
	{
		_Radix = 8;
		++p;
	}
	else if (_Radix == 0)
	{
		_Radix = 10;
	}

	const char *start = p;
	while (*p)
	{
		unsigned char digit;
		if (*p >= '0' && *p <= '9')
		{
			digit = *p - '0';
		}
		else if (*p >= 'a' && *p <= 'z')
		{
			digit = *p - 'a' + 10;
		}
		else if (*p >= 'A' && *p <= 'Z')
		{
			digit = *p - 'A' + 10;
		}
		else
		{
			break;
		}

		if (digit >= _Radix)
			break;

		result = result * _Radix + digit;
		++p;
	}

	if (_EndPtr)
		*_EndPtr = (char *)(p == start ? _String : p);

	return result;
}

CRT_API unsigned long CRT_CALL strtoul(const char *_String, char **_EndPtr, int _Radix)
{
	return (unsigned long)strtoull(_String, _EndPtr, _Radix);
}

CRT_API long long CRT_CALL strtoll(const char *_String, char **_EndPtr, int _Radix)
{
	const char *p = _String;
	long long result = 0;
	int negative = 0;

	while (*p == ' ' || *p == '\t')
		++p;

	if (*p == '+')
	{
		++p;
	}
	else if (*p == '-')
	{
		negative = 1;
		++p;
	}

	if ((_Radix == 0 || _Radix == 16) && p[0] == '0' && (p[1] == 'x' || p[1] == 'X'))
	{
		_Radix = 16;
		p += 2;
	}
	else if (_Radix == 0 && p[0] == '0')
	{
		_Radix = 8;
		++p;
	}
	else if (_Radix == 0)
	{
		_Radix = 10;
	}

	const char *start = p;
	while (*p)
	{
		unsigned char digit;
		if (*p >= '0' && *p <= '9')
		{
			digit = *p - '0';
		}
		else if (*p >= 'a' && *p <= 'z')
		{
			digit = *p - 'a' + 10;
		}
		else if (*p >= 'A' && *p <= 'Z')
		{
			digit = *p - 'A' + 10;
		}
		else
		{
			break;
		}

		if (digit >= _Radix)
			break;

		result = result * _Radix + digit;
		++p;
	}

	if (_EndPtr)
		*_EndPtr = (char *)(p == start ? _String : p);

	return negative ? -result : result;
}

CRT_API long CRT_CALL strtol(const char *_String, char **_EndPtr, int _Radix)
{
	return (long)strtoll(_String, _EndPtr, _Radix);
}

CRT_API const wchar_t *CRT_CALL wcsrchr(wchar_t const *_Str, wchar_t _Ch)
{
	const wchar_t *last = nullptr;

	while (*_Str)
	{
		if (*_Str == _Ch)
			last = _Str;
		++_Str;
	}

	if (_Ch == L'\0')
		return _Str;

	return last;
}

CRT_API int CRT_CALL _wcsicmp(wchar_t const *_String1, wchar_t const *_String2)
{
	wchar_t c1, c2;

	while (*_String1 && *_String2)
	{
		c1 = *_String1;
		c2 = *_String2;

		if (c1 >= L'a' && c1 <= L'z') 
			c1 -= 32;

		if (c2 >= L'a' && c2 <= L'z') 
			c2 -= 32;

		if (c1 != c2)
			return (int)(unsigned short)c1 - (int)(unsigned short)c2;

		_String1++;
		_String2++;
	}

	c1 = *_String1;
	c2 = *_String2;

	if (c1 >= L'a' && c1 <= L'z') 
		c1 -= 32;

	if (c2 >= L'a' && c2 <= L'z') 
		c2 -= 32;

	return (int)(unsigned short)c1 - (int)(unsigned short)c2;
}

CRT_API wchar_t *CRT_CALL wcscpy(wchar_t *_Destination, wchar_t const *_Source)
{
	wchar_t *ret = _Destination;
	while ((*_Destination++ = *_Source++) != L'\0') {}
	return ret;
}

CRT_API size_t CRT_CALL mbstowcs(wchar_t *_Dest, const char *_Source, size_t _MaxCount)
{
	size_t count = 0;

	if (!_Source)
		return (size_t)-1;

	if (!_Dest)
	{
		while (_Source[count] != '\0')
			count++;
		return count;
	}

	while (count < _MaxCount)
	{
		unsigned char ch = (unsigned char)_Source[count];
		_Dest[count] = (wchar_t)ch;

		if (ch == '\0')
			break;

		count++;
	}

	return count;
}

CRT_API int CRT_CALL stricmp(const char *str1, const char *str2)
{
	unsigned char c1, c2;

	while (*str1 && *str2)
	{
		c1 = static_cast<unsigned char>(*str1);
		c2 = static_cast<unsigned char>(*str2);

		if (c1 >= 'a' && c1 <= 'z') c1 -= 'a' - 'A';
		if (c2 >= 'a' && c2 <= 'z') c2 -= 'a' - 'A';

		if (c1 != c2)
			return c1 - c2;

		++str1;
		++str2;
	}

	c1 = static_cast<unsigned char>(*str1);
	c2 = static_cast<unsigned char>(*str2);

	if (c1 >= 'a' && c1 <= 'z') c1 -= 'a' - 'A';
	if (c2 >= 'a' && c2 <= 'z') c2 -= 'a' - 'A';

	return c1 - c2;
}

CRT_API int CRT_CALL strnicmp(const char *str1, const char *str2, size_t maxCount)
{
	unsigned char c1, c2;

	while (maxCount-- && *str1 && *str2)
	{
		c1 = static_cast<unsigned char>(*str1);
		c2 = static_cast<unsigned char>(*str2);

		if (c1 >= 'a' && c1 <= 'z') c1 -= 'a' - 'A';
		if (c2 >= 'a' && c2 <= 'z') c2 -= 'a' - 'A';

		if (c1 != c2)
			return c1 - c2;

		++str1;
		++str2;
	}
	
	if (maxCount == static_cast<size_t>(-1))
		return 0;

	c1 = static_cast<unsigned char>(*str1);
	c2 = static_cast<unsigned char>(*str2);

	if (c1 >= 'a' && c1 <= 'z') c1 -= 'a' - 'A';
	if (c2 >= 'a' && c2 <= 'z') c2 -= 'a' - 'A';
	
	return c1 - c2;
}

CRT_API errno_t CRT_CALL strcat_s(char *dst, rsize_t size, const char *src)
{
	if (!dst || !src || size == 0)
		return EINVAL;

	size_t len = strlen(dst);
	size_t i = 0;

	while (src[i] != '\0')
	{
		if (len + i >= size - 1)
		{
			dst[0] = '\0';
			return ERANGE;
		}

		dst[len + i] = src[i];
		++i;
	}

	dst[len + i] = '\0';
	return 0;
}

CRT_API errno_t CRT_CALL strncat_s(char *dst, rsize_t size, const char *src, rsize_t count)
{
	if (!dst || !src || size == 0)
		return EINVAL;

	size_t len = strlen(dst);
	size_t i = 0;

	while (i < count && src[i] != '\0')
	{
		if (len + i >= size - 1)
		{
			dst[0] = '\0';
			return ERANGE;
		}

		dst[len + i] = src[i];
		++i;
	}

	dst[len + i] = '\0';
	return 0;
}

CRT_API char *CRT_CALL itoa(int value, char *buffer, int radix)
{
	if (!buffer || radix < 2 || radix > 36)
		return nullptr;

	char *ptr = buffer;
	char *start = buffer;
	unsigned int uvalue = value;
	bool negative = false;

	if (radix == 10 && value < 0)
	{
		negative = true;
		uvalue = static_cast<unsigned int>(-value);
	}

	do
	{
		int digit = uvalue % radix;
		*ptr++ = static_cast<char>((digit < 10) ? ('0' + digit) : ('a' + digit - 10));
		uvalue /= radix;
	} while (uvalue);

	if (negative)
		*ptr++ = '-';

	*ptr = '\0';

	for (--ptr; start < ptr; ++start, --ptr)
	{
		char tmp = *start;
		*start = *ptr;
		*ptr = tmp;
	}

	return buffer;
}

CRT_API char *CRT_CALL _itoa(int _Value, char *_Buffer, int _Radix)
{
	if (!_Buffer || _Radix < 2 || _Radix > 36)
		return nullptr;

	char *ptr = _Buffer;
	char *start = _Buffer;
	unsigned int uvalue = _Value;
	bool negative = false;

	if (_Radix == 10 && _Value < 0)
	{
		negative = true;
		uvalue = static_cast<unsigned int>(-_Value);
	}

	do
	{
		int digit = uvalue % _Radix;
		*ptr++ = static_cast<char>((digit < 10) ? ('0' + digit) : ('a' + digit - 10));
		uvalue /= _Radix;
	} while (uvalue);

	if (negative)
		*ptr++ = '-';

	*ptr = '\0';

	for (--ptr; start < ptr; ++start, --ptr)
	{
		char tmp = *start;
		*start = *ptr;
		*ptr = tmp;
	}

	return _Buffer;
}

CRT_API char *CRT_CALL _ultoa(unsigned long _Value, char *_Buffer, int _Radix)
{
	if (!_Buffer || _Radix < 2 || _Radix > 36)
		return nullptr;

	char *ptr = _Buffer;
	char *start = _Buffer;

	do
	{
		int digit = static_cast<int>(_Value % _Radix);
		*ptr++ = static_cast<char>((digit < 10) ? ('0' + digit) : ('a' + digit - 10));
		_Value /= _Radix;
	} while (_Value);

	*ptr = '\0';

	for (--ptr; start < ptr; ++start, --ptr)
	{
		char tmp = *start;
		*start = *ptr;
		*ptr = tmp;
	}

	return _Buffer;
}

CRT_API char *CRT_CALL _i64toa(__int64 _Value, char *_Buffer, int _Radix)
{
	if (!_Buffer || _Radix < 2 || _Radix > 36)
		return nullptr;

	char *ptr = _Buffer;
	char *start = _Buffer;
	unsigned __int64 uvalue = static_cast<unsigned __int64>(_Value);
	bool negative = false;

	if (_Radix == 10 && _Value < 0)
	{
		negative = true;
		uvalue = static_cast<unsigned __int64>(-_Value);
	}

	do
	{
		int digit = static_cast<int>(uvalue % _Radix);
		*ptr++ = static_cast<char>((digit < 10) ? ('0' + digit) : ('a' + digit - 10));
		uvalue /= _Radix;
	} while (uvalue);

	if (negative)
		*ptr++ = '-';

	*ptr = '\0';

	for (--ptr; start < ptr; ++start, --ptr)
	{
		char tmp = *start;
		*start = *ptr;
		*ptr = tmp;
	}

	return _Buffer;
}

CRT_API char *CRT_CALL _ui64toa(unsigned __int64 _Value, char *_Buffer, int _Radix)
{
	if (!_Buffer || _Radix < 2 || _Radix > 36)
		return nullptr;

	char *ptr = _Buffer;
	char *start = _Buffer;

	do
	{
		int digit = static_cast<int>(_Value % _Radix);
		*ptr++ = static_cast<char>((digit < 10) ? ('0' + digit) : ('a' + digit - 10));
		_Value /= _Radix;
	} while (_Value);

	*ptr = '\0';

	for (--ptr; start < ptr; ++start, --ptr)
	{
		char tmp = *start;
		*start = *ptr;
		*ptr = tmp;
	}

	return _Buffer;
}