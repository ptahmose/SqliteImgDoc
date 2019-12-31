#pragma once

#include "DbBase.h"
#include "DbGlobalInfo.h"

class CDbRead3D : CDbBase, public SlImgDoc::IDbRead3D
{
public:
    CDbRead3D(std::shared_ptr<CDb> db) : CDbBase(db) {}

    virtual void ReadTileInfo(SlImgDoc::dbIndex idx, SlImgDoc::TileCoordinate* coord, SlImgDoc::LogicalPositionInfo3D* info);
    virtual void ReadTileData(SlImgDoc::dbIndex ix, SlImgDoc::TilePixelInfo3D* pixelInfo, SlImgDoc::IBlob* data);

    virtual void Query(const SlImgDoc::IDimCoordinateQueryClause* clause, std::function<bool(SlImgDoc::dbIndex)> func);

    virtual void GetTilesIntersectingCuboid(const SlImgDoc::CuboidD& rect, std::function<bool(SlImgDoc::dbIndex)> func);
    virtual void GetTilesIntersectingWithPlane(const SlImgDoc::Plane_NormalAndDistD& plane, std::function<bool(SlImgDoc::dbIndex)> func);
};