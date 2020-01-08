#pragma once
#include "../external/Interface.h"
#include "DbGlobalInfo.h"
#include "DbCreationCommon.h"

class IDbDocInfo3D;

class CDbCreation3D : public CDbCreationCommon
{
private:
    const IDbDocInfo3D& docInfo;
    const SlImgDoc::CreateOptions& opts;
public:
    CDbCreation3D(const IDbDocInfo3D& docInfo, const SlImgDoc::CreateOptions& opts);

    SQLite::Database* DoCreate();

private:
    std::string GetTilesInfoCreateSqlStatement() const;
    std::string GetTilesDataCreateSqlStatement() const;
    std::string GetTilesSpatialIndexCreateSqlStatement() const;
    std::string GetPerTileDataTableSqlStatement() const;
};
