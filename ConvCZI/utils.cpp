#include "utils.h"
#include <locale>
#include <codecvt>

/*static*/std::string CUtils::convertToUtf8(const std::wstring& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
    std::string conv = utf8_conv.to_bytes(str);
    return conv;
}

/*static*/std::wstring CUtils::convertUtf8ToUCS2(const std::string& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8conv;
    std::wstring conv = utf8conv.from_bytes(str);
    return conv;
}
