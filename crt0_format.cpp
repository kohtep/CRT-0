#include "crt0_common.hpp"

#include <Windows.h>
#include <stdio.h>

static bool IsNan(double val)
{
	union
	{
		double d;
		uint64_t u;
	} db;

	db.d = val;
	uint64_t exp = (db.u >> 52) & 0x7FF;
	uint64_t frac = db.u & 0xFFFFFFFFFFFFFull;
	return (exp == 0x7FF) && (frac != 0);
}

static bool IsInf(double val)
{
	union
	{
		double d;
		uint64_t u;
	} db;

	db.d = val;
	uint64_t exp = (db.u >> 52) & 0x7FF;
	uint64_t frac = db.u & 0xFFFFFFFFFFFFFull;
	return (exp == 0x7FF) && (frac == 0);
}

static bool IsNan(long double val)
{
	union
	{
		long double ld;
		struct
		{
			uint64_t mantissa;
			uint16_t exponent;
			uint16_t pad[3];
		} parts;
	} ld_union = {};

	ld_union.ld = val;

	uint16_t exp = ld_union.parts.exponent & 0x7FFF;
	uint64_t frac = ld_union.parts.mantissa & 0x7FFFFFFFFFFFFFFF;

	return (exp == 0x7FFF) && (frac != 0);
}

static bool IsInf(long double val)
{
	union
	{
		long double ld;
		struct
		{
			uint64_t mantissa;
			uint16_t exponent;
			uint16_t pad[3];
		} parts;
	} ld_union = {};

	ld_union.ld = val;

	uint16_t exp = ld_union.parts.exponent & 0x7FFF;
	uint64_t frac = ld_union.parts.mantissa & 0x7FFFFFFFFFFFFFFF;

	return (exp == 0x7FFF) && (frac == 0);
}

static int Int64ToStr(char *lpBuffer, long long nValue)
{
	char *p = lpBuffer;
	bool bNegative = false;

	if (nValue < 0)
	{
		bNegative = true;
		nValue = -nValue;
	}

	char temp[21];
	int i = 0;

	do
	{
		temp[i++] = '0' + (nValue % 10);
		nValue /= 10;
	} while (nValue > 0);

	if (bNegative)
		temp[i++] = '-';

	while (i-- > 0)
		*p++ = temp[i];

	*p = '\0';
	return static_cast<int>(p - lpBuffer);
}

static int IntToStr(char *lpBuffer, int nValue)
{
	char *p = lpBuffer;
	bool bNegative = false;

	if (nValue < 0)
	{
		bNegative = true;
		nValue = -nValue;
	}

	char temp[12];
	int i = 0;

	do
	{
		temp[i++] = '0' + (nValue % 10);
		nValue /= 10;
	} while (nValue > 0);

	if (bNegative)
		temp[i++] = '-';

	while (i-- > 0)
		*p++ = temp[i];

	*p = '\0';
	return static_cast<int>(p - lpBuffer);
}

static int UInt64ToStr(char *lpBuffer, unsigned long long uValue)
{
	char *p = lpBuffer;
	char temp[21];
	int i = 0;

	do {
		temp[i++] = '0' + (uValue % 10);
		uValue /= 10;
	} while (uValue > 0);

	while (i-- > 0)
		*p++ = temp[i];

	*p = '\0';
	return static_cast<int>(p - lpBuffer);
}

static int UIntToStr(char *lpBuffer, unsigned int uValue)
{
	char *p = lpBuffer;
	char temp[11];
	int i = 0;

	do
	{
		temp[i++] = '0' + (uValue % 10);
		uValue /= 10;
	} while (uValue > 0);

	while (i-- > 0)
		*p++ = temp[i];

	*p = '\0';
	return static_cast<int>(p - lpBuffer);
}

static int UInt64ToHex(char *lpBuffer, unsigned long long uValue, bool uppercase)
{
	volatile char hexDigits[17];

	if (uppercase)
	{
		*(uint32_t *)&hexDigits[0] = '3210';
		*(uint32_t *)&hexDigits[4] = '7654';
		*(uint32_t *)&hexDigits[8] = 'BA98';
		*(uint32_t *)&hexDigits[12] = 'FEDC';
	}
	else
	{
		*(uint32_t *)&hexDigits[0] = '3210';
		*(uint32_t *)&hexDigits[4] = '7654';
		*(uint32_t *)&hexDigits[8] = 'ba98';
		*(uint32_t *)&hexDigits[12] = 'fedc';
	}

	char *p = lpBuffer;
	char temp[17];
	int i = 0;

	do
	{
		unsigned digit = uValue % 16;
		uValue /= 16;

		if (digit < 10)
			temp[i++] = hexDigits[digit];
		else
			temp[i++] = hexDigits[digit];
	} while (uValue > 0);

	while (i-- > 0)
		*p++ = temp[i];

	*p = '\0';
	return static_cast<int>(p - lpBuffer);
}

static int DoubleToStr(char *lpBuffer, double value, int precision)
{
	char *p = lpBuffer;

	if (IsNan(value))
	{
		*p++ = 'n'; *p++ = 'a'; *p++ = 'n';
		*p = '\0';
		return 3;
	}

	if (IsInf(value))
	{
		if (value < 0) *p++ = '-';
		*p++ = 'i'; *p++ = 'n'; *p++ = 'f';
		*p = '\0';
		return static_cast<int>(p - lpBuffer);
	}

	if (value < 0)
	{
		*p++ = '-';
		value = -value;
	}

	long long intPart = static_cast<long long>(value);
	p += Int64ToStr(p, intPart);

	*p++ = '.';

	double frac = value - intPart;

	for (int i = 0; i < precision; ++i)
	{
		frac *= 10.0;
		int digit = static_cast<int>(frac);
		*p++ = '0' + digit;
		frac -= digit;
	}

	*p = '\0';
	return static_cast<int>(p - lpBuffer);
}

static int LongDoubleToStr(char *lpBuffer, long double value, int precision)
{
	char *p = lpBuffer;

	if (IsNan(value)) {
		*p++ = 'n'; *p++ = 'a'; *p++ = 'n';
		*p = '\0';
		return 3;
	}

	if (IsInf(value)) {
		if (value < 0) *p++ = '-';
		*p++ = 'i'; *p++ = 'n'; *p++ = 'f';
		*p = '\0';
		return static_cast<int>(p - lpBuffer);
	}

	if (value < 0) {
		*p++ = '-';
		value = -value;
	}

	long long intPart = static_cast<long long>(value);
	p += Int64ToStr(p, intPart);

	*p++ = '.';

	long double frac = value - intPart;

	for (int i = 0; i < precision; ++i) {
		frac *= 10.0;
		int digit = static_cast<int>(frac);
		*p++ = '0' + digit;
		frac -= digit;
	}

	*p = '\0';
	return static_cast<int>(p - lpBuffer);
}

struct FormatModifiers
{
	bool isShort : 1;
	bool isChar : 1;
	bool isLong : 1;
	bool isLongLong : 1;
	bool isIntMax : 1;
	bool isSizeT : 1;
	bool isPtrdiff : 1;
	bool isLongDouble : 1;

	bool hasWidth : 1;
	unsigned int width;

	bool hasPrecision : 1;
	unsigned int precision;

	char paddingChar = ' ';
};

using FormatHandler = int (*)(char *&pOut, size_t &remaining, va_list &args, const FormatModifiers &modifiers);

struct FormatSpecHandler
{
	char specifier;
	FormatHandler handler;
};

static int HandleDecimal(char *&pOut, size_t &remaining, va_list &args, const FormatModifiers &modifiers)
{
	char temp[21];
	int len = 0;

	if (modifiers.isLongLong)
		len = Int64ToStr(temp, va_arg(args, long long));
	else if (modifiers.isLong)
		len = Int64ToStr(temp, static_cast<long long>(va_arg(args, long)));
	else if (modifiers.isShort)
		len = IntToStr(temp, static_cast<short>(va_arg(args, int)));
	else if (modifiers.isChar)
		len = IntToStr(temp, static_cast<signed char>(va_arg(args, int)));
	else
		len = IntToStr(temp, va_arg(args, int));

	if (modifiers.hasWidth && len < modifiers.width) {
		size_t pad = modifiers.width - len;
		while (pad-- && remaining > 0)
			*pOut++ = modifiers.paddingChar, --remaining;
	}

	for (int i = 0; i < len && remaining > 0; ++i, --remaining)
		*pOut++ = temp[i];

	return 0;
}

static int HandleUnsigned(char *&pOut, size_t &remaining, va_list &args, const FormatModifiers &modifiers)
{
	char temp[21];
	unsigned int len = 0;

	if (modifiers.isLongLong)
		len = UInt64ToStr(temp, va_arg(args, unsigned long long));
	else if (modifiers.isLong)
		len = UInt64ToStr(temp, static_cast<uint64_t>(va_arg(args, unsigned long)));
	else if (modifiers.isShort)
		len = UIntToStr(temp, static_cast<unsigned short>(va_arg(args, unsigned int)));
	else if (modifiers.isChar)
		len = UIntToStr(temp, static_cast<unsigned char>(va_arg(args, unsigned int)));
	else
		len = UIntToStr(temp, va_arg(args, unsigned int));

	if (modifiers.hasWidth && len < modifiers.width) {
		size_t pad = modifiers.width - len;
		while (pad-- && remaining > 0)
			*pOut++ = modifiers.paddingChar, --remaining;
	}

	for (unsigned int i = 0; i < len && remaining > 0; ++i, --remaining)
		*pOut++ = temp[i];

	return 0;
}

static int HandleHexLower(char *&pOut, size_t &remaining, va_list &args, const FormatModifiers &modifiers)
{
	char temp[17];
	uint64_t val = 0;

	if (modifiers.isLongLong)
		val = va_arg(args, unsigned long long);
	else if (modifiers.isLong)
		val = va_arg(args, unsigned long);
	else if (modifiers.isShort)
		val = static_cast<unsigned short>(va_arg(args, unsigned int));
	else if (modifiers.isChar)
		val = static_cast<unsigned char>(va_arg(args, unsigned int));
	else
		val = va_arg(args, unsigned int);

	int len = UInt64ToHex(temp, val, false);

	if (modifiers.hasWidth && len < modifiers.width) {
		size_t pad = modifiers.width - len;
		while (pad-- && remaining > 0)
			*pOut++ = modifiers.paddingChar, --remaining;
	}

	for (int i = 0; i < len && remaining > 0; ++i, --remaining)
		*pOut++ = temp[i];

	return 0;
}

static int HandleHexUpper(char *&pOut, size_t &remaining, va_list &args, const FormatModifiers &modifiers)
{
	char temp[17];
	uint64_t value = 0;

	if (modifiers.isLongLong)
		value = va_arg(args, unsigned long long);
	else if (modifiers.isLong)
		value = static_cast<uint64_t>(va_arg(args, unsigned long));
	else if (modifiers.isShort)
		value = static_cast<unsigned short>(va_arg(args, unsigned int));
	else if (modifiers.isChar)
		value = static_cast<unsigned char>(va_arg(args, unsigned int));
	else
		value = va_arg(args, unsigned int);

	int len = UInt64ToHex(temp, value, true);

	if (modifiers.hasWidth && len < modifiers.width) {
		size_t pad = modifiers.width - len;
		while (pad-- && remaining > 0)
			*pOut++ = modifiers.paddingChar, --remaining;
	}

	for (int i = 0; i < len && remaining > 0; ++i, --remaining)
		*pOut++ = temp[i];

	return 0;
}

static int HandleString(char *&pOut, size_t &remaining, va_list &args, const FormatModifiers &modifiers)
{
	volatile const char null[] = { '(', 'n', 'u', 'l', 'l', ')', '\0' };

	const char *str = va_arg(args, const char *);

	if (!str)
		str = const_cast<const char *>(null);

	size_t len = strlen(str);

	if (modifiers.hasWidth && len < modifiers.width) {
		size_t pad = modifiers.width - len;
		while (pad-- && remaining > 0)
			*pOut++ = modifiers.paddingChar, --remaining;
	}

	for (size_t i = 0; str[i] && remaining > 0; ++i, --remaining)
		*pOut++ = str[i];

	return 0;
}

static int HandleChar(char *&pOut, size_t &remaining, va_list &args, const FormatModifiers &modifiers)
{
	char ch = static_cast<char>(va_arg(args, int));

	if (modifiers.hasWidth && 1 < modifiers.width) {
		size_t pad = modifiers.width - 1;
		while (pad-- && remaining > 0)
			*pOut++ = modifiers.paddingChar, --remaining;
	}

	if (remaining > 0) {
		*pOut++ = ch;
		--remaining;
	}

	return 0;
}

static int HandlePointer(char *&pOut, size_t &remaining, va_list &args, const FormatModifiers &modifiers)
{
	uintptr_t ptr = reinterpret_cast<uintptr_t>(va_arg(args, void *));
	char temp[2 + sizeof(uintptr_t) * 2];
	int len = UInt64ToHex(temp, ptr, false);

	if (modifiers.hasWidth && len < modifiers.width) {
		size_t pad = modifiers.width - len;
		while (pad-- && remaining > 0)
			*pOut++ = modifiers.paddingChar, --remaining;
	}

	for (int i = 0; i < len && remaining > 0; ++i, --remaining)
		*pOut++ = temp[i];

	return 0;
}

static int HandleFloat(char *&pOut, size_t &remaining, va_list &args, const FormatModifiers &modifiers)
{
	char temp[64];
	int len = modifiers.isLongDouble ?
		LongDoubleToStr(temp, va_arg(args, long double), modifiers.hasPrecision ? modifiers.precision : 6) :
		DoubleToStr(temp, va_arg(args, double), modifiers.hasPrecision ? modifiers.precision : 6);

	if (modifiers.hasWidth && len < modifiers.width) {
		size_t pad = modifiers.width - len;
		while (pad-- && remaining > 0)
			*pOut++ = modifiers.paddingChar, --remaining;
	}

	for (int i = 0; i < len && remaining > 0; ++i, --remaining)
		*pOut++ = temp[i];

	return 0;
}

static int HandleTime(char *&pOut, size_t &remaining, va_list &, const FormatModifiers &)
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	char sep[4] = ":";
	GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_STIME, sep, (DWORD)sizeof(sep));

	char timeFmt[2] = "1";
	GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_ITIME, timeFmt, (DWORD)sizeof(timeFmt));
	bool is24Hour = (timeFmt[0] == '1');

	int hour = st.wHour;
	bool isPM = false;

	if (!is24Hour)
	{
		if (hour == 0)
		{
			hour = 12;
		}
		else if (hour >= 12)
		{
			if (hour > 12)
				hour -= 12;

			isPM = true;
		}
	}

	char temp[16];
	int i = 0;

	temp[i++] = '0' + ((hour / 10) % 10);
	temp[i++] = '0' + (hour % 10);
	temp[i++] = sep[0];
	temp[i++] = '0' + ((st.wMinute / 10) % 10);
	temp[i++] = '0' + (st.wMinute % 10);
	temp[i++] = sep[0];
	temp[i++] = '0' + ((st.wSecond / 10) % 10);
	temp[i++] = '0' + (st.wSecond % 10);

	if (!is24Hour) {
		temp[i++] = ' ';
		temp[i++] = isPM ? 'P' : 'A';
		temp[i++] = 'M';
	}

	temp[i] = '\0';

	size_t len = strlen(temp);
	if (len > remaining)
		len = remaining;

	memcpy(pOut, temp, len);
	pOut += len;
	remaining -= len;

	return 0;
}

static int HandleDate(char *&pOut, size_t &remaining, va_list &, const FormatModifiers &)
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	char sep[4] = "/";
	GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_SDATE, sep, (DWORD)sizeof(sep));

	char orderStr[2] = "0";
	GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_IDATE, orderStr, (DWORD)sizeof(orderStr));
	int order = orderStr[0] - '0'; // 0 = MDY, 1 = DMY, 2 = YMD

	char temp[16];
	int i = 0;

	char m1 = '0' + ((st.wMonth / 10) % 10);
	char m2 = '0' + (st.wMonth % 10);
	char d1 = '0' + ((st.wDay / 10) % 10);
	char d2 = '0' + (st.wDay % 10);
	char y1 = '0' + ((st.wYear / 1000) % 10);
	char y2 = '0' + ((st.wYear / 100) % 10);
	char y3 = '0' + ((st.wYear / 10) % 10);
	char y4 = '0' + (st.wYear % 10);

	if (order == 0) { // MDY
		temp[i++] = m1; temp[i++] = m2;
		temp[i++] = sep[0];
		temp[i++] = d1; temp[i++] = d2;
		temp[i++] = sep[0];
		temp[i++] = y1; temp[i++] = y2;
		temp[i++] = y3; temp[i++] = y4;
	}
	else if (order == 1) { // DMY
		temp[i++] = d1; temp[i++] = d2;
		temp[i++] = sep[0];
		temp[i++] = m1; temp[i++] = m2;
		temp[i++] = sep[0];
		temp[i++] = y1; temp[i++] = y2;
		temp[i++] = y3; temp[i++] = y4;
	}
	else { // YMD
		temp[i++] = y1; temp[i++] = y2;
		temp[i++] = y3; temp[i++] = y4;
		temp[i++] = sep[0];
		temp[i++] = m1; temp[i++] = m2;
		temp[i++] = sep[0];
		temp[i++] = d1; temp[i++] = d2;
	}

	temp[i] = '\0';

	size_t len = strlen(temp);
	if (len > remaining)
		len = remaining;

	memcpy(pOut, temp, len);
	pOut += len;
	remaining -= len;

	return 0;
}

static int HandleLastError(char *&pOut, size_t &remaining, va_list &, const FormatModifiers &)
{
	DWORD errorCode = GetLastError();

	char temp[512];

	int len = FormatMessageA(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		temp,
		DWORD(sizeof(temp) - 1),
		nullptr);

	if (len > 0)
	{
		while (len > 0 && (temp[len - 1] == '\n' || temp[len - 1] == '\r'))
			--len;

		temp[len] = '\0';
	}
	else
	{
		char prefix[] = { 'U','n','k','n','o','w','n',' ','e','r','r','o','r',' ','(','\0' };
		char suffix[] = { ')','\0' };
		char numBuf[16];
		int numLen = UIntToStr(numBuf, errorCode);

		len = 0;

		for (int i = 0; prefix[i] != '\0'; ++i)
			temp[len++] = prefix[i];

		for (int i = 0; i < numLen; ++i)
			temp[len++] = numBuf[i];

		for (int i = 0; suffix[i] != '\0'; ++i)
			temp[len++] = suffix[i];

		temp[len] = '\0';
	}

	if (static_cast<size_t>(len) > remaining)
		len = static_cast<int>(remaining);

	memcpy(pOut, temp, len);
	pOut += len;
	remaining -= len;

	return 0;
}

static FormatSpecHandler g_formatHandlers[] =
{
	{ 'd', HandleDecimal },
	{ 'i', HandleDecimal },
	{ 'u', HandleUnsigned },
	{ 'x', HandleHexLower },
	{ 'X', HandleHexUpper },
	{ 's', HandleString },
	{ 'c', HandleChar },
	{ 'p', HandlePointer },
	{ 'f', HandleFloat },
	{ 0, nullptr }
};

static FormatHandler FindHandler(char spec)
{
	for (int i = 0; g_formatHandlers[i].specifier != 0; ++i)
	{
		if (g_formatHandlers[i].specifier == spec)
		{
			return g_formatHandlers[i].handler;
		}
	}

	return nullptr;
}

static FormatModifiers ParseFormatModifiers(const char *&format)
{
	FormatModifiers modifiers = {};
	modifiers.paddingChar = ' ';

	if (*format == '0')
	{
		modifiers.paddingChar = '0';
		++format;
	}

	if (*format >= '0' && *format <= '9')
	{
		modifiers.hasWidth = true;
		modifiers.width = 0;

		while (*format >= '0' && *format <= '9')
		{
			modifiers.width = modifiers.width * 10 + (*format - '0');
			++format;
		}
	}

	if (*format == '.')
	{
		++format;
		modifiers.hasPrecision = true;
		modifiers.precision = 0;

		while (*format >= '0' && *format <= '9')
		{
			modifiers.precision = modifiers.precision * 10 + (*format - '0');
			++format;
		}
	}

	while (true)
	{
		if (*format == 'h')
		{
			if (*(format + 1) == 'h')
			{
				modifiers.isChar = true;
				format += 2;
			}
			else
			{
				modifiers.isShort = true;
				++format;
			}
		}
		else if (*format == 'l')
		{
			if (*(format + 1) == 'l')
			{
				modifiers.isLongLong = true;
				format += 2;
			}
			else
			{
				modifiers.isLong = true;
				++format;
			}
		}
		else if (*format == 'j')
		{
			modifiers.isIntMax = true;
			++format;
		}
		else if (*format == 'z')
		{
			modifiers.isSizeT = true;
			++format;
		}
		else if (*format == 't')
		{
			modifiers.isPtrdiff = true;
			++format;
		}
		else if (*format == 'L')
		{
			modifiers.isLongDouble = true;
			++format;
		}
		else
		{
			break;
		}
	}

	return modifiers;
}

int FormatBufSafeV(char *lpBuffer, size_t dwMaxSize, const char *lpFormat, va_list args)
{
	if (lpBuffer == nullptr || lpFormat == nullptr || dwMaxSize == 0)
		return 0;

	char *pOut = lpBuffer;
	size_t remaining = (dwMaxSize == static_cast<size_t>(-1)) ? static_cast<size_t>(-1) : dwMaxSize - 1;

	while (*lpFormat && remaining > 0)
	{
		if (*lpFormat == '%')
		{
			++lpFormat;

			FormatModifiers modifiers = ParseFormatModifiers(lpFormat);

			if (!*lpFormat)
				break;

			if (*lpFormat == '%')
			{
				*pOut++ = '%';
				--remaining;
			}
			else if (*lpFormat == 'n')
			{
				//assert(false && "%%n is disabled for safety reasons");
			}
			else
			{
				FormatHandler handler = FindHandler(*lpFormat);
				if (handler)
				{
					handler(pOut, remaining, args, modifiers);
				}
				else
				{
					//assert(false && "Unknown format specifier");

					if (remaining > 0)
					{
						*pOut++ = '%';
						--remaining;
					}
					if (remaining > 0)
					{
						*pOut++ = *lpFormat;
						--remaining;
					}
				}
			}

			++lpFormat;
		}
		else
		{
			*pOut++ = *lpFormat++;
			--remaining;
		}
	}

	*pOut = '\0';
	return static_cast<int>(pOut - lpBuffer);
}

int FormatBufSafe(char *lpBuffer, size_t dwMaxSize, const char *lpFormat, ...)
{
	va_list args;
	va_start(args, lpFormat);
	int result = FormatBufSafeV(lpBuffer, dwMaxSize, lpFormat, args);
	va_end(args);
	return result;
}

static int va_current = 0;
static char va_string[16][1024];

char *VFormatBufSafe(const char *lpFormat, ...)
{
	va_list argptr;
	va_current = (va_current + 1) % 16;

	va_start(argptr, lpFormat);
	FormatBufSafe(va_string[va_current], _countof(va_string[va_current]), lpFormat, argptr);
	va_end(argptr);

	return va_string[va_current];
}

CRT_API int CRT_CALL __stdio_common_vfprintf(unsigned __int64 _Options, FILE *_Stream,
	char const *_Format, _locale_t _Locale, va_list _ArgList)
{
	HANDLE h = reinterpret_cast<HANDLE>(_Stream);

	char buffer[2048];
	int len = FormatBufSafeV(buffer, sizeof(buffer), _Format, _ArgList);
	if (len < 0)
		return -1;

	DWORD written = 0;

	DWORD mode = 0;
	if (GetConsoleMode(h, &mode))
		WriteConsoleA(h, buffer, len, &written, nullptr);
	else
		WriteFile(h, buffer, len, &written, nullptr);

	return static_cast<int>(written);
}

CRT_API int CRT_CALL __stdio_common_vsprintf(unsigned __int64 _Options, char *_Buffer,
	size_t _BufferCount, const char *_Format, _locale_t _Locale, va_list _ArgList)
{
	(void)_Options;
	(void)_Locale;

	if (!_Buffer || !_Format || _BufferCount == 0)
		return -1;

	return FormatBufSafeV(_Buffer, _BufferCount, _Format, _ArgList);
}