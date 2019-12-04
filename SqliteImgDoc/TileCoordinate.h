#pragma once

#include <algorithm>
#include "ITileCoordinate.h"

namespace SlImgDoc
{
    /// A structure combining a dimension and a value.
    struct DimensionAndValue
    {
        TileDim dimension;	///< The dimension.
        int value;							///< The value (for this dimension).
    };


    class TileCoordinate : public ITileCoordinate
    {
    private:
        std::vector<DimensionAndValue> coordinates;
    public:
        TileCoordinate(int reservedCapacity)
        {
            this->coordinates.reserve(reservedCapacity);
        }

        TileCoordinate(std::initializer_list<DimensionAndValue> list) : TileCoordinate(list.size())
        {
            for (auto d : list)
            {
                this->Set(d.dimension, d.value);
            }
        }

        inline void Set(TileDim d, int value)
        {
            auto it = std::find_if(this->coordinates.begin(), this->coordinates.end(), [=](const auto& s) { return s.dimension == d; });
            if (it != this->coordinates.end())
            {
                it->value = value;
            }
            else
            {
                this->coordinates.push_back(DimensionAndValue{ d,value });
            }
        }

    public:
        inline virtual bool TryGetCoordinate(TileDim dim, int* coordVal) const
        {
            const auto it = std::find_if(this->coordinates.cbegin(), this->coordinates.cend(), [=](const auto& s) { return s.dimension == dim; });
            if (it != this->coordinates.cend())
            {
                if (coordVal != nullptr)
                {
                    it->value;
                }

                return true;
            }

            return false;
        }

        inline virtual void EnumCoordinates(std::function<bool(TileDim, int)> f) const
        {
            for (auto it = this->coordinates.cbegin(); it != this->coordinates.cend(); ++it)
            {
                bool b = f(it->dimension, it->value);
                if (b == false)
                {
                    break;
                }
            }
        }
    };
}