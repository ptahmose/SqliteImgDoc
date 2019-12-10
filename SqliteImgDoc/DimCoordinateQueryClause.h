#pragma once

#include "IDimCoordinateQueryClause.h"
#include <map>

namespace SlImgDoc
{
    class CDimCoordinateQueryClause : public IDimCoordinateQueryClause
    {
    private:
        std::map<TileDim, std::vector<RangeClause>> rangeClauses;
        std::map<TileDim, std::vector<ListClause>> listClauses;
    public:
        void AddRangeClause(TileDim d,const RangeClause& clause)
        {
            this->rangeClauses[d].push_back(clause);
        }

        void AddListClause(TileDim d, const ListClause& clause)
        {
            this->listClauses[d].push_back(clause);
        }

        virtual std::vector<TileDim> GetTileDimsForRangeClause() const override
        {
            std::vector<TileDim> dims;
            dims.reserve(this->rangeClauses.size());
            for (const auto& pair : this->rangeClauses) 
            {
                dims.push_back(pair.first);
            }

            return dims;
        }
        
        virtual std::vector<TileDim> GetTileDimsForListClause() const override
        {
            std::vector<TileDim> dims;
            dims.reserve(this->listClauses.size());
            for (const auto& pair : this->listClauses)
            {
                dims.push_back(pair.first);
            }

            return dims;
        }

        virtual const std::vector<RangeClause>& GetRangeClause(TileDim d) const override
        {
            return this->rangeClauses.at(d);
        }

        virtual const std::vector<ListClause>& GetListClause(TileDim d) const override
        {
            return this->listClauses.at(d);
        }
    };
}
