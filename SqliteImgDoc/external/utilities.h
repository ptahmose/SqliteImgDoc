#pragma once

#include <cmath>

namespace SlImgDoc
{
    template <typename t>
    inline bool approximatelyEqual(t a, t b, t epsilon)
    {
        return std::fabs(a - b) <= ((std::fabs(a) < std::fabs(b) ? std::fabs(b) : std::fabs(a)) * epsilon);
    }

    template <typename t>
    inline bool essentiallyEqual(t a, t b, t epsilon)
    {
        return std::fabs(a - b) <= ((std::fabs(a) > std::fabs(b) ? std::fabs(b) : std::fabs(a))* epsilon);
    }

    template <typename t>
    inline bool definitelyGreaterThan(t a, t b, t epsilon)
    {
        return (a - b) > ((std::fabs(a) < std::fabs(b) ? std::fabs(b) : std::fabs(a)) * epsilon);
    }

    template <typename t>
    inline bool definitelyLessThan(t a, t b, t epsilon)
    {
        return (b - a) > ((std::fabs(a) < std::fabs(b) ? std::fabs(b) : std::fabs(a)) * epsilon);
    }

}
