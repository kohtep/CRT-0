#include "crt0_common.hpp"

#include <Windows.h>
#include <stdio.h>

static size_t FormatFloatingPoint(char *buffer, long double value, int precision)
{
	char *p = buffer;

	if (isnan(value))
	{
		*p++ = 'n'; *p++ = 'a'; *p++ = 'n';
		*p = '\0';
		return 3;
	}

	if (isinf(value))
	{
		if (value < 0) *p++ = '-';
		*p++ = 'i'; *p++ = 'n'; *p++ = 'f';
		*p = '\0';
		return static_cast<size_t>(p - buffer);
	}

	if (value < 0)
	{
		*p++ = '-';
		value = -value;
	}

	long long intPart = static_cast<long long>(value);
	char intBuf[21];
	_i64toa(intPart, intBuf, 10);

	char *ip = intBuf;
	while (*ip)
	{
		*p++ = *ip++;
	}

	*p++ = '.';

	long double frac = value - static_cast<long long>(value);

	for (int j = 0; j < precision; ++j)
	{
		frac *= 10.0;
		int digit = static_cast<int>(frac);
		*p++ = '0' + digit;
		frac -= digit;
	}

	*p = '\0';
	return static_cast<size_t>(p - buffer);
}

static int DoubleToScientific(char *lpBuffer, double value, int precision, bool uppercase)
{
	char *p = lpBuffer;

	if (isnan(value))
	{
		*p++ = 'n';
		*p++ = 'a';
		*p++ = 'n';
		*p = '\0';
		return 3;
	}

	if (isinf(value))
	{
		if (value < 0)
		{
			*p++ = '-';
		}
		*p++ = uppercase ? 'I' : 'i';
		*p++ = uppercase ? 'N' : 'n';
		*p++ = uppercase ? 'F' : 'f';
		*p = '\0';
		return static_cast<int>(p - lpBuffer);
	}

	if (value < 0)
	{
		*p++ = '-';
		value = -value;
	}

	int exponent = 0;

	if (value != 0.0)
	{
		while (value >= 10.0)
		{
			value /= 10.0;
			++exponent;
		}
		while (value < 1.0)
		{
			value *= 10.0;
			--exponent;
		}
	}

	long long intPart = static_cast<long long>(value);
	char intBuf[21];
	_i64toa(intPart, intBuf, 10);

	char *ip = intBuf;
	while (*ip)
	{
		*p++ = *ip++;
	}

	*p++ = '.';

	double frac = value - static_cast<long long>(value);

	for (int i = 0; i < precision; ++i)
	{
		frac *= 10.0;
		int digit = static_cast<int>(frac);
		*p++ = '0' + digit;
		frac -= digit;
	}

	*p++ = uppercase ? 'E' : 'e';

	if (exponent >= 0)
	{
		*p++ = '+';
	}
	else
	{
		*p++ = '-';
		exponent = -exponent;
	}

	if (exponent >= 100)
	{
		*p++ = '0' + (exponent / 100);
		*p++ = '0' + ((exponent / 10) % 10);
		*p++ = '0' + (exponent % 10);
	}
	else
	{
		*p++ = '0' + ((exponent / 10) % 10);
		*p++ = '0' + (exponent % 10);
	}

	*p = '\0';
	return static_cast<int>(p - lpBuffer);
}

static int DoubleToGeneral(char *lpBuffer, double value, int precision, bool uppercase)
{
	char *p = lpBuffer;

	if (isnan(value))
	{
		*p++ = 'n';
		*p++ = 'a';
		*p++ = 'n';
		*p = '\0';
		return 3;
	}

	if (isinf(value))
	{
		if (value < 0)
		{
			*p++ = '-';
			*p++ = uppercase ? 'I' : 'i';
			*p++ = uppercase ? 'N' : 'n';
			*p++ = uppercase ? 'F' : 'f';
			*p = '\0';
			return static_cast<int>(p - lpBuffer);
		}

		*p++ = uppercase ? 'I' : 'i';
		*p++ = uppercase ? 'N' : 'n';
		*p++ = uppercase ? 'F' : 'f';
		*p = '\0';
		return static_cast<int>(p - lpBuffer);
	}

	if (value < 0.0)
	{
		*p++ = '-';
		value = -value;
	}

	int exponent = 0;
	double normalized = value;

	if (value != 0.0)
	{
		while (normalized >= 10.0)
		{
			normalized /= 10.0;
			++exponent;
		}
		while (normalized < 1.0)
		{
			normalized *= 10.0;
			--exponent;
		}
	}

	if (exponent < -4 || exponent >= precision)
	{
		int len = DoubleToScientific(p, value, precision - 1, uppercase);
		return static_cast<int>((p - lpBuffer) + len);
	}
	else
	{
		size_t len = FormatFloatingPoint(p, value, precision);

		for (int i = static_cast<int>((p - lpBuffer) + len) - 1; i > 0; --i)
		{
			if (lpBuffer[i] == '0')
			{
				lpBuffer[i] = '\0';
			}
			else if (lpBuffer[i] == '.')
			{
				lpBuffer[i] = '\0';
				break;
			}
			else
			{
				break;
			}
		}

		return static_cast<int>(strlen(lpBuffer));
	}
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
	size_t len = 0;

	if (modifiers.isLongLong)
	{
		len = strlen(_i64toa(va_arg(args, long long), temp, 10));
	}
	else if (modifiers.isLong)
	{
		len = strlen(_i64toa(static_cast<long long>(va_arg(args, long)), temp, 10));
	}
	else if (modifiers.isShort)
	{
		len = strlen(_itoa(static_cast<short>(va_arg(args, int)), temp, 10));
	}
	else if (modifiers.isChar)
	{
		len = strlen(_itoa(static_cast<signed char>(va_arg(args, int)), temp, 10));
	}
	else
	{
		len = strlen(_itoa(va_arg(args, int), temp, 10));
	}

	if (modifiers.hasWidth && len < modifiers.width)
	{
		size_t pad = modifiers.width - len;
		while (pad-- && remaining > 0)
		{
			*pOut++ = modifiers.paddingChar;
			--remaining;
		}
	}

	for (int i = 0; i < len && remaining > 0; ++i, --remaining)
	{
		*pOut++ = temp[i];
	}

	return 0;
}

static int HandleUnsigned(char *&pOut, size_t &remaining, va_list &args, const FormatModifiers &modifiers)
{
	char temp[21];
	size_t len = 0;

	if (modifiers.isLongLong)
	{
		len = strlen(_ui64toa(va_arg(args, unsigned long long), temp, 10));
	}
	else if (modifiers.isLong)
	{
		len = strlen(_ui64toa(static_cast<uint64_t>(va_arg(args, unsigned long)), temp, 10));
	}
	else if (modifiers.isShort)
	{
		len = strlen(_ultoa(static_cast<unsigned short>(va_arg(args, unsigned int)), temp, 10));
	}
	else if (modifiers.isChar)
	{
		len = strlen(_ultoa(static_cast<unsigned char>(va_arg(args, unsigned int)), temp, 10));
	}
	else
	{
		len = strlen(_ultoa(va_arg(args, unsigned int), temp, 10));
	}

	if (modifiers.hasWidth && len < modifiers.width)
	{
		size_t pad = modifiers.width - len;
		while (pad-- && remaining > 0)
		{
			*pOut++ = modifiers.paddingChar;
			--remaining;
		}
	}

	for (unsigned int i = 0; i < len && remaining > 0; ++i, --remaining)
	{
		*pOut++ = temp[i];
	}

	return 0;
}

static int HandleHexLower(char *&pOut, size_t &remaining, va_list &args, const FormatModifiers &modifiers)
{
	char temp[17];
	uint64_t val = 0;

	if (modifiers.isLongLong)
	{
		val = va_arg(args, unsigned long long);
	}
	else if (modifiers.isLong)
	{
		val = va_arg(args, unsigned long);
	}
	else if (modifiers.isShort)
	{
		val = static_cast<unsigned short>(va_arg(args, unsigned int));
	}
	else if (modifiers.isChar)
	{
		val = static_cast<unsigned char>(va_arg(args, unsigned int));
	}
	else
	{
		val = va_arg(args, unsigned int);
	}

	size_t len = strlen(_ui64toa(val, temp, 16));

	if (modifiers.hasWidth && len < modifiers.width)
	{
		size_t pad = modifiers.width - len;
		while (pad-- && remaining > 0)
		{
			*pOut++ = modifiers.paddingChar;
			--remaining;
		}
	}

	for (int i = 0; i < len && remaining > 0; ++i, --remaining)
	{
		*pOut++ = temp[i];
	}

	return 0;
}

static int HandleHexUpper(char *&pOut, size_t &remaining, va_list &args, const FormatModifiers &modifiers)
{
	char temp[17];
	uint64_t value = 0;

	if (modifiers.isLongLong)
	{
		value = va_arg(args, unsigned long long);
	}
	else if (modifiers.isLong)
	{
		value = static_cast<uint64_t>(va_arg(args, unsigned long));
	}
	else if (modifiers.isShort)
	{
		value = static_cast<unsigned short>(va_arg(args, unsigned int));
	}
	else if (modifiers.isChar)
	{
		value = static_cast<unsigned char>(va_arg(args, unsigned int));
	}
	else
	{
		value = va_arg(args, unsigned int);
	}

	size_t len = strlen(_ui64toa(value, temp, 16));

	for (unsigned int i = 0; i < len; ++i)
	{
		if (temp[i] >= 'a' && temp[i] <= 'z')
		{
			temp[i] -= 32;
		}
	}

	if (modifiers.hasWidth && len < modifiers.width)
	{
		size_t pad = modifiers.width - len;
		while (pad-- && remaining > 0)
		{
			*pOut++ = modifiers.paddingChar;
			--remaining;
		}
	}

	for (int i = 0; i < len && remaining > 0; ++i, --remaining)
	{
		*pOut++ = temp[i];
	}

	return 0;
}

static int HandleString(char *&pOut, size_t &remaining, va_list &args, const FormatModifiers &modifiers)
{
	volatile const char null[] = { '(', 'n', 'u', 'l', 'l', ')', '\0' };

	const char *str = va_arg(args, const char *);

	if (!str)
	{
		str = const_cast<const char *>(null);
	}

	size_t len = strlen(str);

	if (modifiers.hasWidth && len < modifiers.width)
	{
		size_t pad = modifiers.width - len;
		while (pad-- && remaining > 0)
		{
			*pOut++ = modifiers.paddingChar;
			--remaining;
		}
	}

	for (size_t i = 0; str[i] && remaining > 0; ++i, --remaining)
	{
		*pOut++ = str[i];
	}

	return 0;
}

static int HandleChar(char *&pOut, size_t &remaining, va_list &args, const FormatModifiers &modifiers)
{
	char ch = static_cast<char>(va_arg(args, int));

	if (modifiers.hasWidth && 1 < modifiers.width)
	{
		size_t pad = modifiers.width - 1;
		while (pad-- && remaining > 0)
			*pOut++ = modifiers.paddingChar, --remaining;
	}

	if (remaining > 0)
	{
		*pOut++ = ch;
		--remaining;
	}

	return 0;
}

static int HandlePointer(char *&pOut, size_t &remaining, va_list &args, const FormatModifiers &modifiers)
{
	uintptr_t ptr = reinterpret_cast<uintptr_t>(va_arg(args, void *));
	char temp[2 + sizeof(uintptr_t) * 2];

	size_t len = strlen(_ui64toa(ptr, temp, 16));

	if (modifiers.hasWidth && len < modifiers.width)
	{
		size_t pad = modifiers.width - len;
		while (pad-- && remaining > 0)
		{
			*pOut++ = modifiers.paddingChar;
			--remaining;
		}
	}

	for (int i = 0; i < len && remaining > 0; ++i, --remaining)
	{
		*pOut++ = temp[i];
	}

	return 0;
}

static int HandleFloat(char *&pOut, size_t &remaining, va_list &args, const FormatModifiers &modifiers)
{
	char temp[64];
	int precision = modifiers.hasPrecision ? modifiers.precision : 6;

	long double value = modifiers.isLongDouble
		? va_arg(args, long double)
		: static_cast<long double>(va_arg(args, double));

	size_t len = FormatFloatingPoint(temp, value, precision);

	if (modifiers.hasWidth && len < modifiers.width)
	{
		size_t pad = modifiers.width - len;
		while (pad-- && remaining > 0)
		{
			*pOut++ = modifiers.paddingChar;
			--remaining;
		}
	}

	for (int i = 0; i < len && remaining > 0; ++i, --remaining)
	{
		*pOut++ = temp[i];
	}

	return 0;
}

static int HandleScientificInternal(
	char *&pOut,
	size_t &remaining,
	va_list &args,
	const FormatModifiers &modifiers,
	bool uppercase)
{
	char temp[64];

	int len = DoubleToScientific(
		temp,
		modifiers.isLongDouble ? static_cast<double>(va_arg(args, long double)) : va_arg(args, double),
		modifiers.hasPrecision ? modifiers.precision : 6,
		uppercase
	);

	if (modifiers.hasWidth && len < modifiers.width)
	{
		size_t pad = modifiers.width - len;
		while (pad-- && remaining > 0)
		{
			*pOut++ = modifiers.paddingChar;
			--remaining;
		}
	}

	for (int i = 0; i < len && remaining > 0; ++i, --remaining)
	{
		*pOut++ = temp[i];
	}

	return 0;
}

static int HandleScientific(char *&pOut, size_t &remaining, va_list &args, const FormatModifiers &modifiers)
{
	return HandleScientificInternal(pOut, remaining, args, modifiers, false);
}

static int HandleScientificUpper(char *&pOut, size_t &remaining, va_list &args, const FormatModifiers &modifiers)
{
	return HandleScientificInternal(pOut, remaining, args, modifiers, true);
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

	size_t len = FormatMessageA(
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
		size_t numLen = strlen(_ultoa(errorCode, numBuf, 10));

		len = 0;

		for (int i = 0; prefix[i] != '\0'; ++i)
			temp[len++] = prefix[i];

		for (int i = 0; i < numLen; ++i)
			temp[len++] = numBuf[i];

		for (int i = 0; suffix[i] != '\0'; ++i)
			temp[len++] = suffix[i];

		temp[len] = '\0';
	}

	if (len > remaining)
		len = remaining;

	memcpy(pOut, temp, len);
	pOut += len;
	remaining -= len;

	return 0;
}

static int HandleGeneralInternal(
	char *&pOut,
	size_t &remaining,
	va_list &args,
	const FormatModifiers &modifiers,
	bool uppercase)
{
	char temp[64];

	int len = DoubleToGeneral(
		temp,
		modifiers.isLongDouble ? static_cast<double>(va_arg(args, long double)) : va_arg(args, double),
		modifiers.hasPrecision ? modifiers.precision : 6,
		uppercase
	);

	if (modifiers.hasWidth && len < modifiers.width)
	{
		size_t pad = modifiers.width - len;
		while (pad-- && remaining > 0)
		{
			*pOut++ = modifiers.paddingChar;
			--remaining;
		}
	}

	for (int i = 0; i < len && remaining > 0; ++i, --remaining)
	{
		*pOut++ = temp[i];
	}

	return 0;
}

static int HandleGeneral(char *&pOut, size_t &remaining, va_list &args, const FormatModifiers &modifiers)
{
	return HandleGeneralInternal(pOut, remaining, args, modifiers, false);
}

static int HandleGeneralUpper(char *&pOut, size_t &remaining, va_list &args, const FormatModifiers &modifiers)
{
	return HandleGeneralInternal(pOut, remaining, args, modifiers, true);
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
	{ 'e', HandleScientific },
	{ 'E', HandleScientificUpper },
	{ 'g', HandleGeneral },
	{ 'G', HandleGeneralUpper },
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