#include "pch.h"

#include <SQLiteCpp/Database.h>
#include <sqlite3.h> // for SQLITE_ERROR and SQLITE_VERSION_NUMBER

#include "Interface.h"
#include "DbBase.h"

using namespace SlImgDoc;

/*
https://www.tutorialspoint.com/sqlite/sqlite_constraints.htm
*/



/*static*/IDbWrite* IDbFactory::CreateNew(const CreateOptions& opts)
{
    SQLite::Database* db = new SQLite::Database(opts.dbFilename, SQLite::OPEN_CREATE|SQLite::OPEN_READWRITE);

    return new CDbWrite(db);
}