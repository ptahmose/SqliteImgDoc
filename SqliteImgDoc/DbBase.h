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
};

class CDbWrite : CDbBase, public SlImgDoc::IDbWrite
{
private:
public:
    CDbWrite(SQLite::Database* db) : CDbBase(db)
    {
    }

    virtual ~CDbWrite()
    {}
};