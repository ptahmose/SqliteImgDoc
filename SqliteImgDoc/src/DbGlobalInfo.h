#pragma once

#include <string>
#include <algorithm>
#include <map>
#include "../external/ITileCoordinate.h"
#include "../external/CoordinateData.h"

enum class ColumnType
{
    Integer,
    Float
};

struct ColumnTypeInfo
{
    ColumnType type;
    int size;
};

struct ColumnTypeAllInfo : public ColumnTypeInfo
{
    std::string columnName;
};

class DbDocInfoUtils
{
public:
    static std::vector<ColumnTypeAllInfo> Convert(const SlImgDoc::PerTileDataCreateOptions& opts);
private:
    //static bool TryParse(const std::string& str, ColumnTypeInfo* colTypeInfo);
};


class IDbDocInfo
{
public:
    enum class TableType
    {
        TilesData,
        TilesInfo,
        TilesSpatialIndex,
        PerBrickData
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

    enum class PerTileDataColumn
    {
        Pk
    };

    enum class DbParameter
    {
        ///< An enum constant representing the size of the "DataBinHdr"-field in bytes.
        DataBinHdrSize
    };

    virtual const std::string& GetTableName(TableType tt) const = 0;

    virtual const std::vector<SlImgDoc::TileDim>& GetTileDimensions() const = 0;

    virtual const std::string& GetTileInfoColumnName(TilesInfoColumn c) const = 0;

    virtual bool GetTileInfoColumnNameForDimension(SlImgDoc::TileDim d, std::string& columnName) const = 0;

    /// Gets the name of the index used for the specified dimension. Note that only the canonical
    /// name of the index is returned, it is not checked whether it exists. This method will only
    /// fail if the dimension is unknown.
    /// \param          d         The dimension.
    /// \param [out]    indexName If successful, the name of the index is put here.
    /// \returns True if it succeeds, false if it fails.
    virtual bool GetTileInfoIndexNameForDimension(SlImgDoc::TileDim d, std::string& indexName) const = 0;

    virtual const std::string& GetTileDataColumnName(TilesDataColumn c) const = 0;

    virtual const std::string& GetTilesSpatialIndexColumnName(TilesSpatialIndexColumn c) const = 0;

    virtual const std::string& GetPerTilesDataColumnName(PerTileDataColumn c) const = 0;

    virtual std::uint32_t GetDbParameter(DbParameter parameter) const = 0;

    virtual bool GetCoordinateDataColumnNameForDimension(SlImgDoc::TileDim d, std::string& columnName) const = 0;

    virtual const std::vector<ColumnTypeAllInfo>& GetCoordinateDataColumnInfo() const = 0;
};

class CDbDocInfo :public IDbDocInfo
{
private:
    std::vector< SlImgDoc::TileDim> dimensions;
    std::string tableNameTilesData;
    std::string tableNameTilesInfo;
    std::string tableNameSpatialIndex;
    std::string tableNameCoordinateData;
    std::map<IDbDocInfo::DbParameter, std::uint32_t> dbParameters;
    std::vector<ColumnTypeAllInfo> coordinateDataColumns;
private:
    //static std::string TableName_DocumentInfo;
    static std::string TableName_TilesData;
    static std::string TableName_TilesInfo;
    static std::string TableName_TilesSpatialIndex;
    static std::string TableName_PerTileData;

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

    static std::string ColumnName_PerTilesData_Pk;
public:
    CDbDocInfo(std::string tableName_tilesdata, std::string tableName_tilesinfo, std::string tableName_SpatialIndex, std::string tableName_CoordinateData);
    CDbDocInfo();

    template<typename ForwardIterator>
    void SetTileDimensions(ForwardIterator begin, ForwardIterator end)
    {
        this->dimensions.clear();
        std::copy(begin, end, std::back_inserter(this->dimensions));
    }

    template<typename ForwardIterator> 
    void SetCoordinateColumns(ForwardIterator begin, ForwardIterator end)
    {
        this->coordinateDataColumns.clear();
        std::copy(begin, end, std::back_inserter(this->coordinateDataColumns));
    }

    void SetDbParameters(std::map<IDbDocInfo::DbParameter, std::uint32_t>&& dbParams)
    {
        this->dbParameters = std::move(dbParams);
    }

    virtual const std::string& GetTableName(TableType tt) const;

    virtual const std::vector<SlImgDoc::TileDim>& GetTileDimensions() const;

    virtual const std::string& GetTileInfoColumnName(TilesInfoColumn c) const;

    virtual bool GetTileInfoColumnNameForDimension(SlImgDoc::TileDim d, std::string& tableName) const;

    /// Gets the name of the index associated with the specified dimension.
    /// \param       d            The dimension.
    /// \param [out] indexName    The name of the index, associated with the specified dimension.
    /// \returns True if it succeeds, false if it fails.
    virtual bool GetTileInfoIndexNameForDimension(SlImgDoc::TileDim d, std::string& indexName) const;

    virtual const std::string& GetTileDataColumnName(TilesDataColumn c) const;

    virtual const std::string& GetTilesSpatialIndexColumnName(TilesSpatialIndexColumn c) const;

    virtual const std::string& GetPerTilesDataColumnName(PerTileDataColumn c) const;

    virtual std::uint32_t GetDbParameter(DbParameter parameter) const;

    virtual bool GetCoordinateDataColumnNameForDimension(SlImgDoc::TileDim d, std::string& columnName) const;

    virtual const std::vector<ColumnTypeAllInfo>& GetCoordinateDataColumnInfo() const;
public:
    static const std::string& GetDefaultTileInfoColumnName(TilesInfoColumn c);
    static const std::string& GetDefaultTileDataColumnName(TilesDataColumn c);
    static const std::string& GetDefaultSpatialIndexColumnName(TilesSpatialIndexColumn c);
    static const std::string& GetDefaultTableName(TableType tt);
};