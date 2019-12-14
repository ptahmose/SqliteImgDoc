#pragma once

#include "DbBase.h"
#include "ITileCoordinate.h"
#include "LogicalPositionInfo.h"
#include "IDataObj.h"
#include "DbGlobalInfo.h"
#include <SQLiteCpp/Transaction.h>

class CDbWrite : CDbBase, public SlImgDoc::IDbWrite
{
private:
    bool transactionPending;
  //  std::shared_ptr<IDbDocInfo> docInfo;

    std::unique_ptr<SQLite::Statement> addTilesDataRowStatement;
    std::unique_ptr<SQLite::Statement> addTilesInfoRowStatement;
    std::unique_ptr<SQLite::Statement> addTilesSpatialIndexRowStatement;
public:
  /*  CDbWrite(SQLite::Database* db, std::shared_ptr<IDbDocInfo> dbInfo) : CDbBase(db), docInfo(dbInfo), transactionPending(false)
    {
    }*/
    CDbWrite(std::shared_ptr<CDb> db) : CDbBase(db), transactionPending(false) {}

    virtual void BeginTransaction();
    virtual void CommitTransaction();
    virtual void RollbackTransaction();

    virtual void AddSubBlock(const SlImgDoc::ITileCoordinate* coord, const LogicalPositionInfo* info, const IDataObjUncompressedBitmap* data);
    virtual void AddSubBlock(const SlImgDoc::ITileCoordinate* coord, const LogicalPositionInfo* info, const SlImgDoc::TileBaseInfo* tileInfo, const IDataObjCustom* data);

    virtual ~CDbWrite();

private:
    std::int64_t AddSubBlk(const IDataObjUncompressedBitmap* data);
    std::int64_t AddTileData(std::uint32_t width, std::uint32_t height, std::uint8_t pixeltype, std::uint8_t datatype, size_t sizeBinHdr, const void* binHdr, const IDataObjBase* data);

    void AddToSpatialIndexTable(std::int64_t id, const LogicalPositionInfo* info);

    void EnsureAddTilesDataRowStatement();
    void EnsureAddTilesInfoRowStatement();
    void EnsureAddTilesSpatialIndexRowStatement();
};