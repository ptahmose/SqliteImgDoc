#pragma once

#include <vector>
#include <optional>
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

        virtual const std::unordered_set<TileDim>& GetTileDimsForClause() const = 0;
        virtual std::optional<std::reference_wrapper<const std::vector<RangeClause>>> GetRangeClause(TileDim d) const = 0;
        virtual std::optional<std::reference_wrapper<const std::vector<ListClause>>> GetListClause(TileDim d) const = 0;

        virtual ~IDimCoordinateQueryClause() = default;
    };

}
