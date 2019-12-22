#include "pch.h"
#include "miscutilities.h"
#include <cstdlib>
#include <cerrno>
#include <cstdint>
#include <climits>

/*static*/bool MiscUtils::TryParseUint64(const char* sz, std::uint64_t* val)
{
    char* end;
    errno = 0;
    std::uint64_t v = std::strtoull(sz, &end, 10);
    if (*end != '\0')
    {
        return false;
    }

    if (v == ULLONG_MAX)
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
    char* end;
    errno = 0;
    std::uint32_t v = std::strtoul(sz, &end, 10);
    if (*end != '\0')
    {
        return false;
    }

    if (v == ULONG_MAX)
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
