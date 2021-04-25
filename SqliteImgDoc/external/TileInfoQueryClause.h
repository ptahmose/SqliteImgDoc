#pragma once

#include "ITileInfoQueryClause.h"

namespace SlImgDoc
{
    class TileInfoQueryClause : public ITileInfoQueryClause
    {
    private:
        ConditionalOperator conditionalOperator;
        int pyramidLevel;
    public:
        TileInfoQueryClause() : TileInfoQueryClause(ConditionalOperator::Invalid, 0)
        {}

        TileInfoQueryClause(ConditionalOperator op, int pyramidLevel)
            : conditionalOperator(op),
            pyramidLevel(pyramidLevel)
        {}

        virtual bool GetPyramidLevelCondition(ConditionalOperator* op, int* value) const
        {
            if (this->conditionalOperator == ConditionalOperator::Invalid)
            {
                return false;
            }

            if (op != nullptr)
            {
                *op = this->conditionalOperator;
            }

            if (value != nullptr)
            {
                *value = this->pyramidLevel;
            }

            return true;
        }
    };
}
