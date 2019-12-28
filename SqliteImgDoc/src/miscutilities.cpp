#include "pch.h"
#include "miscutilities.h"
#include <cstdlib>
#include <cerrno>
//#include <cstdint>
#include <limits>

#include <iostream>

using namespace std;

/*static*/bool MiscUtils::TryParseUint64(const char* sz, std::uint64_t* val)
{
    char* end;
    errno = 0;
    std::uint64_t v = std::strtoull(sz, &end, 10);
    if (sz[0] == '\0' || *end != '\0')
    {
        return false;
    }

    if (v == numeric_limits<uint64_t>::max()/*ULLONG_MAX*/)
    {
        if (errno == ERANGE)
        {
            return false;
        }
    }

    if (val != nullptr)
    {
        *val = v;
    }

    return true;
}

/*static*/bool MiscUtils::TryParseUint32(const char* sz, std::uint32_t* val)
{
    // note: no idea what wrong, but on Linux std::strtoul behaves strange because if does not detect overflows, e.g. 
    // for "4294967296" I get 0 and errno is NOT set to ERANGE
    uint64_t val64;
    if (!MiscUtils::TryParseUint64(sz, &val64))
    {
        return false;
    }

    if (val64 <= numeric_limits<uint32_t>::max())
    {
        if (val != nullptr)
        {
            *val = static_cast<uint32_t>(val64);
        }

        return true;
    }

    return false;
}
