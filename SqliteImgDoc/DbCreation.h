#pragma once
#include "Interface.h"
#include "DbGlobalInfo.h"

class CDbCreation
{
private:
    const IDbDocInfo& docInfo;
    const SlImgDoc::CreateOptions& opts;
public:
    CDbCreation(const IDbDocInfo& docInfo, const SlImgDoc::CreateOptions& opts);

    void DoCreate();

private:
    std::string GetTilesInfoCreateSqlStatement() const;
    std::string GetTilesDataCreateSqlStatement() const;
};