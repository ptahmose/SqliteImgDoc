#pragma once

#include <algorithm>
#include "ITileCoordinate.h"
#include <map>

namespace SlImgDoc
{
    /// A structure combining a dimension and a value.
    struct DimensionAndValue
    {
        TileDim dimension;	///< The dimension.
        int value;			///< The value (for this dimension).
    };

    struct MinMaxCoordinate
    {
        int minValue;
        int maxValue;
    };

    struct TileCoordinateBounds
    {
        std::map<TileDim, MinMaxCoordinate> dimBounds;
    };

    class TileCoordinate : public ITileCoordinate
    {
    private:
        std::vector<DimensionAndValue> coordinates;
    public:
        TileCoordinate() : TileCoordinate(5) {}

        TileCoordinate(size_t reservedCapacity)
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

        void Set(TileDim d, int value)
        {
            auto it = std::find_if(this->coordinates.begin(), this->coordinates.end(), [=](const DimensionAndValue& s) { return s.dimension == d; });
            if (it != this->coordinates.end())
            {
                it->value = value;
            }
            else
            {
                this->coordinates.push_back(DimensionAndValue{ d,value });
            }
        }

        void Clear()
        {
            this->coordinates.clear();
        }

    public:
        bool TryGetCoordinate(TileDim dim, int* coordVal) const override
        {
            const auto it = std::find_if(this->coordinates.cbegin(), this->coordinates.cend(), [=](const DimensionAndValue& s) { return s.dimension == dim; });
            if (it != this->coordinates.cend())
            {
                if (coordVal != nullptr)
                {
                    *coordVal = it->value;
                }

                return true;
            }

            return false;
        }

        void EnumCoordinates(std::function<bool(TileDim, int)> f) const override
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