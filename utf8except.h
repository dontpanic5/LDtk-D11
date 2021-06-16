#pragma once
#include <stdint.h>
#include <stdexcept>
// Represents an error during UTF-8 encoding conversions
class Utf8ConversionException
	: public std::runtime_error
{
	// Error code from GetLastError()
	uint32_t _errorCode;

public:
	Utf8ConversionException(
		const char* message,
		uint32_t errorCode
	)
		: std::runtime_error(message)
		, _errorCode(errorCode)
	{ }

	uint32_t ErrorCode() const
	{
		return _errorCode;
	}
};