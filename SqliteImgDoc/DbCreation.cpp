#include "pch.h"

#include <SQLiteCpp/Database.h>
#include <sqlite3.h> // for SQLITE_ERROR and SQLITE_VERSION_NUMBER
#include <sstream>
#include <iostream>

#include "DbCreation.h"
#include "Interface.h"
#include "DbBase.h"
#include "DbWrite.h"
#include "DbRead.h"
#include "DbDiscover.h"
#include "CCustomQueries.h"


using namespace SlImgDoc;
using namespace std;

/*
https://www.tutorialspoint.com/sqlite/sqlite_constraints.htm
*/


static void CreateTileTable(SQLite::Database* db)
{
    stringstream ss;
    ss << "CREATE TABLE[" << CDbBase::TableName_DocumentInfo << "]("
        "[" << CDbBase::DocumentInfoColName_Version << "] INTEGER(8),"
        "[" << CDbBase::DocumentInfoColName_CoordinateDimensions << "]   TEXT,"
        "[" << CDbBase::DocumentInfoColName_IndexedDimensions << "] TEXT"
        ");";
    db->exec(ss.str());

    ss = stringstream();
    ss << "CREATE TABLE[" << CDbBase::TableName_TileTable << "]("
        "[PK] INTEGER NOT NULL PRIMARY KEY,"
        "[C] INTEGER(4),"
        "[T] INTEGER(4),"
        "[Z] INTEGER(4),"
        "[M] INTEGER(4),"
        "[X] DOUBLE,"
        "[Y] DOUBLE,"
        "[WIDTH] DOUBLE,"
        "[HEIGHT] DOUBLE,"
        "[PYRLVL] INTEGER,"
        "[CUSTATTR1] INTEGER(4),"
        "[CUSTATTR2] INTEGER(4),"
        "[SUBBLK] INTEGER);";
    db->exec(ss.str());

    ss = stringstream();
    ss << "CREATE TABLE[" << CDbBase::TableName_TileData << "]("
        "[PIXELWIDTH] INTEGER(4),"
        "[PIXELHEIGHT] INTEGER(4),"
        "[PIXELTYPE] INTEGER(1),"
        "[DATATYPE] INTEGER(4),"
        "[DATA_BINHDR] BLOB(32),"
        "[DATA] BLOB);";
    db->exec(ss.str());

    ss = stringstream();
    ss << "CREATE VIRTUAL TABLE " << CDbBase::VTableName_SpatialTable << " USING rtree("
        "id,"           // Integer primary key
        "minX, maxX,"   // Minimum and maximum X coordinate"
        "minY, maxY"    // Minimum and maximum Y coordinate"
        ");";
    db->exec(ss.str());

    /*  static const char* CreateTileTableSqlStatement =
          R"(CREATE TABLE[TILETABLE](
              [PK] INTEGER NOT NULL PRIMARY KEY,
              [C] INTEGER(4),
              [T] INTEGER(4),
              [Z] INTEGER(4),
              [M] INTEGER(4),
              [X] DOUBLE,
              [Y] DOUBLE,
              [WIDTH] DOUBLE,
              [HEIGHT] DOUBLE,
              [PYRLVL] INTEGER,
              [CUSTATTR1] INTEGER(4),
              [CUSTATTR2] INTEGER(4),
              [SUBBLK] INTEGER);)";
      static const char* CreateSubBlkTableSqlStatement =
          R"(CREATE TABLE[SUBBLKTABLE](
              [PIXELWIDTH] INTEGER(4),
              [PIXELHEIGHT] INTEGER(4),
              [PIXELTYPE] INTEGER(1),
              [DATATYPE] INTEGER(4),
              [DATA_BINHDR] BLOB(32),
              [DATA] BLOB);)";
      static const char* CreateSpatialIndexSqlStatement =
          R"(CREATE VIRTUAL TABLE TILESPATIAL_index USING rtree(
               id,              -- Integer primary key
               minX, maxX,      -- Minimum and maximum X coordinate
               minY, maxY       -- Minimum and maximum Y coordinate
              );)";
      db->exec(CreateTileTableSqlStatement);
      db->exec(CreateSubBlkTableSqlStatement);
      db->exec(CreateSpatialIndexSqlStatement);*/
}

#if false
/*static*/IDbWrite* IDbFactory::CreateNew(const CreateOptions& opts)
{
    /* SQLite::Database* db = new SQLite::Database(opts.dbFilename, SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
     try
     {
         CreateTileTable(db);
     }
     catch (SQLite::Exception & excp)
     {
         std::cout << excp.what();
     }*/
    auto docInfo = std::make_shared<CDbDocInfo>();
    docInfo->SetTileDimensions(opts.dimensions.cbegin(), opts.dimensions.cend());
    CDbCreation dbCreator(*docInfo, opts);
    auto db = dbCreator.DoCreate();

    return new CDbWrite(db, docInfo);
}
#endif
/*static*/ std::shared_ptr<IDb> IDbFactory::CreateNew(const CreateOptions& opts)
{
    auto docInfo = std::make_shared<CDbDocInfo>();
    docInfo->SetTileDimensions(opts.dimensions.cbegin(), opts.dimensions.cend());
    CDbCreation dbCreator(*docInfo, opts);
    auto db = dbCreator.DoCreate();

    return make_shared<CDb>(db, docInfo);
}

#if false
/*static*/IDbRead* IDbFactory::OpenExisting(const OpenOptions& opts)
{
    /* SQLite::Database* db = new SQLite::Database(opts.dbFilename, SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
     try
     {
         CreateTileTable(db);
     }
     catch (SQLite::Exception & excp)
     {
         std::cout << excp.what();
     }*/
    SQLite::Database* db = new SQLite::Database(opts.dbFilename, SQLite::OPEN_READONLY);
    CCustomQueries::SetupCustomQueries(db->getHandle());
    CDbDiscover discover(db);
    auto docInfo = discover.GetDocInfo();

    return new CDbRead(db, docInfo);
}
#endif
/*static*/std::shared_ptr<IDb> IDbFactory::OpenExisting(const OpenOptions& opts)
{
    SQLite::Database* db = new SQLite::Database(opts.dbFilename, SQLite::OPEN_READONLY);
    CCustomQueries::SetupCustomQueries(db->getHandle());
    CDbDiscover discover(db);
    auto docInfo = discover.GetDocInfo();

    return make_shared<CDb>(db, docInfo);
}

//-------------------------------------------------------------------------------------------------

CDbCreation::CDbCreation(const IDbDocInfo& docInfo, const SlImgDoc::CreateOptions& opts)
    : docInfo(docInfo), opts(opts)
{
}

SQLite::Database* CDbCreation::DoCreate()
{
    try
    {
        SQLite::Database* db = new SQLite::Database(opts.dbFilename, SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
        CCustomQueries::SetupCustomQueries(db->getHandle());

        auto sqlStatement = this->GetTilesInfoCreateSqlStatement();
        db->exec(sqlStatement);
        sqlStatement = this->GetTilesDataCreateSqlStatement();
        db->exec(sqlStatement);
        sqlStatement = this->GetTilesSpatialIndexCreateSqlStatement();
        db->exec(sqlStatement);
        return db;
    }
    catch (SQLite::Exception & excp)
    {
        std::cout << excp.what();
    }

    return nullptr;
}

std::string CDbCreation::GetTilesInfoCreateSqlStatement() const
{
    auto ss = stringstream();
    ss << "CREATE TABLE[" << this->docInfo.GetTableName(IDbDocInfo::TableType::TilesInfo) << "]("
        "[" << this->docInfo.GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::Pk) << "] INTEGER NOT NULL PRIMARY KEY,"
        "[" << this->docInfo.GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileX) << "] DOUBLE NOT NULL,"
        "[" << this->docInfo.GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileY) << "] DOUBLE NOT NULL,"
        "[" << this->docInfo.GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileWidth) << "] DOUBLE NOT NULL,"
        "[" << this->docInfo.GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileHeight) << "] DOUBLE NOT NULL,"
        "[" << this->docInfo.GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::PyrLvl) << "] INTEGER(1),"
        "[" << this->docInfo.GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileDataId) << "] INTEGER(8)";

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

std::string CDbCreation::GetTilesDataCreateSqlStatement() const
{
    auto ss = stringstream();
    ss << "CREATE TABLE[" << this->docInfo.GetTableName(IDbDocInfo::TableType::TilesData) << "]("
        "[" << this->docInfo.GetTileDataColumnName(IDbDocInfo::TilesDataColumn::PixelWidth) << "] INTEGER(4),"
        "[" << this->docInfo.GetTileDataColumnName(IDbDocInfo::TilesDataColumn::PixelHeight) << "] INTEGER(4),"
        "[" << this->docInfo.GetTileDataColumnName(IDbDocInfo::TilesDataColumn::PixelType) << "] INTEGER(1),"
        "[" << this->docInfo.GetTileDataColumnName(IDbDocInfo::TilesDataColumn::DataType) << "] INTEGER(4),"
        "[" << this->docInfo.GetTileDataColumnName(IDbDocInfo::TilesDataColumn::DataBinHdr) << "] BLOB(32),"
        "[" << this->docInfo.GetTileDataColumnName(IDbDocInfo::TilesDataColumn::Data) << "] BLOB);";
    return ss.str();
}

std::string CDbCreation::GetTilesSpatialIndexCreateSqlStatement() const
{
    auto ss = stringstream();
    ss << "CREATE VIRTUAL TABLE " << this->docInfo.GetTableName(IDbDocInfo::TableType::TilesSpatialIndex) << " USING rtree(" <<
        this->docInfo.GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::Pk) << "," <<         // Integer primary key
        this->docInfo.GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MinX) << "," <<       // Minimum X coordinate"
        this->docInfo.GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MaxX) << "," <<       // Maximum X coordinate"
        this->docInfo.GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MinY) << "," <<       // Minimum Y coordinate"
        this->docInfo.GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MaxY) << ");";        // Maximum Y coordinate"
    return ss.str();
}
