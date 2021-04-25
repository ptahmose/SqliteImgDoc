#pragma once

#include <SQLiteCpp/SQLiteCpp.h>
#include "DbGlobalInfo.h"
#include "../external/IDimCoordinateQueryClause.h"
#include "../external/ITileInfoQueryClause.h"

class QueryBuildUtils
{
public:
    static SQLite::Statement Build(SQLite::Database& db,const IDbDocInfo& docInfo, const SlImgDoc::IDimCoordinateQueryClause* clause, const SlImgDoc::ITileInfoQueryClause* tileInfoQuery);
};
