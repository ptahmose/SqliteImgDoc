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
		TilesSpatialIndex,
		PerBrickData
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
	std::vector< SlImgDoc::TileDim> dimensions;
	std::string tableNameBricksData;
	std::string tableNameBricksInfo;
	std::string tableNameSpatialIndex;
	std::string tableNamePerTileData;
	std::map<IDbDocInfo3D::DbParameter, std::uint32_t> dbParameters;
private:
	static std::string TableName_BricksData;
	static std::string TableName_BricksInfo;
	static std::string TableName_BricksSpatialIndex;
	static std::string TableName_PerBricksData;

	static std::string ColumnName_TilesInfo_TileZ;
	static std::string ColumnName_TilesInfo_TileHeight;

	static std::string ColumnName_TilesData_PixelDepth;

	static std::string ColumnName_TilesSpatialIndex_MinZ;
	static std::string ColumnName_TilesSpatialIndex_MaxZ;
public:
	CDbDocInfo3D(std::string tableName_bricksdata, std::string tableName_bricksinfo, std::string tableName_SpatialIndex);
	CDbDocInfo3D();

	template<typename ForwardIterator>
	void SetTileDimensions(ForwardIterator begin, ForwardIterator end)
	{
		this->dimensions.clear();
		std::copy(begin, end, std::back_inserter(this->dimensions));
	}

	void SetDbParameters(std::map<IDbDocInfo3D::DbParameter, std::uint32_t>&& dbParams)
	{
		this->dbParameters = std::move(dbParams);
	}
public:
	virtual const std::string& GetTableName(TableType tt) const;
	virtual const std::vector<SlImgDoc::TileDim>& GetTileDimensions() const;
	virtual const std::string& GetTileInfoColumnName(TilesInfoColumn c) const;
	virtual bool GetTileInfoColumnNameForDimension(SlImgDoc::TileDim d, std::string& columnName) const;
	virtual const std::string& GetTileDataColumnName(TilesDataColumn c) const;
	virtual const std::string& GetTilesSpatialIndexColumnName(TilesSpatialIndexColumn c) const;
	virtual std::uint32_t GetDbParameter(DbParameter parameter) const;

public:
	static const std::string& GetDefaultTableName(TableType tt);
};