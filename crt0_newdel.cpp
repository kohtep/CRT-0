#include "crt0_common.hpp"

#include <Windows.h>
#include <vcruntime_new.h>
#include <cstddef>

void *__CRTDECL operator new(size_t _Size)
{
	return HeapAlloc(GetProcessHeap(), 0, _Size);
}

void *__CRTDECL operator new(size_t _Size, ::std::nothrow_t const &) noexcept
{
	return HeapAlloc(GetProcessHeap(), 0, _Size);
}

void *__CRTDECL operator new[](size_t _Size)
{
	return HeapAlloc(GetProcessHeap(), 0, _Size);
}

void *__CRTDECL operator new[](size_t _Size, ::std::nothrow_t const &) noexcept
{
	return HeapAlloc(GetProcessHeap(), 0, _Size);
}

void __CRTDECL operator delete(void *_Block) noexcept
{
	if (_Block)
		HeapFree(GetProcessHeap(), 0, _Block);
}

void __CRTDECL operator delete(void *_Block, ::std::nothrow_t const &) noexcept
{
	if (_Block)
		HeapFree(GetProcessHeap(), 0, _Block);
}

void __CRTDECL operator delete[](void *_Block) noexcept
{
	if (_Block)
		HeapFree(GetProcessHeap(), 0, _Block);
}

void __CRTDECL operator delete[](void *_Block, ::std::nothrow_t const &) noexcept
{
	if (_Block)
		HeapFree(GetProcessHeap(), 0, _Block);
}

void __CRTDECL operator delete(void *_Block, size_t) noexcept
{
	if (_Block)
		HeapFree(GetProcessHeap(), 0, _Block);
}

void __CRTDECL operator delete[](void *_Block, size_t) noexcept
{
	if (_Block)
		HeapFree(GetProcessHeap(), 0, _Block);
}

#if 0
#ifdef __cpp_aligned_new
void *__CRTDECL operator new(size_t size, std::align_val_t alignment)
{
	size_t align = static_cast<size_t>(alignment);
	size_t totalSize = size + align - 1 + sizeof(void *);
	void *raw = HeapAlloc(GetProcessHeap(), 0, totalSize);
	if (!raw)
		return nullptr;
	uintptr_t rawAddr = reinterpret_cast<uintptr_t>(raw) + sizeof(void *);
	uintptr_t alignedAddr = (rawAddr + (align - 1)) & ~(align - 1);
	reinterpret_cast<void **>(alignedAddr)[-1] = raw;
	return reinterpret_cast<void *>(alignedAddr);
}

void *__CRTDECL operator new(size_t size, std::align_val_t alignment, const std::nothrow_t &) noexcept
{
	return operator new(size, alignment);
}

void *__CRTDECL operator new[](size_t size, std::align_val_t alignment)
{
	return operator new(size, alignment);
}

void *__CRTDECL operator new[](size_t size, std::align_val_t alignment, const std::nothrow_t &) noexcept
{
	return operator new(size, alignment);
}

void __CRTDECL operator delete(void *ptr, std::align_val_t) noexcept
{
	if (ptr)
	{
		void *raw = reinterpret_cast<void **>(ptr)[-1];
		HeapFree(GetProcessHeap(), 0, raw);
	}
}

void __CRTDECL operator delete(void *ptr, std::align_val_t, const std::nothrow_t &) noexcept
{
	operator delete(ptr, std::align_val_t(alignof(std::max_align_t)));
}

void __CRTDECL operator delete[](void *ptr, std::align_val_t) noexcept
{
	operator delete(ptr, std::align_val_t(alignof(std::max_align_t)));
}

void __CRTDECL operator delete[](void *ptr, std::align_val_t, const std::nothrow_t &) noexcept
{
	operator delete(ptr, std::align_val_t(alignof(std::max_align_t)));
}

void __CRTDECL operator delete(void *ptr, size_t, std::align_val_t) noexcept
{
	operator delete(ptr, std::align_val_t(alignof(std::max_align_t)));
}

void __CRTDECL operator delete[](void *ptr, size_t, std::align_val_t) noexcept
{
	operator delete(ptr, std::align_val_t(alignof(std::max_align_t)));
}
#endif
#endif