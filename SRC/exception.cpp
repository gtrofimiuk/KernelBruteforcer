#include <cstdarg>

#include "exception.h"

char ErrorMessageBuffer[1024];

std::runtime_error Exception(const char *Format, ...)
{
	va_list args;
	va_start(args, Format);
	vsnprintf(ErrorMessageBuffer, 1024 - 1, Format, args);
	va_end(args);
	return std::runtime_error(ErrorMessageBuffer);
}
