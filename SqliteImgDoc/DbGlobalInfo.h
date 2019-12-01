#pragma once

#include <string>

#include "ITileCoordinate.h"


class IDbDocInfo
{
public:
    enum class TableType
    {
        TilesData,
        TilesInfo,
        TilesSpatialIndex
    };

    enum class TilesInfoColumn
    {
        Pk,
        TileX,
        TileY,
        TileWidth,
        TileHeight,
        PyrLvl,
        TileDataId
    };

    virtual const std::string& GetTableName(TableType tt) = 0;

    virtual const std::vector<SlImgDoc::TileDim>& GetTileDimensions() const = 0;

    virtual const std::string& GetTileInfoColumnName(TilesInfoColumn c) const = 0;

    virtual bool GetTileInfoColumnNameForDimension(SlImgDoc::TileDim d, std::string & tableName) const = 0;
};

class CDbDocInfo :public IDbDocInfo
{
private:
    std::vector< SlImgDoc::TileDim> dimensions;
private:
    //static std::string TableName_DocumentInfo;
    static std::string TableName_TilesData;
    static std::string TableName_TilesInfo;
    static std::string TableName_TilesSpatialIndex;

    static std::string ColumnName_TilesInfo_Pk;
    static std::string ColumnName_TilesInfo_TileX;
    static std::string ColumnName_TilesInfo_TileY;
    static std::string ColumnName_TilesInfo_TileWidth;
    static std::string ColumnName_TilesInfo_TileHeight;
    static std::string ColumnName_TilesInfo_PyrLvl;
    static std::string ColumnName_TilesInfo_TileDataId;
public:
public:
    virtual const std::string& GetTableName(TableType tt);

    virtual const std::vector<SlImgDoc::TileDim>& GetTileDimensions() const;

    virtual const std::string& GetTileInfoColumnName(TilesInfoColumn c) const;

    virtual bool GetTileInfoColumnNameForDimension(SlImgDoc::TileDim d, std::string & tableName) const;
};