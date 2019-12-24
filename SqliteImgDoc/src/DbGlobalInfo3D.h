#pragma once

#include <string>
#include <algorithm>
#include <map>
#include "../external/ITileCoordinate.h"

class IDbDocInfo3D
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
		TileZ,
		TileWidth,
		TileHeight,
		TileDepth,
		PyrLvl,
		TileDataId
	};

	enum class TilesDataColumn
	{
		Pk,
		PixelWidth,
		PixelHeight,
		PixelDepth,
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
		MaxY,
		MinZ,
		MaxZ

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

	virtual const std::string& GetTileDataColumnName(TilesDataColumn c) const = 0;

	virtual const std::string& GetTilesSpatialIndexColumnName(TilesSpatialIndexColumn c) const = 0;

	virtual std::uint32_t GetDbParameter(DbParameter parameter) const = 0;
};

class CDbDocInfo3D :public IDbDocInfo3D
{
private:
	static std::string ColumnName_TilesInfo_TileZ;
	static std::string ColumnName_TilesInfo_TileHeight;

	static std::string ColumnName_TilesData_PixelDepth;

	static std::string ColumnName_TilesSpatialIndex_MinZ;
	static std::string ColumnName_TilesSpatialIndex_MaxZ;
public:
	virtual const std::string& GetTableName(TableType tt) const;
	virtual const std::vector<SlImgDoc::TileDim>& GetTileDimensions() const;
	virtual const std::string& GetTileInfoColumnName(TilesInfoColumn c) const;
	virtual bool GetTileInfoColumnNameForDimension(SlImgDoc::TileDim d, std::string& columnName) const;
	virtual const std::string& GetTileDataColumnName(TilesDataColumn c) const;
	virtual const std::string& GetTilesSpatialIndexColumnName(TilesSpatialIndexColumn c) const;
	virtual std::uint32_t GetDbParameter(DbParameter parameter) const;
};