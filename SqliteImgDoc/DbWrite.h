#pragma once

#include "DbBase.h"
#include "SubBlkCoordinate.h"
#include "LogicalPositionInfo.h"
#include "IDataObj.h"
#include <SQLiteCpp/Transaction.h>

class CDbWrite : CDbBase, public SlImgDoc::IDbWrite
{
private:
    SQLite::Transaction* transaction;
public:
    CDbWrite(SQLite::Database* db) : CDbBase(db)
    {
    }

    virtual void BeginTransaction();
    virtual void CommitTransaction();

    virtual void AddSubBlock(const ISubBlkCoordinate* coord, const LogicalPositionInfo* info, const IDataObjUncompressedBitmap* data);

    virtual ~CDbWrite()
    {}

private:
    std::int64_t AddSubBlk(const IDataObjUncompressedBitmap* data);
    void AddToSpatialIndexTable(std::int64_t id, const LogicalPositionInfo* info);
};