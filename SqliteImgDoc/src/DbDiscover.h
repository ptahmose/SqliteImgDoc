#pragma once
#include <string>
#include <memory>
#include <SQLiteCpp/Database.h>
#include "DbGlobalInfo.h"
#include "DbGlobalInfo3D.h"

class CDbDiscover
{
    SQLite::Database* db;
    bool discoveryDone;
    std::shared_ptr<IDbDocInfo> docInfo;
    std::shared_ptr<IDbDocInfo3D> docInfo3d;
public:
    CDbDiscover(SQLite::Database* db) : db(db), discoveryDone(false) {}

    void DoIt();

    std::shared_ptr<IDbDocInfo> GetDocInfo();
    std::shared_ptr<IDbDocInfo3D> GetDocInfo3D();
private:
    void DoTiles2DDiscovery();
    void DoTiles3DDiscovery();

    std::vector<std::string> GetColumnNamesStartingWith(const char* tableName, const char* startsWith);
    std::vector<SlImgDoc::TileDim> GetTileDims(std::vector<std::string> colNames);
    std::uint32_t GetSchemaSizeOfColumn(const char* tableName, const char* columnName);

    std::vector<std::string> GetColumnNamesStartingWith(const std::string& tableName, const std::string& startsWith)
    {
        return this->GetColumnNamesStartingWith(tableName.c_str(), startsWith.c_str());
    }

    std::uint32_t GetSchemaSizeOfColumn(const std::string& tableName, const std::string& columnName)
    {
        return this->GetSchemaSizeOfColumn(tableName.c_str(), columnName.c_str());
    }

    bool TryParseBlobSize(const std::string& str, std::uint32_t* s);

    void ThrowIfDiscoveryWasNotDone();

    void GetPerTilesDataColumnsInfo(CDbDocInfo& docInfo);
    void GetPerTilesDataColumnsInfo(CDbDocInfo3D& docInfo);
    std::vector<ColumnTypeAllInfo> GetPerTilesDataColumnsInfo(const std::string& tableName, const std::string& pkColName);
};
