#include "pch.h"
#include "DbDiscover.h"
#include <iostream>
#include <sstream>

using namespace std;

std::shared_ptr<IDbDocInfo> CDbDiscover::GetDocInfo()
{
    auto colNames = this->GetColumnNamesStartingWith("TILESINFO", "DIM_");
    auto dims = this->GetTileDims(colNames);
    /*SQLite::Statement query(*this->db, "SELECT name FROM PRAGMA_TABLE_INFO('TILESINFO');");

    bool b = query.executeStep();

    int count = query.getColumnCount();

    for (int i = 0; i < count; ++i)
    {
        auto colName = query.getColumn(i);
        std::cout << colName << endl;
    }*/
    auto docInfo = std::make_shared< CDbDocInfo>();
    docInfo->SetTileDimensions(dims.cbegin(), dims.cend());

    return docInfo;
    //throw std::invalid_argument("Unknown enumeration");
}

std::vector<std::string> CDbDiscover::GetColumnNamesStartingWith(const char* tableName, const char* startsWith)
{
    stringstream ss;
    ss << "SELECT SUBSTR(name," << strlen(startsWith) + 1 << ") FROM PRAGMA_TABLE_INFO('" << tableName << "') WHERE name LIKE '" << startsWith << "%';";

    SQLite::Statement query(*this->db, ss.str());

    std::vector<std::string> result;

    while (query.executeStep())
    {
        result.push_back(query.getColumn(0));
    }

    return result;
}

std::vector<SlImgDoc::TileDim> CDbDiscover::GetTileDims(std::vector<std::string> colNames)
{
    std::vector<SlImgDoc::TileDim> dims;
    dims.reserve(colNames.size());

    for (const auto& s : colNames)
    {
        dims.push_back(s[0]);
    }

    return dims;
}