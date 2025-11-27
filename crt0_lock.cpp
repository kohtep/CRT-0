#include "crt0_common.hpp"

#include <Windows.h>
#include <yvals.h>

namespace std
{
    _RTL_CRITICAL_SECTION mtx[8];
}

_RTL_CRITICAL_SECTION _acrt_lock_table[15];

void CRT_CALL _Mtxlock(_RTL_CRITICAL_SECTION *_Mtx) noexcept
{
    EnterCriticalSection(_Mtx);
}

void CRT_CALL Mtxunlock(LPCRITICAL_SECTION lpCriticalSection)
{
    LeaveCriticalSection(lpCriticalSection);
}

// The '_acrt_eagerly_load_locale_apis' call is missing
void CRT_CALL lock_locales()
{
    EnterCriticalSection(&_acrt_lock_table[4]);
}

void CRT_CALL unlock_locales()
{
    LeaveCriticalSection(&_acrt_lock_table[4]);
}

__CLR_OR_THIS_CALL std::_Lockit::_Lockit(int kind) noexcept
{
    _Locktype = kind;

    if (kind == 0)
    {
        lock_locales();
        return;
    }

    if (kind > 0 && kind < 8)
       _Mtxlock(&std::mtx[kind]);
}

__CLR_OR_THIS_CALL std::_Lockit::~_Lockit() noexcept
{
    int kind = _Locktype;

    if (kind == 0)
    {
        unlock_locales();
        return;
    }

    if (kind > 0 && kind < 8)
        Mtxunlock(&std::mtx[kind]);
}