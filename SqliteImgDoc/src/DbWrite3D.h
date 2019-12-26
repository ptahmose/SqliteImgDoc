#pragma once

#include "DbBase.h"
#include "../external/ITileCoordinate.h"
#include "../external/LogicalPositionInfo.h"
#include "../external/IDataObj.h"
#include "DbGlobalInfo.h"
#include <SQLiteCpp/Transaction.h>
#include "DbWriteBase.h"

class CDbWrite3D : CDbWriteBase, public virtual SlImgDoc::IDbWrite3D
{
private:
    //bool transactionPending;

    std::unique_ptr<SQLite::Statement> addTilesDataRowStatement;
    std::unique_ptr<SQLite::Statement> addTilesInfoRowStatement;
    std::unique_ptr<SQLite::Statement> addTilesSpatialIndexRowStatement;
public:
    CDbWrite3D(std::shared_ptr<CDb> db) : CDbWriteBase(db) {}

    virtual void AddBrick(const SlImgDoc::ITileCoordinate* coord, const SlImgDoc::LogicalPositionInfo3D* info, const IDataObjUncompressedBrick* data) = 0;
    virtual void AddBrick(const SlImgDoc::ITileCoordinate* coord, const SlImgDoc::LogicalPositionInfo3D* info, const SlImgDoc::TileBaseInfo3D* tileInfo, const IDataObjCustom* data) = 0;


    virtual ~CDbWrite3D();
private:
    std::int64_t AddBrickUncompressed(const IDataObjUncompressedBrick* data);
    std::int64_t AddBrickData(std::uint32_t width, std::uint32_t height, std::uint32_t depth,std::uint8_t pixeltype, std::uint8_t datatype, size_t sizeBinHdr, const void* binHdr, const IDataObjBase* data);

    void AddToSpatialIndexTable(std::int64_t id, const SlImgDoc::LogicalPositionInfo3D* info);

    void EnsureAddTilesDataRowStatement();
    void EnsureAddTilesInfoRowStatement();
    void EnsureAddTilesSpatialIndexRowStatement();
};
