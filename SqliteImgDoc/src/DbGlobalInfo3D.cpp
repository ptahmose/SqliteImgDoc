#include "pch.h"
#include <stdexcept>
#include "DbGlobalInfo3D.h"
#include "DbGlobalInfo.h"

/*static*/std::string CDbDocInfo3D::ColumnName_TilesInfo_TileZ = "TilePosZ";
/*static*/std::string CDbDocInfo3D::ColumnName_TilesInfo_TileHeight = "TileDepth";

/*virtual*/const std::string& CDbDocInfo3D::GetTileInfoColumnName(TilesInfoColumn c) const
{
    switch (c)
    {
    case TilesInfoColumn::Pk: return CDbDocInfo::GetDefaultTileInfoColumnName(IDbDocInfo::TilesInfoColumn::Pk);
    case TilesInfoColumn::TileX:return CDbDocInfo::GetDefaultTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileX);
    case TilesInfoColumn::TileY:return CDbDocInfo::GetDefaultTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileY);
    case TilesInfoColumn::TileZ:return CDbDocInfo3D::ColumnName_TilesInfo_TileZ;
    case TilesInfoColumn::TileWidth:return CDbDocInfo::GetDefaultTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileWidth);
    case TilesInfoColumn::TileHeight:return CDbDocInfo::GetDefaultTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileHeight);
    case TilesInfoColumn::TileDepth: return CDbDocInfo3D::ColumnName_TilesInfo_TileHeight;
    case TilesInfoColumn::PyrLvl:return CDbDocInfo::GetDefaultTileInfoColumnName(IDbDocInfo::TilesInfoColumn::PyrLvl);
    case TilesInfoColumn::TileDataId:return CDbDocInfo::GetDefaultTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileDataId);
    }

    throw std::invalid_argument("Unknown enumeration");
}