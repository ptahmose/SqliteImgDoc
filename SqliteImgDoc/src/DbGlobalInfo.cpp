#include "pch.h"
#include <stdexcept> 
#include <algorithm>
#include <utility>
#include <sstream>
#include "DbGlobalInfo.h"
#include "../external/SqliteImgDocException.h"
#include "dbutils.h"

using namespace std;
using namespace SlImgDoc;

/*static*/std::string CDbDocInfo::ColumnName_TilesInfo_Pk = "Pk";
/*static*/std::string CDbDocInfo::ColumnName_TilesInfo_TileX = "TilePosX";
/*static*/std::string CDbDocInfo::ColumnName_TilesInfo_TileY = "TilePosY";
/*static*/std::string CDbDocInfo::ColumnName_TilesInfo_TileWidth = "TileWidth";
/*static*/std::string CDbDocInfo::ColumnName_TilesInfo_TileHeight = "TileHeight";
/*static*/std::string CDbDocInfo::ColumnName_TilesInfo_PyrLvl = "PyramidLevel";
/*static*/std::string CDbDocInfo::ColumnName_TilesInfo_TileDataId = "TileDataId";

/*static*/std::string CDbDocInfo::ColumnName_TilesData_Pk = "rowid";
/*static*/std::string CDbDocInfo::ColumnName_TilesData_PixelWidth = "Pixelwidth";
/*static*/std::string CDbDocInfo::ColumnName_TilesData_PixelHeight = "Pixelheight";
/*static*/std::string CDbDocInfo::ColumnName_TilesData_PixelType = "Pixeltype";
/*static*/std::string CDbDocInfo::ColumnName_TilesData_DataType = "Datatype";
/*static*/std::string CDbDocInfo::ColumnName_TilesData_DataBinHdr = "Data_BinHdr";
/*static*/std::string CDbDocInfo::ColumnName_TilesData_Data = "Data";

/*static*/std::string CDbDocInfo::ColumnName_TilesSpatialIndex_Pk = "id";
/*static*/std::string CDbDocInfo::ColumnName_TilesSpatialIndex_MinX = "minX";
/*static*/std::string CDbDocInfo::ColumnName_TilesSpatialIndex_MaxX = "maxX";
/*static*/std::string CDbDocInfo::ColumnName_TilesSpatialIndex_MinY = "minY";
/*static*/std::string CDbDocInfo::ColumnName_TilesSpatialIndex_MaxY = "maxY";

/*static*/std::string CDbDocInfo::ColumnName_PerTilesData_Pk = "Pk";

CDbDocInfo::CDbDocInfo(
    std::string tableName_tilesdata,
    std::string tableName_tilesinfo,
    std::string tableName_SpatialIndex,
    std::string tableName_CoordinateData)

    : tableNameTilesData(std::move(tableName_tilesdata)),
    tableNameTilesInfo(std::move(tableName_tilesinfo)),
    tableNameSpatialIndex(std::move(tableName_SpatialIndex)),
    tableNameCoordinateData(std::move(tableName_CoordinateData))
{
}

CDbDocInfo::CDbDocInfo() : CDbDocInfo("TILESDATA", "TILESINFO", "TILESPATIAL_index", "COORDINATEDATA")
{
}

/*virtual*/const std::string& CDbDocInfo::GetTableName(TableType tt) const
{
    switch (tt)
    {
    case TableType::TilesData:
        return this->tableNameTilesData;
    case TableType::TilesInfo:
        return this->tableNameTilesInfo;
    case TableType::TilesSpatialIndex:
        return this->tableNameSpatialIndex;
    case TableType::CoordinateData:
        return this->tableNameCoordinateData;
    }

    throw std::invalid_argument("Unknown enumeration");
}

/*virtual*/const std::vector<SlImgDoc::TileDim>& CDbDocInfo::GetTileDimensions() const
{
    return this->dimensions;
}

/*virtual*/const std::string& CDbDocInfo::GetTileInfoColumnName(TilesInfoColumn c) const
{
    /*switch (c)
    {
    case TilesInfoColumn::Pk:
        return CDbDocInfo::ColumnName_TilesInfo_Pk;
    case TilesInfoColumn::TileX:
        return CDbDocInfo::ColumnName_TilesInfo_TileX;
    case TilesInfoColumn::TileY:
        return CDbDocInfo::ColumnName_TilesInfo_TileY;
    case TilesInfoColumn::TileWidth:
        return CDbDocInfo::ColumnName_TilesInfo_TileWidth;
    case TilesInfoColumn::TileHeight:
        return CDbDocInfo::ColumnName_TilesInfo_TileHeight;
    case TilesInfoColumn::PyrLvl:
        return CDbDocInfo::ColumnName_TilesInfo_PyrLvl;
    case TilesInfoColumn::TileDataId:
        return CDbDocInfo::ColumnName_TilesInfo_TileDataId;
    }

    throw std::invalid_argument("Unknown enumeration");*/
    return CDbDocInfo::GetDefaultTileInfoColumnName(c);
}

/*virtual*/bool CDbDocInfo::GetTileInfoColumnNameForDimension(SlImgDoc::TileDim d, std::string& tableName) const
{
    const auto& it = std::find(this->dimensions.cbegin(), this->dimensions.cend(), d);
    if (it == this->dimensions.cend())
    {
        return false;
    }

    tableName = "Dim_";
    tableName += d;
    return true;
}

/*virtual*/const std::string& CDbDocInfo::GetTileDataColumnName(TilesDataColumn c) const
{
    /*
    switch (c)
    {
    case TilesDataColumn::Pk:
        return CDbDocInfo::ColumnName_TilesData_Pk;
    case TilesDataColumn::PixelWidth:
        return CDbDocInfo::ColumnName_TilesData_PixelWidth;
    case TilesDataColumn::PixelHeight:
        return CDbDocInfo::ColumnName_TilesData_PixelHeight;
    case TilesDataColumn::PixelType:
        return CDbDocInfo::ColumnName_TilesData_PixelType;
    case TilesDataColumn::DataType:
        return CDbDocInfo::ColumnName_TilesData_DataType;
    case TilesDataColumn::DataBinHdr:
        return CDbDocInfo::ColumnName_TilesData_DataBinHdr;
    case TilesDataColumn::Data:
        return CDbDocInfo::ColumnName_TilesData_Data;
    }

    throw std::invalid_argument("Unknown enumeration");*/
    return CDbDocInfo::GetDefaultTileDataColumnName(c);
}

/*virtual*/const std::string& CDbDocInfo::GetTilesSpatialIndexColumnName(TilesSpatialIndexColumn c) const
{
    /* switch (c)
     {
     case TilesSpatialIndexColumn::Pk:
         return CDbDocInfo::ColumnName_TilesSpatialIndex_Pk;
     case TilesSpatialIndexColumn::MinX:
         return CDbDocInfo::ColumnName_TilesSpatialIndex_MinX;
     case TilesSpatialIndexColumn::MaxX:
         return CDbDocInfo::ColumnName_TilesSpatialIndex_MaxX;
     case TilesSpatialIndexColumn::MinY:
         return CDbDocInfo::ColumnName_TilesSpatialIndex_MinY;
     case TilesSpatialIndexColumn::MaxY:
         return CDbDocInfo::ColumnName_TilesSpatialIndex_MaxY;
     }

     throw std::invalid_argument("Unknown enumeration");*/
    return CDbDocInfo::GetDefaultSpatialIndexColumnName(c);
}

/*virtual*/std::uint32_t CDbDocInfo::GetDbParameter(DbParameter parameter) const
{
    try
    {
        return this->dbParameters.at(parameter);
    }
    catch (const std::out_of_range & oor)
    {
        stringstream ss;
        ss << "The DbParameter with value \"" << static_cast<typename std::underlying_type<IDbDocInfo::DbParameter>::type>(parameter) << "\" is unknown.";
        throw SqliteImgDocInvalidArgumentException(ss.str(), oor);
    }
}

/*virtual*/bool CDbDocInfo::GetCoordinateDataColumnNameForDimension(SlImgDoc::TileDim d, std::string& columnName) const
{
    const auto& it = std::find(this->dimensions.cbegin(), this->dimensions.cend(), d);
    if (it == this->dimensions.cend())
    {
        return false;
    }

    columnName = "The";
    columnName += *it;
    return true;
}

/*virtual*/const std::vector<ColumnTypeAllInfo>& CDbDocInfo::GetCoordinateDataColumnInfo() const
{
    return this->coordinateDataColumns;
}

/*static*/const std::string& CDbDocInfo::GetDefaultTileInfoColumnName(TilesInfoColumn c)
{
    switch (c)
    {
    case TilesInfoColumn::Pk:
        return CDbDocInfo::ColumnName_TilesInfo_Pk;
    case TilesInfoColumn::TileX:
        return CDbDocInfo::ColumnName_TilesInfo_TileX;
    case TilesInfoColumn::TileY:
        return CDbDocInfo::ColumnName_TilesInfo_TileY;
    case TilesInfoColumn::TileWidth:
        return CDbDocInfo::ColumnName_TilesInfo_TileWidth;
    case TilesInfoColumn::TileHeight:
        return CDbDocInfo::ColumnName_TilesInfo_TileHeight;
    case TilesInfoColumn::PyrLvl:
        return CDbDocInfo::ColumnName_TilesInfo_PyrLvl;
    case TilesInfoColumn::TileDataId:
        return CDbDocInfo::ColumnName_TilesInfo_TileDataId;
    }

    throw std::invalid_argument("Unknown enumeration");
}

/*static*/const std::string& CDbDocInfo::GetDefaultTileDataColumnName(TilesDataColumn c)
{
    switch (c)
    {
    case TilesDataColumn::Pk:
        return CDbDocInfo::ColumnName_TilesData_Pk;
    case TilesDataColumn::PixelWidth:
        return CDbDocInfo::ColumnName_TilesData_PixelWidth;
    case TilesDataColumn::PixelHeight:
        return CDbDocInfo::ColumnName_TilesData_PixelHeight;
    case TilesDataColumn::PixelType:
        return CDbDocInfo::ColumnName_TilesData_PixelType;
    case TilesDataColumn::DataType:
        return CDbDocInfo::ColumnName_TilesData_DataType;
    case TilesDataColumn::DataBinHdr:
        return CDbDocInfo::ColumnName_TilesData_DataBinHdr;
    case TilesDataColumn::Data:
        return CDbDocInfo::ColumnName_TilesData_Data;
    }

    throw std::invalid_argument("Unknown enumeration");
}

/*static*/const std::string& CDbDocInfo::GetDefaultSpatialIndexColumnName(TilesSpatialIndexColumn c)
{
    switch (c)
    {
    case TilesSpatialIndexColumn::Pk:
        return CDbDocInfo::ColumnName_TilesSpatialIndex_Pk;
    case TilesSpatialIndexColumn::MinX:
        return CDbDocInfo::ColumnName_TilesSpatialIndex_MinX;
    case TilesSpatialIndexColumn::MaxX:
        return CDbDocInfo::ColumnName_TilesSpatialIndex_MaxX;
    case TilesSpatialIndexColumn::MinY:
        return CDbDocInfo::ColumnName_TilesSpatialIndex_MinY;
    case TilesSpatialIndexColumn::MaxY:
        return CDbDocInfo::ColumnName_TilesSpatialIndex_MaxY;
    }

    throw std::invalid_argument("Unknown enumeration");
}

/*virtual*/const std::string& CDbDocInfo::GetPerTilesDataColumnName(PerTileDataColumn c) const
{
    switch (c)
    {
    case PerTileDataColumn::Pk:
        return CDbDocInfo::ColumnName_PerTilesData_Pk;
    }

    throw std::invalid_argument("Unknown enumeration");
}

//-----------------------------------------------------------------------------

/*static*/std::vector<ColumnTypeAllInfo> DbDocInfoUtils::Convert(const SlImgDoc::PerTileDataCreateOptions& opts)
{
    std::vector<ColumnTypeAllInfo> r;
    r.reserve(opts.descriptions.size());
    for (const auto& i : opts.descriptions)
    {
        ColumnTypeAllInfo allInfo;
        bool b = DbUtils::TryParse(i.DataType, &allInfo);
        //bool b = TryParse(i.DataType, &allInfo);
        allInfo.columnName = i.Name;
        r.emplace_back(allInfo);
    }

    return r;
}

///*static*/bool DbDocInfoUtils::TryParse(const std::string& str, ColumnTypeInfo* colTypeInfo)
//{
//    ColumnTypeInfo ti;
//    if (str == "FLOAT")
//    {
//        ti.type = ColumnType::Float;
//        ti.size = 0;
//    }
//    else if (str == "INTEGER" || str == "INTEGER(4)")
//    {
//        ti.type = ColumnType::Integer;
//        ti.size = 4;
//    }
//    else if (str == "INTEGER(1)")
//    {
//        ti.type = ColumnType::Integer;
//        ti.size = 1;
//    }
//    else if (str == "INTEGER(2)")
//    {
//        ti.type = ColumnType::Integer;
//        ti.size = 1;
//    }
//    else if (str == "INTEGER(8)")
//    {
//        ti.type = ColumnType::Integer;
//        ti.size = 8;
//    }
//    else
//    {
//        return false;
//    }
//
//    if (colTypeInfo != nullptr)
//    {
//        *colTypeInfo = ti;
//    }
//
//    return true;
//}