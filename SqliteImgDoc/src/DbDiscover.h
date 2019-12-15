#pragma once
#include <memory>
#include <SQLiteCpp/Database.h>
#include "DbGlobalInfo.h"

class CDbDiscover
{
    SQLite::Database* db;
public:
    CDbDiscover(SQLite::Database* db) : db(db){}

    std::shared_ptr<IDbDocInfo> GetDocInfo();
private:
    std::vector<std::string> GetColumnNamesStartingWith(const char* tableName, const char* startsWith);
    std::vector<SlImgDoc::TileDim> GetTileDims(std::vector<std::string> colNames);
};
