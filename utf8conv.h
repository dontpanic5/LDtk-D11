#pragma once
#include <Windows.h>
#include <string>
#include <stdexcept>
#include "utf8except.h"
std::wstring Utf8ToUtf16(const std::string& utf8)
{
	std::wstring utf16; // Result
	if (utf8.empty())
	{
		return utf16;
	}

	// Safely fails if an invalid UTF-8 character
	// is encountered in the input string
	constexpr DWORD kFlags = MB_ERR_INVALID_CHARS;

	if (utf8.length() > static_cast<size_t>((std::numeric_limits<int>::max)()))
	{
		throw std::overflow_error(
			"Input string too long: size_t-length doesn't fit into int.");
	}
	const int utf8Length = static_cast<int>(utf8.length());
	const int utf16Length = ::MultiByteToWideChar(
		CP_UTF8,
		kFlags,
		utf8.data(),
		utf8Length,
		nullptr,
		0
	);

	if (utf16Length == 0)
	{
		// Conversion error: capture error code and throw
		const DWORD error = ::GetLastError();
		throw Utf8ConversionException(
			"Cannot get result sring length when converting " \
			"from UTF-8 to UTF-16 (MultiByteToWideChar failed).",
			error);
	}
	
	utf16.resize(utf16Length);

	int result = ::MultiByteToWideChar(
		CP_UTF8,
		kFlags,
		utf8.data(),
		utf8Length,
		&utf16[0],
		utf16Length
	);

	if (result == 0)
	{
		const DWORD error = ::GetLastError();
		throw Utf8ConversionException(
			"Cannot convert from UTF-8 to UTF-16 "\
			"(MultiByteToWideChar failed).",
			error);
	}

	return utf16;
}

std::string Utf16ToUtf8(const std::wstring& utf16)
{
	std::string utf8; // Result
	if (utf16.empty())
	{
		return utf8;
	}

	if (utf16.length() > static_cast<size_t>((std::numeric_limits<int>::max)()))
	{
		throw std::overflow_error(
			"Input string too long: size_t-length doesn't fit into int.");
	}
	const int utf16Length = static_cast<int>(utf16.length());
	const int utf8Length = ::WideCharToMultiByte(
		CP_UTF8,
		0,
		utf16.data(),
		utf16Length,
		nullptr,
		0,
		NULL,
		NULL
	);

	if (utf8Length == 0)
	{
		const DWORD error = ::GetLastError();
		throw Utf8ConversionException(
			"Cannot get result sring length when converting " \
			"from UTF-16 to UTF-8 (WideCharToMultiByte failed).",
			error
		);
	}

	utf8.resize(utf8Length);

	int result = ::WideCharToMultiByte(
		CP_UTF8,
		0,
		utf16.data(),
		utf16Length,
		&utf8[0],
		utf8Length,
		NULL,
		NULL
	);

	if (result == 0)
	{
		const DWORD error = ::GetLastError();
		throw Utf8ConversionException(
			"Cannot convert from UTF-16 to UTF-8 "\
			"(WideCharToMultiByte failed).",
			error);
	}
	
	return utf8;
}