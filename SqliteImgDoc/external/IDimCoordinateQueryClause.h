#pragma once

#include <vector>
#include <set>
#include "ITileCoordinate.h"

namespace SlImgDoc
{
    /// This interface describes a query clause for the dimension-columns.
    /// It is important that this object is idempotent - meaning if the methods are 
    /// called multiple times, an implementation is required to give identical results.
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

        /// Gets the set of dimensions for which there is a clause present. It is important that the order
        /// of elements is idempotent (as of course the content itself).
        /// \returns The set of dimensions for which there are clauses.
        virtual const std::set<TileDim>& GetTileDimsForClause() const = 0;

        virtual const std::vector<RangeClause>* GetRangeClause(TileDim d) const = 0;
        virtual const std::vector<ListClause>* GetListClause(TileDim d) const = 0;

        virtual ~IDimCoordinateQueryClause() = default;
    };

}
