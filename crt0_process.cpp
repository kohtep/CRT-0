#include "crt0_common.hpp"

#include <Windows.h>
#include <process.h>

CRT_API void CRT_CALL abort()
{
	TerminateProcess(GetCurrentProcess(), 3);
	for (;;) {}
}

CRT_API void CRT_CALL exit(int code)
{
	ExitProcess(static_cast<UINT>(code));
}

CRT_API int CRT_CALL system(const char *command)
{
	if (!command)
		return 1;
	
	STARTUPINFOA si = { sizeof(si) };
	PROCESS_INFORMATION pi = {};

	char cmdline[1024];
	strncpy(cmdline, command, sizeof(cmdline));

	BOOL ok = CreateProcessA(
		nullptr,
		cmdline,
		nullptr,
		nullptr,
		FALSE,
		CREATE_NO_WINDOW,
		nullptr,
		nullptr,
		&si,
		&pi
	);

	if (!ok)
		return -1;

	WaitForSingleObject(pi.hProcess, INFINITE);

	DWORD exit_code = 0;
	GetExitCodeProcess(pi.hProcess, &exit_code);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return static_cast<int>(exit_code);
}

CRT_API char *CRT_CALL getenv(const char *name)
{
	static char buffer[512];
	DWORD len = GetEnvironmentVariableA(name, buffer, sizeof(buffer));
	return (len > 0 && len < sizeof(buffer)) ? buffer : nullptr;
}