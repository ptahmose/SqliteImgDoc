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
    static const char* TableName_TileTable;
    static const char* TableName_TileData;
    static const char* VTableName_SpatialTable;
};

