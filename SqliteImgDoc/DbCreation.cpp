#include "pch.h"

#include <SQLiteCpp/Database.h>
#include <sqlite3.h> // for SQLITE_ERROR and SQLITE_VERSION_NUMBER

#include "Interface.h"
#include "DbBase.h"
#include "DbWrite.h"

using namespace SlImgDoc;

/*
https://www.tutorialspoint.com/sqlite/sqlite_constraints.htm
*/


static void CreateTileTable(SQLite::Database* db)
{
    static const char* CreateTileTableSqlStatement = 
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
    db->exec(CreateTileTableSqlStatement);
    db->exec(CreateSubBlkTableSqlStatement);
}

/*static*/IDbWrite* IDbFactory::CreateNew(const CreateOptions& opts)
{
    SQLite::Database* db = new SQLite::Database(opts.dbFilename, SQLite::OPEN_CREATE|SQLite::OPEN_READWRITE);

    CreateTileTable(db);

    return new CDbWrite(db);
}