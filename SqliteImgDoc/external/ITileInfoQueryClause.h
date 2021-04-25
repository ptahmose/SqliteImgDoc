#pragma once

#include <functional>
#include <vector>

namespace SlImgDoc
{
    enum class ConditionalOperator
    {
        Invalid = 0,
        Equal,
        NotEqual,
        LessThan,
        LessThanOrEqual,
        GreaterThan,
        GreaterThanOrEqual
    };

    class ITileInfoQueryClause
    {
    public:
        virtual bool GetPyramidLevelCondition(ConditionalOperator* op, int* value) const = 0;
    };
}
