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
private:
    struct BuildInfo
    {
        const std::string& TableName;

        const std::string& PkColumnName;

        const std::string& PyramidLevelColumnName;

        std::function<bool(SlImgDoc::TileDim dim, std::string& columnName)> GetColumnNameForDimension;
    };

    static SQLite::Statement QueryBuildUtils::Build(SQLite::Database& db, const BuildInfo& info, const SlImgDoc::IDimCoordinateQueryClause* clause, const SlImgDoc::ITileInfoQueryClause* tileInfoQuery);
};
