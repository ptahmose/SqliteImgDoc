#pragma once

namespace SlImgDoc
{
    template <typename t>
    inline bool approximatelyEqual(t a, t b, t epsilon)
    {
        return fabs(a - b) <= ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
    }

    template <typename t>
    inline bool essentiallyEqual(t a, t b, t epsilon)
    {
        return fabs(a - b) <= ((fabs(a) > fabs(b) ? fabs(b) : fabs(a))* epsilon);
    }

    template <typename t>
    inline bool definitelyGreaterThan(t a, t b, t epsilon)
    {
        return (a - b) > ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
    }

    template <typename t>
    inline bool definitelyLessThan(t a, t b, t epsilon)
    {
        return (b - a) > ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
    }

}
