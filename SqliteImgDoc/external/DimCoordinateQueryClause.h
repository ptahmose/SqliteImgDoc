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
        std::unordered_set<TileDim> dims;
    public:
        void AddRangeClause(TileDim d, const RangeClause& clause)
        {
            this->rangeClauses[d].push_back(clause);
            this->dims.insert(d);
        }

        void AddListClause(TileDim d, const ListClause& clause)
        {
            this->listClauses[d].push_back(clause);
            this->dims.insert(d);
        }

        const std::unordered_set<TileDim>& GetTileDimsForClause() const override
        {
            return this->dims;
        }

        const std::vector<RangeClause>* GetRangeClause(TileDim d) const override
        {
            const auto& c = this->rangeClauses.find(d);
            if (c != this->rangeClauses.cend())
            {
                return &c->second;
            }

            return nullptr;
        }

        const std::vector<ListClause>* GetListClause(TileDim d) const override
        {
            const auto& c = this->listClauses.find(d);
            if (c != this->listClauses.cend())
            {
                return &c->second;
            }

            return nullptr;
        }
    };
}
