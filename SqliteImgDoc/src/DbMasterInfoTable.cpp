#include "pch.h"
#include <sstream>
#include "DbMasterInfoTable.h"

using namespace std;
using namespace SQLite;

/*static*/const std::string CDbMasterInfoTableHelper::TableName_MasterTable = "SqliteImgDoc_Master";
/*static*/const std::string CDbMasterInfoTableHelper::TableName_MasterTableColumnName_Key = "key";
/*static*/const std::string CDbMasterInfoTableHelper::TableName_MasterTableColumnName_ValueString = "value_string";
/*static*/const std::string CDbMasterInfoTableHelper::FieldName_Version = "Version";
/*static*/const std::string CDbMasterInfoTableHelper::FieldName_TableNameDocumentType = "DocType";
/*static*/const std::string CDbMasterInfoTableHelper::FieldName_TableNameDocumentTilesData = "DocTilesData";
/*static*/const std::string CDbMasterInfoTableHelper::FieldName_TableNameDocumentTilesInfo = "DocTilesInfo";
/*static*/const std::string CDbMasterInfoTableHelper::FieldName_TableNameDocumentTilesSpatialIndex = "DocSpatialIndex";
/*static*/const std::string CDbMasterInfoTableHelper::FieldName_TableNameDocumentPerTileData = "DocPerTileData";

/*static*/const std::string& CDbMasterInfoTableHelper::ToString(MasterInfo_DocumentType dt)
{
    static const string DT_Tiles2d = "Tiles2D";
    static const string DT_Tiles3d = "Tiles3D";

    switch (dt)
    {
    case MasterInfo_DocumentType::Tiles2D: return DT_Tiles2d;
    case MasterInfo_DocumentType::Tiles3D: return DT_Tiles3d;
    }

    throw invalid_argument("Unknown document-type");
}

/*static*/MasterInfo_DocumentType CDbMasterInfoTableHelper::MasterInfoDocumentTypeFromString(const std::string& str)
{
    if (str == CDbMasterInfoTableHelper::ToString(MasterInfo_DocumentType::Tiles2D))
    {
        return  MasterInfo_DocumentType::Tiles2D;
    }

    if (str == CDbMasterInfoTableHelper::ToString(MasterInfo_DocumentType::Tiles3D))
    {
        return  MasterInfo_DocumentType::Tiles3D;
    }

    throw invalid_argument("Unknown document-type");
}

/*static*/void CDbMasterInfoTableHelper::AddMasterTable(SQLite::Database* db, const std::string& version)
{
    stringstream ss;
    ss << "CREATE TABLE[" << CDbMasterInfoTableHelper::TableName_MasterTable << "](" <<
        "[" << CDbMasterInfoTableHelper::TableName_MasterTableColumnName_Key << "] TEXT(40) UNIQUE," <<
        "[" << CDbMasterInfoTableHelper::TableName_MasterTableColumnName_ValueString << "] TEXT);" <<
        "INSERT INTO [" << CDbMasterInfoTableHelper::TableName_MasterTable << "] ([" << CDbMasterInfoTableHelper::TableName_MasterTableColumnName_Key << "]," <<
        "[" << CDbMasterInfoTableHelper::TableName_MasterTableColumnName_ValueString << "]) VALUES('" << CDbMasterInfoTableHelper::FieldName_Version << "','" << version << "');";

    db->exec(ss.str());
}

/*static*/void CDbMasterInfoTableHelper::AddDocumentTiles2D(SQLite::Database* db, const std::function<const std::string(IDbDocInfo::TableType)>& funcGetTableName)
{
    CDbMasterInfoTableHelper::AddDocumentTiles2D(db, 0, funcGetTableName);
}

/*static*/void CDbMasterInfoTableHelper::AddDocumentTiles3D(SQLite::Database* db, const std::function<const std::string(IDbDocInfo3D::TableType)>& funcGetTableName)
{
    CDbMasterInfoTableHelper::AddDocumentTiles3D(db, 0, funcGetTableName);
}

/*static*/CDbMasterInfoTableHelper::DocumentInfo CDbMasterInfoTableHelper::GetDocumentInfo(SQLite::Database* db)
{
    DocumentInfo docInfo;
    CDbMasterInfoTableHelper::GetVersionAndDocumentInfo(db, nullptr, &docInfo.documentType);
    return docInfo;
}

/*static*/CDbMasterInfoTableHelper::DocumentInfoTile2D CDbMasterInfoTableHelper::GetDocumentInfoTile2D(SQLite::Database* db, int idx)
{
    const auto& tableNameDocumentTilesData = CDbMasterInfoTableHelper::GetFieldName_TableNameDocumentTilesDataNo(0);
    const auto& tableNameDocumentTilesInfo = CDbMasterInfoTableHelper::GetFieldName_TableNameDocumentTilesInfoNo(0);
    const auto& tableNameDocumentTilesSpatialIndex = CDbMasterInfoTableHelper::GetFieldName_TableNameDocumentTilesSpatialIndexNo(0);
    const auto& tableNameDocumentPerTileData = CDbMasterInfoTableHelper::GetFieldName_TableNameDocumentPerTileDataNo(0);
    stringstream ss;
    ss << "SELECT " << CDbMasterInfoTableHelper::TableName_MasterTableColumnName_Key << "," << CDbMasterInfoTableHelper::TableName_MasterTableColumnName_ValueString <<
        " FROM [" << CDbMasterInfoTableHelper::TableName_MasterTable << "] WHERE [" << CDbMasterInfoTableHelper::TableName_MasterTableColumnName_Key << "] IN(";
    ss << "'" << tableNameDocumentTilesData << "',";
    ss << "'" << tableNameDocumentTilesInfo << "',";
    ss << "'" << tableNameDocumentTilesSpatialIndex << "',";
    ss << "'" << tableNameDocumentPerTileData << "');";
    SQLite::Statement query(*db, ss.str());
    DocumentInfoTile2D docInfo2d;
    while (query.executeStep())
    {
        const auto& key = query.getColumn(0).getString();
        if (tableNameDocumentTilesData == key)
        {
            docInfo2d.tables[IDbDocInfo::TableType::TilesData] = query.getColumn(1).getString();
        }
        else if (tableNameDocumentTilesInfo == key)
        {
            docInfo2d.tables[IDbDocInfo::TableType::TilesInfo] = query.getColumn(1).getString();
        }
        else if (tableNameDocumentTilesSpatialIndex == key)
        {
            docInfo2d.tables[IDbDocInfo::TableType::TilesSpatialIndex] = query.getColumn(1).getString();
        }
        else if (tableNameDocumentPerTileData == key)
        {
            docInfo2d.tables[IDbDocInfo::TableType::CoordinateData] = query.getColumn(1).getString();
        }
    }

    return docInfo2d;
}

/*static*/CDbMasterInfoTableHelper::DocumentInfoTile3D CDbMasterInfoTableHelper::GetDocumentInfoTile3D(SQLite::Database* db, int idx)
{
    throw "not implemented";
}

/*static*/void CDbMasterInfoTableHelper::AddDocumentTiles2D(SQLite::Database* db, int no, const std::function<const std::string(IDbDocInfo::TableType)>& funcGetTableName)
{
    static const struct TableAndGetFieldName
    {
        IDbDocInfo::TableType tableType;
        string(*pfn)(int);
    } list[] =
    {
        { IDbDocInfo::TableType::TilesData, CDbMasterInfoTableHelper::GetFieldName_TableNameDocumentTilesDataNo },
        { IDbDocInfo::TableType::TilesInfo, CDbMasterInfoTableHelper::GetFieldName_TableNameDocumentTilesInfoNo },
        { IDbDocInfo::TableType::TilesSpatialIndex, CDbMasterInfoTableHelper::GetFieldName_TableNameDocumentTilesSpatialIndexNo },
        { IDbDocInfo::TableType::CoordinateData, CDbMasterInfoTableHelper::GetFieldName_TableNameDocumentPerTileDataNo }
    };

    CDbMasterInfoTableHelper::ExecuteInsertInto<TableAndGetFieldName>(db,no, list, sizeof(list) / sizeof(list[0]), [&](const TableAndGetFieldName& x)->string {return funcGetTableName(x.tableType); });
    /*stringstream ss;
    int valueNo = 1;

    ss << "INSERT INTO [" << CDbMasterInfoTableHelper::TableName_MasterTable << "] ([" << CDbMasterInfoTableHelper::TableName_MasterTableColumnName_Key << "]," << "[" << CDbMasterInfoTableHelper::TableName_MasterTableColumnName_ValueString << "]) VALUES";
    for (size_t i = 0; i < sizeof(list) / sizeof(list[0]); ++i)
    {
        ss << "(?" << valueNo << ",?" << valueNo + 1 << "),";
        valueNo += 2;
    }

    ss << "(?" << valueNo << ",?" << valueNo + 1 << ")";

    valueNo = 1;
    Statement statement(*db, ss.str());
    for (const auto& i : list)
    {
        statement.bind(valueNo++, i.pfn(no));
        statement.bind(valueNo++, funcGetTableName(i.tableType));
    }

    statement.bind(valueNo++, CDbMasterInfoTableHelper::GetFieldName_TableNameDocumentTypeNo(no));
    statement.bind(valueNo, CDbMasterInfoTableHelper::ToString(MasterInfo_DocumentType::Tiles2D));

    statement.exec();*/
}

/*static*/void CDbMasterInfoTableHelper::AddDocumentTiles3D(SQLite::Database* db, int no, const std::function<const std::string(IDbDocInfo3D::TableType)>& funcGetTableName)
{
    static const struct TableAndGetFieldName
    {
        IDbDocInfo3D::TableType tableType;
        string(*pfn)(int);
    } list[] =
    {
        { IDbDocInfo3D::TableType::TilesData, CDbMasterInfoTableHelper::GetFieldName_TableNameDocumentTilesDataNo },
        { IDbDocInfo3D::TableType::TilesInfo, CDbMasterInfoTableHelper::GetFieldName_TableNameDocumentTilesInfoNo },
        { IDbDocInfo3D::TableType::TilesSpatialIndex, CDbMasterInfoTableHelper::GetFieldName_TableNameDocumentTilesSpatialIndexNo },
        { IDbDocInfo3D::TableType::PerBrickData, CDbMasterInfoTableHelper::GetFieldName_TableNameDocumentPerTileDataNo }
    };

    CDbMasterInfoTableHelper::ExecuteInsertInto<TableAndGetFieldName>(db, no, list, sizeof(list) / sizeof(list[0]), [&](const TableAndGetFieldName& x)->string {return funcGetTableName(x.tableType); });
}

/*static*/std::string CDbMasterInfoTableHelper::GetFieldName_TableNameDocumentTypeNo(int no)
{
    stringstream ss;
    ss << CDbMasterInfoTableHelper::FieldName_TableNameDocumentType << no;
    return ss.str();
}

/*static*/std::string CDbMasterInfoTableHelper::GetFieldName_TableNameDocumentTilesDataNo(int no)
{
    stringstream ss;
    ss << CDbMasterInfoTableHelper::FieldName_TableNameDocumentTilesData << no;
    return ss.str();
}

/*static*/std::string CDbMasterInfoTableHelper::GetFieldName_TableNameDocumentTilesInfoNo(int no)
{
    stringstream ss;
    ss << CDbMasterInfoTableHelper::FieldName_TableNameDocumentTilesInfo << no;
    return ss.str();
}

/*static*/std::string CDbMasterInfoTableHelper::GetFieldName_TableNameDocumentTilesSpatialIndexNo(int no)
{
    stringstream ss;
    ss << CDbMasterInfoTableHelper::FieldName_TableNameDocumentTilesSpatialIndex << no;
    return ss.str();
}

/*static*/std::string CDbMasterInfoTableHelper::GetFieldName_TableNameDocumentPerTileDataNo(int no)
{
    stringstream ss;
    ss << CDbMasterInfoTableHelper::FieldName_TableNameDocumentPerTileData << no;
    return ss.str();
}

/*static*/void CDbMasterInfoTableHelper::GetVersionAndDocumentInfo(SQLite::Database* db, std::string* version, MasterInfo_DocumentType* docType)
{
    std::string docTypeString;
    CDbMasterInfoTableHelper::GetVersionAndDocumentInfo(db, version, &docTypeString);
    MasterInfo_DocumentType dt = CDbMasterInfoTableHelper::MasterInfoDocumentTypeFromString(docTypeString);
    if (docType != nullptr)
    {
        *docType = dt;
    }
}

/*static*/void CDbMasterInfoTableHelper::GetVersionAndDocumentInfo(SQLite::Database* db, std::string* version, std::string* docType)
{
    stringstream ss;
    ss << "SELECT " << CDbMasterInfoTableHelper::TableName_MasterTableColumnName_Key << "," << CDbMasterInfoTableHelper::TableName_MasterTableColumnName_ValueString <<
        " FROM [" << CDbMasterInfoTableHelper::TableName_MasterTable << "] WHERE [" << CDbMasterInfoTableHelper::TableName_MasterTableColumnName_Key << "] IN(" <<
        "'" << CDbMasterInfoTableHelper::FieldName_Version << "','" << CDbMasterInfoTableHelper::GetFieldName_TableNameDocumentTypeNo(0) << "');";
    SQLite::Statement query(*db, ss.str());
    bool hasVersion = false;
    bool hasDocType = false;
    while (query.executeStep())
    {
        const auto& key = query.getColumn(0).getString();
        if (key == CDbMasterInfoTableHelper::FieldName_Version)
        {
            if (version != nullptr)
            {
                *version = query.getColumn(1).getString();
            }

            hasVersion = true;
        }
        else if (key == CDbMasterInfoTableHelper::GetFieldName_TableNameDocumentTypeNo(0))
        {
            if (docType != nullptr)
            {
                *docType = query.getColumn(1).getString();
            }

            hasDocType = true;
        }
    }
}
