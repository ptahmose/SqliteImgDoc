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

    static const char* TableName_TileTable;
};

