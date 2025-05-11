#include "crt0_common.hpp"

#include <Windows.h>
#include <stdio.h>

CRT_API FILE *CRT_CALL __acrt_iob_func(unsigned index)
{
	HANDLE h = nullptr;

	switch (index)
	{
	case 0: h = GetStdHandle(STD_INPUT_HANDLE); break;
	case 1: h = GetStdHandle(STD_OUTPUT_HANDLE); break;
	case 2: h = GetStdHandle(STD_ERROR_HANDLE); break;
	default: return nullptr;
	}

	return reinterpret_cast<FILE *>(h);
}

#define MAX_OPEN_FILES 256

struct FileContext
{
	HANDLE handle = 0;
	int last_char = -1;
	bool error = false;
	bool eof = false;
	bool in_use = false;
};

static FileContext g_file_contexts[MAX_OPEN_FILES];

static FILE *FileFromIndex(size_t index)
{
	return reinterpret_cast<FILE *>(index + 1);
}

static size_t IndexFromFile(FILE *f)
{
	return reinterpret_cast<size_t>(f) - 1;
}

static FileContext *GetFileContext(FILE *f)
{
	size_t index = IndexFromFile(f);
	if (index >= MAX_OPEN_FILES || !g_file_contexts[index].in_use)
		return nullptr;
	return &g_file_contexts[index];
}

static FileContext *AllocateFileContext()
{
	for (size_t i = 0; i < MAX_OPEN_FILES; ++i)
	{
		if (!g_file_contexts[i].in_use)
		{
			g_file_contexts[i].in_use = true;
			return &g_file_contexts[i];
		}
	}

	return nullptr;
}

static bool HasFreeFileSlot()
{
	for (size_t i = 0; i < MAX_OPEN_FILES; ++i)
	{
		if (!g_file_contexts[i].in_use)
			return true;
	}
	return false;
}

CRT_API void CRT_CALL clearerr(FILE *stream)
{
	if (auto ctx = GetFileContext(stream))
	{
		ctx->error = false;
		ctx->eof = false;
	}
}

CRT_API int CRT_CALL fclose(FILE *stream)
{
	size_t index = IndexFromFile(stream);
	if (index >= MAX_OPEN_FILES || !g_file_contexts[index].in_use)
		return EOF;

	FileContext &ctx = g_file_contexts[index];
	BOOL ok = CloseHandle(ctx.handle);
	ctx = FileContext();
	ctx.in_use = false;
	return ok ? 0 : EOF;
}

CRT_API int CRT_CALL feof(FILE *stream)
{
	auto ctx = GetFileContext(stream);
	return ctx && ctx->eof ? 1 : 0;
}

CRT_API int CRT_CALL ferror(FILE *stream)
{
	auto ctx = GetFileContext(stream);
	return ctx && ctx->error ? 1 : 0;
}

CRT_API int CRT_CALL fflush(FILE *stream)
{
	auto ctx = GetFileContext(stream);
	return ctx && FlushFileBuffers(ctx->handle) ? 0 : EOF;
}

CRT_API char *CRT_CALL fgets(char *buffer, int count, FILE *stream)
{
	if (!buffer || count <= 0)
		return nullptr;

	auto ctx = GetFileContext(stream);
	if (!ctx)
		return nullptr;

	DWORD read = 0;
	int i = 0;

	while (i < count - 1)
	{
		char ch;
		DWORD r = 0;
		if (!ReadFile(ctx->handle, &ch, 1, &r, nullptr))
		{
			ctx->error = true;
			break;
		}

		if (r == 0)
		{
			ctx->eof = true;
			break;
		}

		buffer[i++] = ch;
		if (ch == '\n')
			break;
	}

	if (i == 0)
		return nullptr;

	buffer[i] = '\0';
	return buffer;
}

CRT_API FILE *CRT_CALL fopen(const char *filename, const char *mode)
{
	FileContext *ctx = AllocateFileContext();
	if (!ctx)
	{
		SetLastError(ERROR_TOO_MANY_OPEN_FILES);
		return nullptr;
	}

	DWORD access = 0;
	DWORD creation = 0;

	while (*mode)
	{
		switch (*mode)
		{
		case 'r': access |= GENERIC_READ; creation = OPEN_EXISTING; break;
		case 'w': access |= GENERIC_WRITE; creation = CREATE_ALWAYS; break;
		case 'a': access |= FILE_APPEND_DATA; creation = OPEN_ALWAYS; break;
		case '+': access |= GENERIC_READ | GENERIC_WRITE; break;
		}
		++mode;
	}

	HANDLE h = CreateFileA(filename, access, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, creation, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (h == INVALID_HANDLE_VALUE)
	{
		ctx->in_use = false;
		return nullptr;
	}

	ctx->handle = h;
	size_t index = ctx - g_file_contexts;
	return FileFromIndex(index);
}

CRT_API errno_t CRT_CALL fopen_s(FILE **pFile, const char *filename, const char *mode)
{
	if (!pFile)
		return EINVAL;

	*pFile = nullptr;

	if (!filename || !mode)
		return EINVAL;

	FileContext *ctx = AllocateFileContext();
	if (!ctx)
		return EMFILE;

	DWORD access = 0;
	DWORD creation = 0;

	while (*mode)
	{
		switch (*mode)
		{
		case 'r': access |= GENERIC_READ; creation = OPEN_EXISTING; break;
		case 'w': access |= GENERIC_WRITE; creation = CREATE_ALWAYS; break;
		case 'a': access |= FILE_APPEND_DATA; creation = OPEN_ALWAYS; break;
		case '+': access |= GENERIC_READ | GENERIC_WRITE; break;
		}
		++mode;
	}

	HANDLE h = CreateFileA(filename, access, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, creation, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (h == INVALID_HANDLE_VALUE)
	{
		ctx->in_use = false;
		return static_cast<errno_t>(GetLastError());
	}

	ctx->handle = h;
	size_t index = ctx - g_file_contexts;
	*pFile = FileFromIndex(index);
	return 0;
}

CRT_API size_t CRT_CALL fread(void *buffer, size_t size, size_t count, FILE *stream)
{
	auto ctx = GetFileContext(stream);
	if (!ctx)
		return 0;

	DWORD total = 0;
	if (!ReadFile(ctx->handle, buffer, static_cast<DWORD>(size * count), &total, nullptr))
	{
		ctx->error = true;
		return 0;
	}

	if (total == 0)
		ctx->eof = true;

	return size ? total / size : 0;
}

CRT_API FILE *CRT_CALL freopen(const char *filename, const char *mode, FILE *stream)
{
	fclose(stream);

	FILE *result = nullptr;
	if (fopen_s(&result, filename, mode) != 0)
		return nullptr;

	return result;
}

CRT_API size_t CRT_CALL fwrite(const void *buffer, size_t size, size_t count, FILE *stream)
{
	auto ctx = GetFileContext(stream);
	if (!ctx)
		return 0;

	DWORD total = 0;
	if (!WriteFile(ctx->handle, buffer, static_cast<DWORD>(size * count), &total, nullptr))
	{
		ctx->error = true;
		return 0;
	}

	return size ? total / size : 0;
}

CRT_API int CRT_CALL getc(FILE *stream)
{
	auto ctx = GetFileContext(stream);
	if (!ctx)
		return EOF;

	if (ctx->last_char >= 0)
	{
		int ch = ctx->last_char;
		ctx->last_char = -1;
		return ch;
	}

	char ch;
	DWORD r = 0;
	if (!ReadFile(ctx->handle, &ch, 1, &r, nullptr))
	{
		ctx->error = true;
		return EOF;
	}

	if (r == 0)
	{
		ctx->eof = true;
		return EOF;
	}

	return static_cast<unsigned char>(ch);
}

CRT_API int CRT_CALL ungetc(int ch, FILE *stream)
{
	auto ctx = GetFileContext(stream);
	if (!ctx || ctx->last_char >= 0)
		return EOF;

	ctx->last_char = ch;
	ctx->eof = false;
	return ch;
}

CRT_API int CRT_CALL fseek(FILE *stream, long offset, int origin)
{
	auto ctx = GetFileContext(stream);
	if (!ctx)
		return -1;

	DWORD move_method = 0;
	switch (origin)
	{
	case SEEK_SET: move_method = FILE_BEGIN; break;
	case SEEK_CUR: move_method = FILE_CURRENT; break;
	case SEEK_END: move_method = FILE_END; break;
	default: return -1;
	}

	DWORD result = SetFilePointer(ctx->handle, offset, nullptr, move_method);
	if (result == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
	{
		ctx->error = true;
		return -1;
	}

	ctx->eof = false;
	ctx->last_char = -1;
	return 0;
}

CRT_API long CRT_CALL ftell(FILE *stream)
{
	auto ctx = GetFileContext(stream);
	if (!ctx)
		return -1;

	DWORD pos = SetFilePointer(ctx->handle, 0, nullptr, FILE_CURRENT);
	if (pos == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
	{
		ctx->error = true;
		return -1;
	}

	return static_cast<long>(pos);
}

CRT_API int CRT_CALL fputc(int ch, FILE *stream)
{
	auto ctx = GetFileContext(stream);
	if (!ctx)
		return EOF;

	char c = static_cast<char>(ch);
	DWORD written = 0;
	if (!WriteFile(ctx->handle, &c, 1, &written, nullptr) || written != 1)
	{
		ctx->error = true;
		return EOF;
	}

	return static_cast<unsigned char>(c);
}

CRT_API FILE *CRT_CALL tmpfile()
{
	char path[MAX_PATH];
	char name[MAX_PATH];

	if (GetTempPathA(MAX_PATH, path) == 0)
		return nullptr;

	if (GetTempFileNameA(path, "tmp", 0, name) == 0)
		return nullptr;

	FILE *result = nullptr;
	if (fopen_s(&result, name, "w+b") != 0)
		return nullptr;

	return result;
}

CRT_API char *CRT_CALL tmpnam(char *str)
{
	static char static_buf[L_tmpnam];

	char path[MAX_PATH];
	char name[MAX_PATH];

	if (GetTempPathA(MAX_PATH, path) == 0)
		return nullptr;

	if (GetTempFileNameA(path, "tmp", 0, name) == 0)
		return nullptr;

	const char *src = name;
	char *dst = str ? str : static_buf;
	size_t i = 0;

	while (src[i] && i < L_tmpnam - 1)
	{
		dst[i] = src[i];
		++i;
	}

	dst[i] = '\0';
	return dst;
}

CRT_API int CRT_CALL remove(const char *filename)
{
	return DeleteFileA(filename) ? 0 : -1;
}

CRT_API int CRT_CALL rename(const char *oldname, const char *newname)
{
	return MoveFileA(oldname, newname) ? 0 : -1;
}

CRT_API int CRT_CALL setvbuf(FILE *stream, char *buffer, int mode, size_t size)
{
	(void)stream;
	(void)buffer;
	(void)mode;
	(void)size;
	return 0;
}

CRT_API FILE *CRT_CALL _popen(const char *command, const char *mode)
{
	HANDLE read_pipe = nullptr;
	HANDLE write_pipe = nullptr;
	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };

	if (mode[0] == 'r')
	{
		if (!CreatePipe(&read_pipe, &write_pipe, &sa, 0))
			return nullptr;
	}
	else if (mode[0] == 'w')
	{
		if (!CreatePipe(&write_pipe, &read_pipe, &sa, 0))
			return nullptr;
	}
	else
	{
		return nullptr;
	}

	STARTUPINFOA si = { sizeof(STARTUPINFOA) };
	PROCESS_INFORMATION pi = {};
	si.dwFlags = STARTF_USESTDHANDLES;

	if (mode[0] == 'r')
	{
		si.hStdOutput = write_pipe;
		si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	}
	else
	{
		si.hStdInput = read_pipe;
		si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	}

	si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

	char cmdline[1024];
	strncpy(cmdline, command, sizeof(cmdline) - 1);
	cmdline[sizeof(cmdline) - 1] = '\0';

	BOOL ok = CreateProcessA(
		nullptr, cmdline, nullptr, nullptr, TRUE,
		CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi
	);

	if (mode[0] == 'r')
	{
		CloseHandle(write_pipe);
	}
	else
	{
		CloseHandle(read_pipe);
	}

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	if (!ok)
	{
		HANDLE used_pipe = (mode[0] == 'r') ? read_pipe : write_pipe;
		CloseHandle(used_pipe);
		return nullptr;
	}

	HANDLE used_pipe = (mode[0] == 'r') ? read_pipe : write_pipe;

	FileContext *ctx = AllocateFileContext();
	if (!ctx)
	{
		CloseHandle(used_pipe);
		return nullptr;
	}

	ctx->handle = used_pipe;
	size_t index = ctx - g_file_contexts;
	return FileFromIndex(index);
}

CRT_API int CRT_CALL _pclose(FILE *stream)
{
	return fclose(stream);
}

CRT_API int CRT_CALL _fseeki64(FILE *stream, __int64 offset, int origin)
{
	auto ctx = GetFileContext(stream);
	if (!ctx)
		return -1;

	DWORD move_method = 0;
	switch (origin)
	{
	case SEEK_SET: move_method = FILE_BEGIN; break;
	case SEEK_CUR: move_method = FILE_CURRENT; break;
	case SEEK_END: move_method = FILE_END; break;
	default: return -1;
	}

	LARGE_INTEGER li;
	li.QuadPart = offset;

	LARGE_INTEGER result;
	if (!SetFilePointerEx(ctx->handle, li, &result, move_method))
	{
		ctx->error = true;
		return -1;
	}

	ctx->eof = false;
	ctx->last_char = -1;
	return 0;
}

CRT_API __int64 CRT_CALL _ftelli64(FILE *stream)
{
	auto ctx = GetFileContext(stream);
	if (!ctx)
		return -1;

	LARGE_INTEGER li;
	li.QuadPart = 0;

	LARGE_INTEGER result;
	if (!SetFilePointerEx(ctx->handle, li, &result, FILE_CURRENT))
	{
		ctx->error = true;
		return -1;
	}

	return result.QuadPart;
}