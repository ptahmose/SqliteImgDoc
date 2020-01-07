#pragma once

#include "DbBase.h"
#include "DbGlobalInfo.h"
#include "../external/CoordinateData.h"

class CDbRead : CDbBase, public SlImgDoc::IDbRead
{
private:
   // std::shared_ptr<IDbDocInfo> docInfo;
public:
   /* CDbRead(SQLite::Database* db, std::shared_ptr<IDbDocInfo> dbInfo) : CDbBase(db), docInfo(dbInfo)
    {
    }*/
    CDbRead(std::shared_ptr<CDb> db) : CDbBase(db){}

    virtual void ReadTileInfo(SlImgDoc::dbIndex idx, SlImgDoc::TileCoordinate* coord, SlImgDoc::LogicalPositionInfo* info);
    virtual void ReadTileData(SlImgDoc::dbIndex ix, SlImgDoc::TilePixelInfo* pixelInfo, SlImgDoc::IBlob* data);

    virtual void Query(const SlImgDoc::IDimCoordinateQueryClause* clause, std::function<bool(SlImgDoc::dbIndex)> func);

    virtual void ReadPerTileData(SlImgDoc::dbIndex idx, const std::vector<std::string>& columns, std::function<bool(const SlImgDoc::KeyVariadicValuePair&)> func);    

    virtual void GetTilesIntersectingRect(const SlImgDoc::RectangleD& rect, std::function<bool(SlImgDoc::dbIndex)> func);
    virtual void GetTilesIntersectingWithLine(const SlImgDoc::LineThruTwoPointsD& rect, std::function<bool(SlImgDoc::dbIndex)> func);
private:

};