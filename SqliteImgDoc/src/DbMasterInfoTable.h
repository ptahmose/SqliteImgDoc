#pragma once

#include <string>
#include <map>
#include <functional>
#include <SQLiteCpp/Database.h>
#include "DbGlobalInfo.h"
#include "DbGlobalInfo3D.h"

enum class MasterInfo_DocumentType
{
    Tiles2D,
    Tiles3D
};

class CDbMasterInfoTableHelper
{
private:
    static const std::string TableName_MasterTable;
    static const std::string TableName_MasterTableColumnName_Key;
    static const std::string TableName_MasterTableColumnName_ValueString;
    static const std::string FieldName_Version;
    static const std::string FieldName_TableNameDocumentType;
    static const std::string FieldName_TableNameDocumentTilesData;
    static const std::string FieldName_TableNameDocumentTilesInfo;
    static const std::string FieldName_TableNameDocumentTilesSpatialIndex;
    static const std::string FieldName_TableNameDocumentPerTileData;
public:
    struct DocumentInfo
    {
        MasterInfo_DocumentType documentType;
    };

    struct DocumentInfoTile2D
    {
        std::map<IDbDocInfo::TableType, std::string> tables;
    };

    struct DocumentInfoTile3D
    {
        std::map<IDbDocInfo3D::TableType, std::string> tables;
    };

    static const std::string& ToString(MasterInfo_DocumentType dt);
    static MasterInfo_DocumentType MasterInfoDocumentTypeFromString(const std::string& str);
public:
    static void AddMasterTable(SQLite::Database* db, const std::string& version);
    static void AddDocumentTiles2D(SQLite::Database* db, const std::function<const std::string(IDbDocInfo::TableType)>& funcGetTableName);
    static void AddDocumentTiles3D(std::function<const std::string(IDbDocInfo3D::TableType)> funcGetTableName);

    static DocumentInfo GetDocumentInfo(SQLite::Database* db);

    static DocumentInfoTile2D GetDocumentInfoTile2D(SQLite::Database* db,int idx);
    static DocumentInfoTile3D GetDocumentInfoTile3D(SQLite::Database* db, int idx);
private:
    static void AddDocumentTiles2D(SQLite::Database* db, int no, const std::function<const std::string(IDbDocInfo::TableType)>& funcGetTableName);

    static std::string GetFieldName_TableNameDocumentTypeNo(int no);
    static std::string GetFieldName_TableNameDocumentTilesDataNo(int no);
    static std::string GetFieldName_TableNameDocumentTilesInfoNo(int no);
    static std::string GetFieldName_TableNameDocumentTilesSpatialIndexNo(int no);
    static std::string GetFieldName_TableNameDocumentPerTileDataNo(int no);

    static void GetVersionAndDocumentInfo(SQLite::Database* db, std::string* version, MasterInfo_DocumentType* docType);
    static void GetVersionAndDocumentInfo(SQLite::Database* db, std::string* version, std::string* docType);
};
