#include "pch.h"
#include "DbDiscover.h"
#include <iostream>
#include <sstream>
#include <regex>
#include "miscutilities.h"
#include "../external/SqliteImgDocException.h"

using namespace std;
using namespace SlImgDoc;

std::shared_ptr<IDbDocInfo> CDbDiscover::GetDocInfo()
{
    const auto colNames = this->GetColumnNamesStartingWith("TILESINFO", "DIM_");
    const auto dims = this->GetTileDims(colNames);

    auto docInfo = std::make_shared< CDbDocInfo>();
    docInfo->SetTileDimensions(dims.cbegin(), dims.cend());

    std::map<IDbDocInfo::DbParameter, std::uint32_t> dbParams;
    dbParams[IDbDocInfo::DbParameter::DataBinHdrSize] =this->GetSchemaSizeOfColumn("TILESDATA", "Data_BinHdr");
    docInfo->SetDbParameters(std::move(dbParams));

    return docInfo;
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

std::uint32_t CDbDiscover::GetSchemaSizeOfColumn(const char* tableName, const char* columnName)
{
    stringstream ss;
    ss << "PRAGMA table_info(" << tableName << ")";
    SQLite::Statement query(*this->db, ss.str());
    const int colIdx_name = query.getColumnIndex("name");
    const int colIdx_type = query.getColumnIndex("type");
    while (query.executeStep())
    {
        if (strcmp(query.getColumn(colIdx_name).getText(), columnName) == 0)
        {
            auto type = query.getColumn(colIdx_type).getString();
            std::uint32_t s;
            const bool b = CDbDiscover::TryParseBlobSize(type, &s);
            if (b == true)
            {
                return s;
            }
        }
    }

    stringstream ssExcp;
    ssExcp << "Error in retrieving the size of the BLOB in column \"" << columnName << "\" in table \"" << tableName << "\".";
    throw SqliteImgDocDbDiscoverException(ssExcp.str(), SqliteImgDocDbDiscoverException::ErrorType::UnsuitableDb);
}

bool CDbDiscover::TryParseBlobSize(const std::string& str, std::uint32_t* s)
{
    smatch sm;
    if (regex_match(str, sm, regex("BLOB\\([[:digit:]]+\\)")))
    {
        const auto& capture = sm[1].str();
        return  MiscUtils::TryParseUint32(capture, s);
    }
    
    return false;
}