#include "pch.h"
#include <sstream>
#include "QueryBuildUtils.h"

#include "miscutilities.h"

using namespace std;
using namespace SlImgDoc;

/*static*/SQLite::Statement QueryBuildUtils::Build(SQLite::Database& db, const IDbDocInfo& docInfo, const SlImgDoc::IDimCoordinateQueryClause* clause, const SlImgDoc::ITileInfoQueryClause* tileInfoQuery)
{
    BuildInfo buildInfo
    {
        docInfo.GetTableName(IDbDocInfo::TableType::TilesInfo),
        docInfo.GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::Pk),
        docInfo.GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::PyrLvl),
        [&](SlImgDoc::TileDim dim, std::string& columnName)->bool
        {
            return docInfo.GetTileInfoColumnNameForDimension(dim,columnName);
        }
    };

    return QueryBuildUtils::Build(db, buildInfo, clause, tileInfoQuery);
}

/*static*/void QueryBuildUtils::AddSqlStatement(SQLite::Database& db, std::stringstream& ss, int startParamNo, const char* firstClauseStatement, const IDbDocInfo& docInfo, const SlImgDoc::IDimCoordinateQueryClause* clause, const SlImgDoc::ITileInfoQueryClause* tileInfoQuery)
{
    BuildInfo buildInfo
    {
        docInfo.GetTableName(IDbDocInfo::TableType::TilesInfo),
        docInfo.GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::Pk),
        docInfo.GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::PyrLvl),
        [&](SlImgDoc::TileDim dim, std::string& columnName)->bool
        {
            return docInfo.GetTileInfoColumnNameForDimension(dim,columnName);
        }
    };

    QueryBuildUtils::CreateQueryClause(ss, startParamNo, firstClauseStatement, buildInfo, clause, tileInfoQuery);
}

/*static*/void QueryBuildUtils::BindData(SQLite::Database& db, SQLite::Statement& sqlStatement, int startParamNo, const IDbDocInfo& docInfo, const SlImgDoc::IDimCoordinateQueryClause* clause, const SlImgDoc::ITileInfoQueryClause* tileInfoQuery)
{
    BuildInfo buildInfo
    {
        docInfo.GetTableName(IDbDocInfo::TableType::TilesInfo),
        docInfo.GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::Pk),
        docInfo.GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::PyrLvl),
        [&](SlImgDoc::TileDim dim, std::string& columnName)->bool
        {
            return docInfo.GetTileInfoColumnNameForDimension(dim,columnName);
        }
    };

    QueryBuildUtils::BindDataForSqlStatement(sqlStatement, startParamNo, buildInfo, clause, tileInfoQuery);
}

/*static*/SQLite::Statement QueryBuildUtils::Build(SQLite::Database& db, const IDbDocInfo3D& docInfo3D, const SlImgDoc::IDimCoordinateQueryClause* clause, const SlImgDoc::ITileInfoQueryClause* tileInfoQuery)
{
    BuildInfo buildInfo
    {
        docInfo3D.GetTableName(IDbDocInfo3D::TableType::TilesInfo),
            docInfo3D.GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::Pk),
            docInfo3D.GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::PyrLvl),
            [&](SlImgDoc::TileDim dim, std::string& columnName)->bool
        {
            return docInfo3D.GetTileInfoColumnNameForDimension(dim, columnName);
        }
    };

    return QueryBuildUtils::Build(db, buildInfo, clause, tileInfoQuery);
}

/*static*/SQLite::Statement QueryBuildUtils::Build(SQLite::Database& db, const BuildInfo& info, const SlImgDoc::IDimCoordinateQueryClause* clause, const SlImgDoc::ITileInfoQueryClause* tileInfoQuery)
{
    std::stringstream ss;
    ss << "SELECT " << info.PkColumnName << " FROM " << info.TableName;

    QueryBuildUtils::CreateQueryClause(ss, 1, "WHERE", info, clause, tileInfoQuery);
    //int paramNo = 1;    // this is used to enumerate the parameters
    //bool firstClause = true;

    //if (clause != nullptr)
    //{
    //    // get the set of dimensions for which we have a clause
    //    auto rangeDims = clause->GetTileDimsForClause();

    //    for (const auto dim : rangeDims)
    //    {
    //        string dimColumnName;
    //        info.GetColumnNameForDimension(dim, dimColumnName);

    //        auto ranges = clause->GetRangeClause(dim);
    //        auto list = clause->GetListClause(dim);

    //        if (ranges != nullptr && list != nullptr)
    //        {
    //            throw invalid_argument("...TODO...");
    //        }

    //        ss << (firstClause ? " WHERE " : " AND ");
    //        ss << "(";

    //        bool first = true;
    //        if (ranges != nullptr)
    //        {
    //            for (const auto& r : *ranges)
    //            {
    //                if (!first)
    //                {
    //                    ss << " OR ";
    //                }

    //                if (r.start != numeric_limits<int>::min() && r.end != numeric_limits<int>::max())
    //                {
    //                    if (r.start == r.end)
    //                    {
    //                        ss << "(" << dimColumnName << "= ?" << paramNo++ << ")";
    //                    }
    //                    else
    //                    {
    //                        ss << "(" << dimColumnName << ">= ?" << paramNo << " AND " << dimColumnName << "<= ?" << paramNo + 1 << ")";
    //                        paramNo += 2; // Note: do not use increment-operator in above statement, because the order of execution is indeterminate then
    //                    }
    //                }
    //                else if (r.start != numeric_limits<int>::min())
    //                {
    //                    ss << "(" << dimColumnName << ">= ?" << paramNo++ << ")";
    //                }
    //                else if (r.end != numeric_limits<int>::max())
    //                {
    //                    ss << "(" << dimColumnName << "<= ?" << paramNo++ << ")";
    //                }

    //                first = false;
    //            }
    //        }

    //        if (list != nullptr)
    //        {
    //            for (const auto& l : *list)
    //            {
    //                if (!first)
    //                {
    //                    ss << " OR ";
    //                }

    //                ss << "(" << dimColumnName << " IN (";

    //                bool firstInList = true;
    //                for (const auto& i : l.list)
    //                {
    //                    if (!firstInList)
    //                    {
    //                        ss << ",";
    //                    }

    //                    ss << "?" << paramNo++;
    //                    firstInList = false;
    //                }

    //                ss << "))";
    //                first = false;
    //            }
    //        }

    //        ss << ")";

    //        firstClause = false;
    //    }
    //}

    //if (tileInfoQuery != nullptr)
    //{
    //    ConditionalOperator op;
    //    if (tileInfoQuery->GetPyramidLevelCondition(&op, nullptr))
    //    {
    //        ss << (firstClause ? " WHERE " : " AND ");
    //        ss << "(" << info.PyramidLevelColumnName << MiscUtils::ConditionalOperatorToString(op) << " ?" << paramNo++ << ")";
    //        firstClause = false;
    //    }
    //}

    SQLite::Statement query(db, ss.str());

    /*int bindingNo = 1;
    if (clause != nullptr)
    {
        for (const auto dim : clause->GetTileDimsForClause())
        {
            string dimColumnName;
            info.GetColumnNameForDimension(dim, dimColumnName);

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

            if (list != nullptr)
            {
                for (const auto& l : *list)
                {
                    for (const auto& i : l.list)
                    {
                        query.bind(bindingNo++, i);
                    }
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
    }*/
    QueryBuildUtils::BindDataForSqlStatement(query, 1, info, clause, tileInfoQuery);

    return query;
}

/*static*/void QueryBuildUtils::BindDataForSqlStatement(SQLite::Statement& sqlStatement, int startParamNo, const BuildInfo& info, const SlImgDoc::IDimCoordinateQueryClause* clause, const SlImgDoc::ITileInfoQueryClause* tileInfoQuery)
{
    int bindingNo = startParamNo;
    if (clause != nullptr)
    {
        for (const auto dim : clause->GetTileDimsForClause())
        {
            string dimColumnName;
            info.GetColumnNameForDimension(dim, dimColumnName);

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
                            sqlStatement.bind(bindingNo++, r.start);
                        }
                        else
                        {
                            sqlStatement.bind(bindingNo++, r.start);
                            sqlStatement.bind(bindingNo++, r.end);
                        }
                    }
                    else if (r.start != numeric_limits<int>::min())
                    {
                        sqlStatement.bind(bindingNo++, r.start);
                    }
                    else if (r.end != numeric_limits<int>::max())
                    {
                        sqlStatement.bind(bindingNo++, r.end);
                    }
                }
            }

            if (list != nullptr)
            {
                for (const auto& l : *list)
                {
                    for (const auto& i : l.list)
                    {
                        sqlStatement.bind(bindingNo++, i);
                    }
                }
            }
        }
    }

    if (tileInfoQuery != nullptr)
    {
        int value;
        if (tileInfoQuery->GetPyramidLevelCondition(nullptr, &value))
        {
            sqlStatement.bind(bindingNo++, value);
        }
    }
}

/*static*/void QueryBuildUtils::CreateQueryClause(std::stringstream& ss, int startParamNo, const char* firstClauseStatement, const BuildInfo& info, const SlImgDoc::IDimCoordinateQueryClause* clause, const SlImgDoc::ITileInfoQueryClause* tileInfoQuery)
{
    int paramNo = startParamNo;    // this is used to enumerate the parameters
    bool firstClause = true;

    if (clause != nullptr)
    {
        // get the set of dimensions for which we have a clause
        auto rangeDims = clause->GetTileDimsForClause();

        for (const auto dim : rangeDims)
        {
            string dimColumnName;
            info.GetColumnNameForDimension(dim, dimColumnName);

            auto ranges = clause->GetRangeClause(dim);
            auto list = clause->GetListClause(dim);

            if (ranges != nullptr && list != nullptr)
            {
                throw invalid_argument("...TODO...");
            }

            if (firstClause)
            {
                ss << " " << firstClauseStatement << " ";
            }
            else
            {
                ss << " AND ";
            }
            //ss << (firstClause ? " WHERE " : " AND ");
            ss << "(";

            bool first = true;
            if (ranges != nullptr)
            {
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
                        ss << "(" << dimColumnName << ">= ?" << paramNo++ << ")";
                    }
                    else if (r.end != numeric_limits<int>::max())
                    {
                        ss << "(" << dimColumnName << "<= ?" << paramNo++ << ")";
                    }

                    first = false;
                }
            }

            if (list != nullptr)
            {
                for (const auto& l : *list)
                {
                    if (!first)
                    {
                        ss << " OR ";
                    }

                    ss << "(" << dimColumnName << " IN (";

                    bool firstInList = true;
                    for (const auto& i : l.list)
                    {
                        if (!firstInList)
                        {
                            ss << ",";
                        }

                        ss << "?" << paramNo++;
                        firstInList = false;
                    }

                    ss << "))";
                    first = false;
                }
            }

            ss << ")";

            firstClause = false;
        }
    }

    if (tileInfoQuery != nullptr)
    {
        ConditionalOperator op;
        if (tileInfoQuery->GetPyramidLevelCondition(&op, nullptr))
        {
            if (firstClause)
            {
                ss << " " << firstClauseStatement << " ";
            }
            else
            {
                ss << " AND ";
            }
            //ss << (firstClause ? " WHERE " : " AND ");
            ss << "(" << info.PyramidLevelColumnName << MiscUtils::ConditionalOperatorToString(op) << " ?" << paramNo++ << ")";
            firstClause = false;
        }
    }
}

/*static*/SQLite::Statement QueryBuildUtils::QueryDimBounds(SQLite::Database& db, const IDbDocInfo& docInfo)
{
    const auto tileDims = docInfo.GetTileDimensions();
    auto sqlString = GenerateQueryMinMaxSqlQuery(
        docInfo.GetTableName(IDbDocInfo::TableType::TilesInfo),
        tileDims.size(),
        [&](size_t idx)->string
        {
            string colName;
            bool b = docInfo.GetTileInfoColumnNameForDimension(tileDims[idx], colName);
            return colName;
        });

    return SQLite::Statement(db, sqlString);
}

/*static*/std::string QueryBuildUtils::GenerateQueryMinMaxSqlQuery(std::string tableName, size_t noOfDimensions, std::function<std::string(size_t)> getColumnName)
{
    std::stringstream ss;
    ss << "SELECT ";

    for (size_t i = 0; i < noOfDimensions; ++i)
    {
        const auto& colName = getColumnName(i);
        if (i > 0)
        {
            ss << ",";
        }

        ss << "MIN(" << colName << "),MAX(" << colName << ")";
    }

    ss << " FROM " << tableName;
    return ss.str();
}

