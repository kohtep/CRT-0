#include "crt0_common.hpp"

#include <Windows.h>

CRT_NO_INTRINSIC(memcpy);
CRT_NO_INTRINSIC(memmove);
CRT_NO_INTRINSIC(memset);
CRT_NO_INTRINSIC(memcmp);
CRT_NO_INTRINSIC(memchr);

CRT_API void *CRT_CALL malloc(size_t size)
{
	return HeapAlloc(GetProcessHeap(), 0, size);
}

CRT_API void CRT_CALL free(void *ptr)
{
	if (ptr)
		HeapFree(GetProcessHeap(), 0, ptr);
}

CRT_API void *CRT_CALL calloc(size_t num, size_t size)
{
	SIZE_T total = num * size;
	return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, total);
}

CRT_API void *CRT_CALL realloc(void *ptr, size_t new_size)
{
	if (!ptr)
		return HeapAlloc(GetProcessHeap(), 0, new_size);
	if (new_size == 0)
	{
		HeapFree(GetProcessHeap(), 0, ptr);
		return nullptr;
	}
	return HeapReAlloc(GetProcessHeap(), 0, ptr, new_size);
}

CRT_API void *CRT_CALL memcpy(void *lpDestination, const void *lpSource, size_t dwSize)
{
	volatile unsigned char *dest = static_cast<unsigned char *>(lpDestination);
	const unsigned char *src = static_cast<const unsigned char *>(lpSource);

	for (size_t i = 0; i < dwSize; ++i)
	{
		dest[i] = src[i];
	}

	return lpDestination;
}

CRT_API void *CRT_CALL memmove(void *dest, const void *src, size_t n)
{
	unsigned char *d = static_cast<unsigned char *>(dest);
	const unsigned char *s = static_cast<const unsigned char *>(src);

	if (d < s)
	{
		while (n--) *d++ = *s++;
	}
	else
	{
		d += n;
		s += n;
		while (n--) *(--d) = *(--s);
	}

	return dest;
}

CRT_API void *CRT_CALL memset(void *lpDestination, int nValue, size_t dwSize)
{
	// `volatile` helps prevent insertion of a `memset` call from the CRT library.
	volatile unsigned char *dest = static_cast<unsigned char *>(lpDestination);

	for (size_t i = 0; i < dwSize; ++i)
	{
		dest[i] = static_cast<unsigned char>(nValue);
	}

	return lpDestination;
}

CRT_API int CRT_CALL memcmp(const void *lpBlock1, const void *lpBlock2, size_t dwSize)
{
	const unsigned char *p1 = static_cast<const unsigned char *>(lpBlock1);
	const unsigned char *p2 = static_cast<const unsigned char *>(lpBlock2);

	for (size_t i = 0; i < dwSize; ++i)
	{
		if (p1[i] != p2[i])
			return static_cast<int>(p1[i]) - static_cast<int>(p2[i]);
	}

	return 0;
}

CRT_API const void *CRT_CALL memchr(const void *ptr, int value, size_t num)
{
	const unsigned char *p = static_cast<const unsigned char *>(ptr);
	unsigned char ch = static_cast<unsigned char>(value);

	for (size_t i = 0; i < num; ++i)
	{
		if (p[i] == ch)
			return p + i;
	}

	return nullptr;
}