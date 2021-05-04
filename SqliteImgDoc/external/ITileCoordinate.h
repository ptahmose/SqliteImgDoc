#pragma once

#include <functional>
#include <vector>

namespace SlImgDoc
{
    typedef char TileDim;

    class ITileCoordinate
    {
    public:
        virtual bool TryGetCoordinate(TileDim dim, int* coordVal) const = 0;
        virtual void EnumCoordinates(std::function<bool(TileDim, int)> f) const = 0;
        virtual ~ITileCoordinate() = default;

        inline void EnumDimensions(std::function<bool(TileDim)> f) const
        {
            this->EnumCoordinates([&](TileDim d, int v)->bool {return f(d); });
        }

        inline std::vector<TileDim> GetDimensions() const
        {
            std::vector<TileDim> vec;
            this->EnumDimensions([&](TileDim d)->bool {vec.push_back(d); return true; });
            return vec;
        }

        inline static bool IsValidDimension(TileDim d)
        {
            return ((d >= 'a' && d <= 'z') || (d >= 'A' && d <= 'Z'));
        }
    };
}
