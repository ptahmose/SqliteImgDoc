#pragma once

#include "DbBase.h"
#include "../external/ITileCoordinate.h"
#include "../external/LogicalPositionInfo.h"
#include "../external/IDataObj.h"
#include "DbGlobalInfo.h"
#include "DbWriteBase.h"

class CDbWrite3D : public virtual SlImgDoc::IDbWrite3D, public CDbWriteBase
{
private:
    std::unique_ptr<SQLite::Statement> addTilesDataRowStatement;
    std::unique_ptr<SQLite::Statement> addTilesInfoRowStatement;
    std::unique_ptr<SQLite::Statement> addTilesSpatialIndexRowStatement;
public:
    CDbWrite3D(std::shared_ptr<CDb> db) : CDbWriteBase(db) {}

    virtual SlImgDoc::dbIndex AddBrick(const SlImgDoc::ITileCoordinate* coord, const SlImgDoc::LogicalPositionInfo3D* info, const SlImgDoc::TileBaseInfo3D* tileInfo, SlImgDoc::DataTypes datatype, const IDataObjBase* data);

    virtual void AddPerTileData(SlImgDoc::dbIndex index, std::function<bool(int, SlImgDoc::KeyVariadicValuePair&)> funcGetData);

    virtual ~CDbWrite3D();
private:
    std::int64_t AddBrickData(std::uint32_t width, std::uint32_t height, std::uint32_t depth, std::uint8_t pixeltype, SlImgDoc::DataTypes datatype, size_t sizeBinHdr, const void* binHdr, const IDataObjBase* data);

    void AddToSpatialIndexTable(std::int64_t id, const SlImgDoc::LogicalPositionInfo3D* info);

    void EnsureAddTilesDataRowStatement();
    void EnsureAddTilesInfoRowStatement();
    void EnsureAddTilesSpatialIndexRowStatement();
};
