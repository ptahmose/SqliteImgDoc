#pragma once

#include <memory>
#include "Interface.h"
#include <SQLiteCpp/Database.h>

class CDbBase
{
private:
    std::unique_ptr<SQLite::Database> db;
public:
    CDbBase(SQLite::Database* db) : db(db)
    {
    }

    virtual ~CDbBase()
    {}

protected:
    SQLite::Database& GetDb() { return *this->db.get(); }

public:
    /// The name of the table with the "technical document info"
    static const char* TableName_DocumentInfo;

    /// The name of the table with the "tile list"
    static const char* TableName_TileTable;

    /// The name of the table with the "tile data"
    static const char* TableName_TileData;

    /// The name of the table with the "spatial index"
    static const char* VTableName_SpatialTable;

    static const char* DocumentInfoColName_Version;
    static const char* DocumentInfoColName_CoordinateDimensions;
    static const char* DocumentInfoColName_IndexedDimensions;
};

