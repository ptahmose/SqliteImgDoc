#pragma once
#include <string>

class CUtils
{
public:
    static std::string convertToUtf8(const std::wstring& str);
    static std::wstring convertUtf8ToUCS2(const std::string& str);
};
