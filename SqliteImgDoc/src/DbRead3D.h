#pragma once

#include "DbBase.h"
#include "DbGlobalInfo.h"
#include "DbReadCommon.h"

class CDbRead3D : CDbReadCommon, public SlImgDoc::IDbRead3D
{
public:
    CDbRead3D(std::shared_ptr<CDb> db) : CDbReadCommon(std::move(db)) {}

    // interface IDbRead3D
    virtual void ReadTileInfo(SlImgDoc::dbIndex idx, SlImgDoc::TileCoordinate* coord, SlImgDoc::LogicalPositionInfo3D* info);
    virtual void ReadTileData(SlImgDoc::dbIndex ix, SlImgDoc::TilePixelInfo3D* pixelInfo, SlImgDoc::IBlob* data);
    virtual void GetTilesIntersectingCuboid(const SlImgDoc::CuboidD& rect, std::function<bool(SlImgDoc::dbIndex)> func);
    virtual void GetTilesIntersectingWithPlane(const SlImgDoc::Plane_NormalAndDistD& plane, std::function<bool(SlImgDoc::dbIndex)> func);

    // interface IDbReadCommon
    virtual SlImgDoc::TileCoordinateBounds QueryDimensionBounds();
    virtual void ReadPerTileData(SlImgDoc::dbIndex idx, const std::vector<std::string>& columns, std::function<bool(const SlImgDoc::KeyVariadicValuePair&)> func);
    virtual void Query(const SlImgDoc::IDimCoordinateQueryClause* clause, const SlImgDoc::ITileInfoQueryClause* tileInfoQuery, std::function<bool(SlImgDoc::dbIndex)> func);
    virtual void EnumPerTileColumns(const std::function<bool(const SlImgDoc::ColumnDescription&)>& func) const;
};
