#include "pch.h"
#include <stdexcept>
#include "DbGlobalInfo3D.h"
#include "DbGlobalInfo.h"

/*static*/std::string CDbDocInfo3D::ColumnName_TilesInfo_TileZ = "TilePosZ";
/*static*/std::string CDbDocInfo3D::ColumnName_TilesInfo_TileHeight = "TileDepth";
/*static*/std::string CDbDocInfo3D::ColumnName_TilesData_PixelDepth = "Pixeldepth";
/*static*/std::string CDbDocInfo3D::ColumnName_TilesSpatialIndex_MinZ = "minZ";
/*static*/std::string CDbDocInfo3D::ColumnName_TilesSpatialIndex_MaxZ = "maxZ";

/*virtual*/const std::string& CDbDocInfo3D::GetTileInfoColumnName(TilesInfoColumn c) const
{
    switch (c)
    {
    case TilesInfoColumn::Pk:           return CDbDocInfo::GetDefaultTileInfoColumnName(IDbDocInfo::TilesInfoColumn::Pk);
    case TilesInfoColumn::TileX:        return CDbDocInfo::GetDefaultTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileX);
    case TilesInfoColumn::TileY:        return CDbDocInfo::GetDefaultTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileY);
    case TilesInfoColumn::TileZ:        return CDbDocInfo3D::ColumnName_TilesInfo_TileZ;
    case TilesInfoColumn::TileWidth:    return CDbDocInfo::GetDefaultTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileWidth);
    case TilesInfoColumn::TileHeight:   return CDbDocInfo::GetDefaultTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileHeight);
    case TilesInfoColumn::TileDepth:    return CDbDocInfo3D::ColumnName_TilesInfo_TileHeight;
    case TilesInfoColumn::PyrLvl:       return CDbDocInfo::GetDefaultTileInfoColumnName(IDbDocInfo::TilesInfoColumn::PyrLvl);
    case TilesInfoColumn::TileDataId:   return CDbDocInfo::GetDefaultTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileDataId);
    }

    throw std::invalid_argument("Unknown enumeration");
}

/*virtual*/const std::string& CDbDocInfo3D::GetTileDataColumnName(TilesDataColumn c) const
{
    switch (c)
    {
    case TilesDataColumn::Pk:           return CDbDocInfo::GetDefaultTileDataColumnName(IDbDocInfo::TilesDataColumn::Pk);
    case TilesDataColumn::PixelWidth:   return CDbDocInfo::GetDefaultTileDataColumnName(IDbDocInfo::TilesDataColumn::PixelWidth);
    case TilesDataColumn::PixelHeight:  return CDbDocInfo::GetDefaultTileDataColumnName(IDbDocInfo::TilesDataColumn::PixelHeight);
    case TilesDataColumn::PixelDepth:   return CDbDocInfo3D::ColumnName_TilesData_PixelDepth;
    case TilesDataColumn::PixelType:    return CDbDocInfo::GetDefaultTileDataColumnName(IDbDocInfo::TilesDataColumn::PixelType);
    case TilesDataColumn::DataType:     return CDbDocInfo::GetDefaultTileDataColumnName(IDbDocInfo::TilesDataColumn::DataType);
    case TilesDataColumn::DataBinHdr:   return CDbDocInfo::GetDefaultTileDataColumnName(IDbDocInfo::TilesDataColumn::DataBinHdr);
    case TilesDataColumn::Data:         return CDbDocInfo::GetDefaultTileDataColumnName(IDbDocInfo::TilesDataColumn::Data);
    }

    throw std::invalid_argument("Unknown enumeration");
}

/*virtual*/const std::string& CDbDocInfo3D::GetTilesSpatialIndexColumnName(TilesSpatialIndexColumn c) const
{
    switch (c)
    {
    case TilesSpatialIndexColumn::Pk:       return CDbDocInfo::GetDefaultSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::Pk);
    case TilesSpatialIndexColumn::MinX:     return CDbDocInfo::GetDefaultSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MinX);
    case TilesSpatialIndexColumn::MaxX:     return CDbDocInfo::GetDefaultSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MaxX);
    case TilesSpatialIndexColumn::MinY:     return CDbDocInfo::GetDefaultSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MinY);
    case TilesSpatialIndexColumn::MaxY:     return CDbDocInfo::GetDefaultSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MaxY);
    case TilesSpatialIndexColumn::MinZ:     return CDbDocInfo3D::ColumnName_TilesSpatialIndex_MinZ;
    case TilesSpatialIndexColumn::MaxZ:     return CDbDocInfo3D::ColumnName_TilesSpatialIndex_MaxZ;
    }

    throw std::invalid_argument("Unknown enumeration");
}