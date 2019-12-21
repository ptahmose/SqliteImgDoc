#pragma once

#include <cstdint>

class MiscUtils
{
public:
    static bool TryParseUint64(const char* sz, std::uint64_t* val);
    static bool TryParseUint32(const char* sz, std::uint32_t* val);
};
