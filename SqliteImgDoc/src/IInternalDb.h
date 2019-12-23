#pragma once
#include <SQLiteCpp/Database.h>

class IInternalDb
{
public:
    virtual SQLite::Database& GetDb() = 0;
};
