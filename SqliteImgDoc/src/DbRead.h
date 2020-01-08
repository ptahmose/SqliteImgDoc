#pragma once

#include <utility>
#include "DbBase.h"
#include "DbGlobalInfo.h"
#include "../external/CoordinateData.h"
#include "DbReadCommon.h"

class CDbRead : CDbReadCommon, public SlImgDoc::IDbRead
{
public:
    CDbRead() = delete;
    CDbRead(std::shared_ptr<CDb> db) : CDbReadCommon(std::move(db)) {}

    virtual void ReadTileInfo(SlImgDoc::dbIndex idx, SlImgDoc::TileCoordinate* coord, SlImgDoc::LogicalPositionInfo* info);
    virtual void ReadTileData(SlImgDoc::dbIndex ix, SlImgDoc::TilePixelInfo* pixelInfo, SlImgDoc::IBlob* data);
    virtual void GetTilesIntersectingRect(const SlImgDoc::RectangleD& rect, std::function<bool(SlImgDoc::dbIndex)> func);
    virtual void GetTilesIntersectingWithLine(const SlImgDoc::LineThruTwoPointsD& rect, std::function<bool(SlImgDoc::dbIndex)> func);
public:
    virtual void Query(const SlImgDoc::IDimCoordinateQueryClause* clause, std::function<bool(SlImgDoc::dbIndex)> func);
    virtual void ReadPerTileData(SlImgDoc::dbIndex idx, const std::vector<std::string>& columns, std::function<bool(const SlImgDoc::KeyVariadicValuePair&)> func);
    virtual void EnumPerTileColumns(const std::function<bool(const SlImgDoc::ColumnDescription&)>& func);
};
