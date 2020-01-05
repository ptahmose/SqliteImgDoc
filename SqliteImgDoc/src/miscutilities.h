#pragma once

#include <cstdint>
#include <string>

class MiscUtils
{
public:
    static std::string empty_string;
    static bool TryParseUint64(const char* sz, std::uint64_t* val);
    static bool TryParseUint32(const char* sz, std::uint32_t* val);
    static bool TryParseUint64(const std::string& str, std::uint64_t* val) { return MiscUtils::TryParseUint64(str.c_str(), val); }
    static bool TryParseUint32(const std::string& str, std::uint32_t* val) { return MiscUtils::TryParseUint32(str.c_str(), val); }
};
