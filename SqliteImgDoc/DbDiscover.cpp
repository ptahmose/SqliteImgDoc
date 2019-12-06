#include "pch.h"
#include "DbDiscover.h"

std::shared_ptr<IDbDocInfo> CDbDiscover::GetDocInfo()
{
    SQLite::Statement query(*this->db, "pragma table_info ('TILESINFO')");

    query.executeStep();

    throw std::invalid_argument("Unknown enumeration");
}