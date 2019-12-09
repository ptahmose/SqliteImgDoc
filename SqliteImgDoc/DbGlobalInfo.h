#pragma once

#include <string>
#include <algorithm>

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

    enum class TilesDataColumn
    {
        Pk,
        PixelWidth,
        PixelHeight,
        PixelType,
        DataType,
        DataBinHdr,
        Data
    };

    enum class TilesSpatialIndexColumn
    {
        Pk,
        MinX,
        MaxX,
        MinY,
        MaxY
    };

    virtual const std::string& GetTableName(TableType tt) const = 0;

    virtual const std::vector<SlImgDoc::TileDim>& GetTileDimensions() const = 0;

    virtual const std::string& GetTileInfoColumnName(TilesInfoColumn c) const = 0;

    virtual bool GetTileInfoColumnNameForDimension(SlImgDoc::TileDim d, std::string & columnName) const = 0;

    virtual const std::string& GetTileDataColumnName(TilesDataColumn c) const = 0;

    virtual const std::string& GetTilesSpatialIndexColumnName(TilesSpatialIndexColumn c) const = 0;
};

class CDbDocInfo :public IDbDocInfo
{
private:
    std::vector< SlImgDoc::TileDim> dimensions;
    std::string tableNameTilesData;
    std::string tableNameTilesInfo;
    std::string tableNameSpatialIndex;
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

    static std::string ColumnName_TilesData_Pk;
    static std::string ColumnName_TilesData_PixelWidth;
    static std::string ColumnName_TilesData_PixelHeight;
    static std::string ColumnName_TilesData_PixelType;
    static std::string ColumnName_TilesData_DataType;
    static std::string ColumnName_TilesData_DataBinHdr;
    static std::string ColumnName_TilesData_Data;

    static std::string ColumnName_TilesSpatialIndex_Pk;
    static std::string ColumnName_TilesSpatialIndex_MinX;
    static std::string ColumnName_TilesSpatialIndex_MaxX;
    static std::string ColumnName_TilesSpatialIndex_MinY;
    static std::string ColumnName_TilesSpatialIndex_MaxY;
public:
    CDbDocInfo(const std::string& tableName_tilesdata, const std::string& tableName_tilesinfo, const std::string& tableName_SpatialIndex);
    CDbDocInfo();

    template<typename ForwardIterator>
    void SetTileDimensions(ForwardIterator begin, ForwardIterator end)
    {
        this->dimensions.clear();
        std::copy(begin, end, std::back_inserter(this->dimensions));
    }

    virtual const std::string& GetTableName(TableType tt) const;

    virtual const std::vector<SlImgDoc::TileDim>& GetTileDimensions() const;

    virtual const std::string& GetTileInfoColumnName(TilesInfoColumn c) const;

    virtual bool GetTileInfoColumnNameForDimension(SlImgDoc::TileDim d, std::string & tableName) const;

    virtual const std::string& GetTileDataColumnName(TilesDataColumn c) const;

    virtual const std::string& GetTilesSpatialIndexColumnName(TilesSpatialIndexColumn c) const;
};