#pragma once
#include <SQLiteCpp/Database.h>
#include "../external/Interface.h"
#include "DbGlobalInfo.h"

class CDbCreation
{
private:
    const IDbDocInfo& docInfo;
    const SlImgDoc::CreateOptions& opts;
public:
    CDbCreation(const IDbDocInfo& docInfo, const SlImgDoc::CreateOptions& opts);

    SQLite::Database* DoCreate();
private:
    std::string GetTilesInfoCreateSqlStatement() const;
    std::string GetTilesDataCreateSqlStatement() const;
    std::string GetTilesSpatialIndexCreateSqlStatement() const;

    void CheckCreateOptions(const SlImgDoc::CreateOptions& opts);
};