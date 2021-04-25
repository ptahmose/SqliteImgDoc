#pragma once

#include <vector>
//#include <optional>
#include "ITileCoordinate.h"

namespace SlImgDoc
{
    class IDimCoordinateQueryClause
    {
    public:
        /// A range clause means that the value must be greater than or equal to the
        /// 'start' field and less than or equal than the 'end'. Use int-min for start
        /// in order to have only a "less than or equal" comparison, and int-max for
        /// "greater or equal".
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
        virtual const std::vector<RangeClause>* GetRangeClause(TileDim d) const = 0;
        virtual const std::vector<ListClause>* GetListClause(TileDim d) const = 0;

        virtual ~IDimCoordinateQueryClause() = default;
    };

}
