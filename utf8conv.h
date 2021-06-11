#pragma once
#include <Windows.h>
#include <string>
std::wstring Utf8ToUtf16(const std::string& utf8)
{
	std::wstring utf16; // Result
	if (utf8.empty())
	{
		return utf16;
	}
}