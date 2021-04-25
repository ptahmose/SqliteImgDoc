#include "pch.h"
#include <sstream>
#include "QueryBuildUtils.h"

#include "miscutilities.h"

using namespace std;
using namespace SlImgDoc;

/*static*/SQLite::Statement QueryBuildUtils::Build(SQLite::Database& db, IDbDocInfo& docInfo, const SlImgDoc::IDimCoordinateQueryClause* clause, const SlImgDoc::ITileInfoQueryClause* tileInfoQuery)
{
    std::stringstream ss;
    ss << "SELECT " << docInfo.GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::Pk) << " FROM " << docInfo.GetTableName(IDbDocInfo::TableType::TilesInfo) << " WHERE ";

    int paramNo = 1;    // this is used to enumerate the parameters

    if (clause != nullptr)
    {
        // get the set of dimensions for which we have a clause
        auto rangeDims = clause->GetTileDimsForClause();

        bool firstClause = true;
        for (const auto dim : rangeDims)
        {
            string dimColumnName;
            docInfo.GetTileInfoColumnNameForDimension(dim, dimColumnName);

            auto ranges = clause->GetRangeClause(dim);
            auto list = clause->GetListClause(dim);

            if (ranges != nullptr && list != nullptr)
            {
                throw invalid_argument("...TODO...");
            }

            if (!firstClause)
            {
                ss << " AND ";
            }

            ss << "(";
            if (ranges != nullptr)
            {
                bool first = true;
                for (const auto& r : *ranges)
                {
                    if (!first)
                    {
                        ss << " OR ";
                    }

                    if (r.start != numeric_limits<int>::min() && r.end != numeric_limits<int>::max())
                    {
                        if (r.start == r.end)
                        {
                            ss << "(" << dimColumnName << "= ?" << paramNo++ << ")";
                        }
                        else
                        {
                            ss << "(" << dimColumnName << ">= ?" << paramNo << " AND " << dimColumnName << "<= ?" << paramNo + 1 << ")";
                            paramNo += 2; // Note: do not use increment-operator in above statement, because the order of execution is indeterminate then
                        }
                    }
                    else if (r.start != numeric_limits<int>::min())
                    {
                        ss << "(" << dimColumnName << "<= ?" << paramNo++ << ")";
                    }
                    else if (r.end != numeric_limits<int>::max())
                    {
                        ss << "(" << dimColumnName << ">= ?" << paramNo++ << ")";
                    }

                    first = false;
                }
            }

            ss << ")";

            firstClause = false;
        }

        if (tileInfoQuery != nullptr)
        {
            ConditionalOperator op;
            if (tileInfoQuery->GetPyramidLevelCondition(&op, nullptr))
            {
                ss << " AND ";
                ss << "(" << docInfo.GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::PyrLvl) << MiscUtils::ConditionalOperatorToString(op) << " ?" << paramNo++ << ")";
            }
        }

        SQLite::Statement query(db, ss.str());

        int bindingNo = 1;
        for (const auto dim : rangeDims)
        {
            string dimColumnName;
            docInfo.GetTileInfoColumnNameForDimension(dim, dimColumnName);

            auto ranges = clause->GetRangeClause(dim);
            auto list = clause->GetListClause(dim);

            if (ranges != nullptr)
            {
                for (const auto& r : *ranges)
                {
                    if (r.start != numeric_limits<int>::min() && r.end != numeric_limits<int>::max())
                    {
                        if (r.start == r.end)
                        {
                            query.bind(bindingNo++, r.start);
                        }
                        else
                        {
                            query.bind(bindingNo++, r.start);
                            query.bind(bindingNo++, r.end);
                        }
                    }
                    else if (r.start != numeric_limits<int>::min())
                    {
                        query.bind(bindingNo++, r.start);
                    }
                    else if (r.end != numeric_limits<int>::max())
                    {
                        query.bind(bindingNo++, r.end);
                    }
                }
            }
        }

        if (tileInfoQuery != nullptr)
        {
            int value;
            if (tileInfoQuery->GetPyramidLevelCondition(nullptr, &value))
            {
                query.bind(bindingNo++, value);
            }
        }

        return query;
    }
}