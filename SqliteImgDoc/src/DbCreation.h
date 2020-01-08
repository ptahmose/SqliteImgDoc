#pragma once
#include <SQLiteCpp/Database.h>
#include "../external/Interface.h"
#include "DbGlobalInfo.h"
#include "DbCreationCommon.h"

class CDbCreation : public CDbCreationCommon
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
    std::string GetPerTileDataTableSqlStatement() const;

    void CheckCreateOptions(const SlImgDoc::CreateOptions& opts);
};
