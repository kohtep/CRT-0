#include "crt0_common.hpp"

EXCEPTION_DISPOSITION CRT_CALL __C_specific_handler(
    struct _EXCEPTION_RECORD *ExceptionRecord,
    void *EstablisherFrame,
    struct _CONTEXT *ContextRecord,
    struct _DISPATCHER_CONTEXT *DispatcherContext
)
{
	return ExceptionContinueSearch;
}