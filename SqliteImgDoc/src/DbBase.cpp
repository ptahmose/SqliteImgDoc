#include "pch.h"
#include "DbBase.h"

/*static*/const char* CDbBase::TableName_DocumentInfo = "DOCINFO";
/*static*/const char* CDbBase::TableName_TileTable = "TILES";
/*static*/const char* CDbBase::TableName_TileData = "TILEDATA";
/*static*/const char* CDbBase::VTableName_SpatialTable = "TILESPATIAL_index";


/*static*/const char* CDbBase::DocumentInfoColName_Version = "Version";
/*static*/const char* CDbBase::DocumentInfoColName_CoordinateDimensions = "CoordDims";
/*static*/const char* CDbBase::DocumentInfoColName_IndexedDimensions = "IndexedDims";