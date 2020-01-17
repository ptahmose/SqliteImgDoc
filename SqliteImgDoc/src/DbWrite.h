#pragma once

#include "DbBase.h"
#include "../external/ITileCoordinate.h"
#include "../external/LogicalPositionInfo.h"
#include "../external/IDataObj.h"
#include "DbGlobalInfo.h"
#include <SQLiteCpp/Transaction.h>
#include "DbWriteBase.h"

class CDbWrite : public virtual SlImgDoc::IDbWrite, public CDbWriteBase
{
private:
    //bool transactionPending;

    std::unique_ptr<SQLite::Statement> addTilesDataRowStatement;
    std::unique_ptr<SQLite::Statement> addTilesInfoRowStatement;
    std::unique_ptr<SQLite::Statement> addTilesSpatialIndexRowStatement;
public:
    CDbWrite(std::shared_ptr<CDb> db) : CDbWriteBase(db)/*, transactionPending(false)*/ {}

    //virtual void BeginTransaction();
    //virtual void CommitTransaction();
    //virtual void RollbackTransaction();

    virtual SlImgDoc::dbIndex AddTile(const SlImgDoc::ITileCoordinate* coord, const SlImgDoc::LogicalPositionInfo* info, const SlImgDoc::TileBaseInfo* tileInfo, std::uint8_t datatype, const IDataObjBase* data);
    //virtual SlImgDoc::dbIndex AddTile(const SlImgDoc::ITileCoordinate* coord, const SlImgDoc::LogicalPositionInfo* info, const IDataObjUncompressedBitmap* data);
    //virtual SlImgDoc::dbIndex AddTile(const SlImgDoc::ITileCoordinate* coord, const SlImgDoc::LogicalPositionInfo* info, const SlImgDoc::TileBaseInfo* tileInfo, const IDataObjCustom* data);

    virtual void AddPerTileData(SlImgDoc::dbIndex index, std::function<bool(int, SlImgDoc::KeyVariadicValuePair&)> funcGetData);

    virtual ~CDbWrite();

private:
    //std::int64_t AddTileUncompressed(const IDataObjUncompressedBitmap* data);
    std::int64_t AddTileData(std::uint32_t width, std::uint32_t height, std::uint8_t pixeltype, std::uint8_t datatype, size_t sizeBinHdr, const void* binHdr, const IDataObjBase* data);

    void AddToSpatialIndexTable(std::int64_t id, const SlImgDoc::LogicalPositionInfo* info);

    void EnsureAddTilesDataRowStatement();
    void EnsureAddTilesInfoRowStatement();
    void EnsureAddTilesSpatialIndexRowStatement();
};
