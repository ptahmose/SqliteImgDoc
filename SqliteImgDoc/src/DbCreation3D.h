#pragma once
#include "../external/Interface.h"
#include "DbGlobalInfo.h"

class IDbDocInfo3D;

class CDbCreation3D
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
};
