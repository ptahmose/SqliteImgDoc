#include "pch.h"
#include <stdexcept> 
#include <algorithm>
#include "DbGlobalInfo.h"

/*static*/std::string CDbDocInfo::ColumnName_TilesInfo_Pk = "Pk";
/*static*/std::string CDbDocInfo::ColumnName_TilesInfo_TileX = "TilePosX";
/*static*/std::string CDbDocInfo::ColumnName_TilesInfo_TileY = "TilePosY";
/*static*/std::string CDbDocInfo::ColumnName_TilesInfo_TileWidth = "TileWidth";
/*static*/std::string CDbDocInfo::ColumnName_TilesInfo_TileHeight = "TileHeight";
/*static*/std::string CDbDocInfo::ColumnName_TilesInfo_PyrLvl = "PyramidLevel";
/*static*/std::string CDbDocInfo::ColumnName_TilesInfo_TileDataId = "TileDataId";

/*static*/std::string CDbDocInfo::ColumnName_TilesData_PixelWidth = "Pixelwidth";
/*static*/std::string CDbDocInfo::ColumnName_TilesData_PixelHeight = "Pixelheight";
/*static*/std::string CDbDocInfo::ColumnName_TilesData_PixelType = "Pixeltype";
/*static*/std::string CDbDocInfo::ColumnName_TilesData_DataType = "Datatype";
/*static*/std::string CDbDocInfo::ColumnName_TilesData_DataBinHdr = "Data_BinHdr";
/*static*/std::string CDbDocInfo::ColumnName_TilesData_Data = "Data";


/*virtual*/const std::string& CDbDocInfo::GetTableName(TableType tt) const
{

}

/*virtual*/const std::vector<SlImgDoc::TileDim>& CDbDocInfo::GetTileDimensions() const
{
    return this->dimensions;
}

/*virtual*/const std::string& CDbDocInfo::GetTileInfoColumnName(TilesInfoColumn c) const
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

/*virtual*/bool CDbDocInfo::GetTileInfoColumnNameForDimension(SlImgDoc::TileDim d, std::string& tableName) const
{
    const auto it = std::find(this->dimensions.cbegin(), this->dimensions.cend(), d);
    if (it != this->dimensions.cend())
    {
        return false;
    }

    tableName = "Dim_" + d;
    return true;
}

/*virtual*/const std::string& CDbDocInfo::GetTileDataColumnName(TilesDataColumn c) const
{
    switch (c)
    {
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