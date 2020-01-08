#include "pch.h"

#include <SQLiteCpp/Database.h>
#include <sqlite3.h> // for SQLITE_ERROR and SQLITE_VERSION_NUMBER
#include <sstream>
#include <iostream>

#include "DbCreation3D.h"
#include "../external/Interface.h"
#include "DbBase.h"
#include "DbWrite.h"
#include "DbRead.h"
#include "DbDiscover.h"
#include "CCustomQueries.h"
#include "DbGlobalInfo3D.h"
#include "DbMasterInfoTable.h"


using namespace SlImgDoc;
using namespace std;

CDbCreation3D::CDbCreation3D(const IDbDocInfo3D& docInfo, const SlImgDoc::CreateOptions& opts)
    : docInfo(docInfo), opts(opts)
{
}

SQLite::Database* CDbCreation3D::DoCreate()
{
    try
    {
        SQLite::Database* db = new SQLite::Database(opts.dbFilename, SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE | SQLite::OPEN_URI);
        CCustomQueries::SetupCustomQueries(db->getHandle());

        CDbMasterInfoTableHelper::AddMasterTable(db, "0.0.1-alpha");
        CDbMasterInfoTableHelper::AddDocumentTiles3D(
            db,
            CDbDocInfo3D::GetDefaultTableName);

        auto sqlStatement = this->GetTilesInfoCreateSqlStatement();
        db->exec(sqlStatement);
        sqlStatement = this->GetTilesDataCreateSqlStatement();
        db->exec(sqlStatement);
        sqlStatement = this->GetTilesSpatialIndexCreateSqlStatement();
        db->exec(sqlStatement);

        sqlStatement = this->GetPerTileDataTableSqlStatement();
        if (!sqlStatement.empty())
        {
            db->exec(sqlStatement);
        }

        return db;
    }
    catch (SQLite::Exception & excp)
    {
        std::cout << excp.what();
    }

    return nullptr;
}

std::string CDbCreation3D::GetTilesInfoCreateSqlStatement() const
{
    auto ss = stringstream();
    ss << "CREATE TABLE[" << this->docInfo.GetTableName(IDbDocInfo3D::TableType::TilesInfo) << "]("
        "[" << this->docInfo.GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::Pk) << "] INTEGER NOT NULL PRIMARY KEY,"
        "[" << this->docInfo.GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::TileX) << "] DOUBLE NOT NULL,"
        "[" << this->docInfo.GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::TileY) << "] DOUBLE NOT NULL,"
        "[" << this->docInfo.GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::TileZ) << "] DOUBLE NOT NULL,"
        "[" << this->docInfo.GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::TileWidth) << "] DOUBLE NOT NULL,"
        "[" << this->docInfo.GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::TileHeight) << "] DOUBLE NOT NULL,"
        "[" << this->docInfo.GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::TileDepth) << "] DOUBLE NOT NULL,"
        "[" << this->docInfo.GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::PyrLvl) << "] INTEGER(1),"
        "[" << this->docInfo.GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::TileDataId) << "] INTEGER(8)";

    const auto tileDims = this->docInfo.GetTileDimensions();
    for (const auto dim : tileDims)
    {
        string colName;
        bool b = this->docInfo.GetTileInfoColumnNameForDimension(dim, colName);
        ss << ", [" << colName << "] INTEGER(4)";
    }

    ss << ");";
    return ss.str();
}

std::string CDbCreation3D::GetTilesDataCreateSqlStatement() const
{
    auto ss = stringstream();
    ss << "CREATE TABLE[" << this->docInfo.GetTableName(IDbDocInfo3D::TableType::TilesData) << "]("
        "[" << this->docInfo.GetTileDataColumnName(IDbDocInfo3D::TilesDataColumn::PixelWidth) << "] INTEGER(4),"
        "[" << this->docInfo.GetTileDataColumnName(IDbDocInfo3D::TilesDataColumn::PixelHeight) << "] INTEGER(4),"
        "[" << this->docInfo.GetTileDataColumnName(IDbDocInfo3D::TilesDataColumn::PixelDepth) << "] INTEGER(4),"
        "[" << this->docInfo.GetTileDataColumnName(IDbDocInfo3D::TilesDataColumn::PixelType) << "] INTEGER(1),"
        "[" << this->docInfo.GetTileDataColumnName(IDbDocInfo3D::TilesDataColumn::DataType) << "] INTEGER(4),"
        "[" << this->docInfo.GetTileDataColumnName(IDbDocInfo3D::TilesDataColumn::DataBinHdr) << "] BLOB(32),"
        "[" << this->docInfo.GetTileDataColumnName(IDbDocInfo3D::TilesDataColumn::Data) << "] BLOB);";
    return ss.str();
}

std::string CDbCreation3D::GetTilesSpatialIndexCreateSqlStatement() const
{
    auto ss = stringstream();
    ss << "CREATE VIRTUAL TABLE " << this->docInfo.GetTableName(IDbDocInfo3D::TableType::TilesSpatialIndex) << " USING rtree(" <<
        this->docInfo.GetTilesSpatialIndexColumnName(IDbDocInfo3D::TilesSpatialIndexColumn::Pk) << "," <<         // Integer primary key
        this->docInfo.GetTilesSpatialIndexColumnName(IDbDocInfo3D::TilesSpatialIndexColumn::MinX) << "," <<       // Minimum X coordinate"
        this->docInfo.GetTilesSpatialIndexColumnName(IDbDocInfo3D::TilesSpatialIndexColumn::MaxX) << "," <<       // Maximum X coordinate"
        this->docInfo.GetTilesSpatialIndexColumnName(IDbDocInfo3D::TilesSpatialIndexColumn::MinY) << "," <<       // Minimum Y coordinate"
        this->docInfo.GetTilesSpatialIndexColumnName(IDbDocInfo3D::TilesSpatialIndexColumn::MaxY) << "," <<       // Maximum Y coordinate"
        this->docInfo.GetTilesSpatialIndexColumnName(IDbDocInfo3D::TilesSpatialIndexColumn::MinZ) << "," <<       // Minimum Y coordinate"
        this->docInfo.GetTilesSpatialIndexColumnName(IDbDocInfo3D::TilesSpatialIndexColumn::MaxZ) << ");";        // Maximum Y coordinate"
    return ss.str();
}

std::string CDbCreation3D::GetPerTileDataTableSqlStatement() const
{
    struct Adapter
    {
        const CDbCreation3D& r;
        Adapter(const CDbCreation3D& r) :r(r) {};

        const std::vector<ColumnTypeAllInfo>& GetPerTileColumnInfo() const { return this->r.docInfo.GetCoordinateDataColumnInfo(); }
        std::string GetTableNamePerTileData() const { return this->r.docInfo.GetTableName(IDbDocInfo3D::TableType::PerBrickData); }
        std::string GetColumnNamePk() const { return this->r.docInfo.GetPerTilesDataColumnName(IDbDocInfo3D::PerTileDataColumn::Pk); }
        std::string GetTableNameTilesInfo() const { return this->r.docInfo.GetTableName(IDbDocInfo3D::TableType::TilesInfo); }
        std::string GetColumnNamePkTilesInfo() const { return this->r.docInfo.GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::Pk); }
    };

    return this->GetPerTileDataTableSqlStatementCommon(Adapter(*this));
    /*

    const auto& coordinateDataColInfo = this->docInfo.GetCoordinateDataColumnInfo();
    if (coordinateDataColInfo.empty())
    {
        return string();
    }

    auto ss = stringstream();
    ss << "CREATE TABLE[" << this->docInfo.GetTableName(IDbDocInfo3D::TableType::PerBrickData) << "](";
    ss << this->docInfo.GetPerTilesDataColumnName(IDbDocInfo3D::PerTileDataColumn::Pk) << " INTEGER";

    for (const auto& ci : this->docInfo.GetCoordinateDataColumnInfo())
    {
        switch (ci.type)
        {
        case ColumnType::Integer:
            ss << ",[" << ci.columnName << "] INTEGER(" << ci.size << ")";
            break;
        case ColumnType::Float:
            ss << ",[" << ci.columnName << "] DOUBLE";
            break;
        }
    }

    ss << ",FOREIGN KEY(" << this->docInfo.GetPerTilesDataColumnName(IDbDocInfo3D::PerTileDataColumn::Pk) << ") REFERENCES " <<
        this->docInfo.GetTableName(IDbDocInfo3D::TableType::TilesInfo) << "(" << this->docInfo.GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::Pk) << ")";

    ss << ");";

    return ss.str();

    */
}