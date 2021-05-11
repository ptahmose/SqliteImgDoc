#pragma once

#include <string>
#include <vector>

class Utils
{
public:
    static std::wstring convertUtf8ToWchart(const std::string& str);
    static std::string convertToUtf8(const std::wstring& str);
    static std::vector<std::wstring> wrap(const wchar_t* text, size_t line_length/* = 72*/);
};