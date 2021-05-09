#pragma once

#include "DbBase.h"
#include "../external/ITileCoordinate.h"
#include "../external/LogicalPositionInfo.h"
#include "../external/IDataObj.h"
#include "DbGlobalInfo.h"
#include "DbWriteBase.h"

class CDbWrite : public virtual SlImgDoc::IDbWrite, public CDbWriteBase
{
private:
    std::unique_ptr<SQLite::Statement> addTilesDataRowStatement;
    std::unique_ptr<SQLite::Statement> addTilesInfoRowStatement;
    std::unique_ptr<SQLite::Statement> addTilesSpatialIndexRowStatement;
public:
    CDbWrite(std::shared_ptr<CDb> db) : CDbWriteBase(db)/*, transactionPending(false)*/ {}

    virtual SlImgDoc::dbIndex AddTile(const SlImgDoc::ITileCoordinate* coord, const SlImgDoc::LogicalPositionInfo* info, const SlImgDoc::TileBaseInfo* tileInfo, SlImgDoc::DataTypes datatype, const IDataObjBase* data);

    virtual void AddPerTileData(SlImgDoc::dbIndex index, std::function<bool(int, SlImgDoc::KeyVariadicValuePair&)> funcGetData);

    // IDbIndexManagement
    virtual void DropIndexOnCoordinate(SlImgDoc::TileDim dim);
    virtual void CreateIndexOnCoordinate(SlImgDoc::TileDim dim);

    virtual ~CDbWrite();

private:
    std::int64_t AddTileData(std::uint32_t width, std::uint32_t height, std::uint8_t pixeltype, SlImgDoc::DataTypes datatype, size_t sizeBinHdr, const void* binHdr, const IDataObjBase* data);

    void AddToSpatialIndexTable(std::int64_t id, const SlImgDoc::LogicalPositionInfo* info);

    void EnsureAddTilesDataRowStatement();
    void EnsureAddTilesInfoRowStatement();
    void EnsureAddTilesSpatialIndexRowStatement();
};
