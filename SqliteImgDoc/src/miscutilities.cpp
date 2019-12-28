#include "pch.h"
#include "miscutilities.h"
#include <cstdlib>
#include <cerrno>
#include <cstdint>
#include <limits>

#include <iostream>

using namespace std;

/*static*/bool MiscUtils::TryParseUint64(const char* sz, std::uint64_t* val)
{
    char* end;
    errno = 0;
    std::uint64_t v = std::strtoull(sz, &end, 10);
    if (*end != '\0')
    {
        return false;
    }

    cout << "v=" << v << " errno=" << errno << "numeric_limits<uint64_t>::max()=" << numeric_limits<uint64_t>::max()<<" ERANGE = "<<ERANGE << endl;
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
    char* end;
    errno = 0;
    std::uint32_t v = std::strtoul(sz, &end, 10);
    if (*end != '\0')
    {
        return false;
    }

    if (v == numeric_limits<uint32_t>::max()/*ULONG_MAX*/)
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
