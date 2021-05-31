#pragma once

#include <functional>
#include <SQLiteCpp/SQLiteCpp.h>
#include "DbGlobalInfo.h"
#include "DbGlobalInfo3D.h"
#include "../external/IDimCoordinateQueryClause.h"
#include "../external/ITileInfoQueryClause.h"

class QueryBuildUtils
{
public:
    static SQLite::Statement Build(SQLite::Database& db,const IDbDocInfo& docInfo, const SlImgDoc::IDimCoordinateQueryClause* clause, const SlImgDoc::ITileInfoQueryClause* tileInfoQuery);
    static SQLite::Statement Build(SQLite::Database& db, const IDbDocInfo3D& docInfo, const SlImgDoc::IDimCoordinateQueryClause* clause, const SlImgDoc::ITileInfoQueryClause* tileInfoQuery);

    static SQLite::Statement QueryDimBounds(SQLite::Database& db, const IDbDocInfo& docInfo);

    static void AddSqlStatement(SQLite::Database& db, std::stringstream& ss, int startParamNo, const char* firstClauseStatement, const IDbDocInfo& docInfo, const SlImgDoc::IDimCoordinateQueryClause* clause, const SlImgDoc::ITileInfoQueryClause* tileInfoQuery);
    static void BindData(SQLite::Database& db, SQLite::Statement& sqlStatement, int startParamNo,  const IDbDocInfo& docInfo, const SlImgDoc::IDimCoordinateQueryClause* clause, const SlImgDoc::ITileInfoQueryClause* tileInfoQuery);
private:
    struct BuildInfo
    {
        const std::string& TableName;

        const std::string& PkColumnName;

        const std::string& PyramidLevelColumnName;

        std::function<bool(SlImgDoc::TileDim dim, std::string& columnName)> GetColumnNameForDimension;
    };

    static SQLite::Statement Build(SQLite::Database& db, const BuildInfo& info, const SlImgDoc::IDimCoordinateQueryClause* clause, const SlImgDoc::ITileInfoQueryClause* tileInfoQuery);

    static void CreateQueryClause(std::stringstream& ss,int startParamNo, const char* firstClauseStatement, const BuildInfo& info, const SlImgDoc::IDimCoordinateQueryClause* clause, const SlImgDoc::ITileInfoQueryClause* tileInfoQuery);
    static void BindDataForSqlStatement(SQLite::Statement& sqlStatement, int startParamNo, const BuildInfo& info, const SlImgDoc::IDimCoordinateQueryClause* clause, const SlImgDoc::ITileInfoQueryClause* tileInfoQuery);

    static std::string GenerateQueryMinMaxSqlQuery(std::string tableName, size_t noOfDimensions, std::function<std::string(size_t)> getColumnName);
    
};
