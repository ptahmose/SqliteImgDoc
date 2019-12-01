#include "pch.h"

#include <SQLiteCpp/Database.h>
#include <sqlite3.h> // for SQLITE_ERROR and SQLITE_VERSION_NUMBER
#include <sstream>

#include "Interface.h"
#include "DbBase.h"
#include "DbWrite.h"


using namespace SlImgDoc;
using namespace std;

/*
https://www.tutorialspoint.com/sqlite/sqlite_constraints.htm
*/


static void CreateTileTable(SQLite::Database* db)
{
    stringstream ss;
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

/*static*/IDbWrite* IDbFactory::CreateNew(const CreateOptions& opts)
{
    SQLite::Database* db = new SQLite::Database(opts.dbFilename, SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);

    CreateTileTable(db);

    return new CDbWrite(db);
}