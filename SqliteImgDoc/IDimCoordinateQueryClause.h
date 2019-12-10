#pragma once

#include <vector>
#include "ITileCoordinate.h"

namespace SlImgDoc
{

    class IDimCoordinateQueryClause
    {
    public:
        struct RangeClause
        {
            int start;
            int end;
        };

        struct ListClause
        {
            std::vector<int> list;
        };

        virtual std::vector<TileDim> GetTileDimsForRangeClause() const = 0;
        virtual std::vector<TileDim> GetTileDimsForListClause() const = 0;
        virtual const std::vector<RangeClause>& GetRangeClause(TileDim d) const = 0;
        virtual const std::vector<ListClause>& GetListClause(TileDim d) const = 0;

        virtual ~IDimCoordinateQueryClause() = default;
    };

}
