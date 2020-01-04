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

/*static*/void CDbMasterInfoTableHelper::AddMasterTable(SQLite::Database* db, const std::string& version)
{
    stringstream ss;
    ss << "CREATE TABLE[" << CDbMasterInfoTableHelper::TableName_MasterTable << "](";
    ss << "[" << CDbMasterInfoTableHelper::TableName_MasterTableColumnName_Key << "] TEXT(40) UNIQUE," <<
        "[" << CDbMasterInfoTableHelper::TableName_MasterTableColumnName_ValueString << "] TEXT);" <<
        "INSERT INTO [" << CDbMasterInfoTableHelper::TableName_MasterTable << "] ([" << CDbMasterInfoTableHelper::TableName_MasterTableColumnName_Key << "]," <<
        //"[" << CDbMasterInfoTableHelper::TableName_MasterTableColumnName_ValueString << "]) VALUES(?1,?2);";
        "[" << CDbMasterInfoTableHelper::TableName_MasterTableColumnName_ValueString << "]) VALUES('" << CDbMasterInfoTableHelper::FieldName_Version << "','" << version << "');";

    db->exec(ss.str());
    //Statement statement(*db, ss.str());
    //statement.bind(1, CDbMasterInfoTableHelper::FieldName_Version);
    //statement.bind(2, version);
    //statement.exec();
}

/*static*/void CDbMasterInfoTableHelper::AddDocumentTiles2D(SQLite::Database* db, const std::function<const std::string(IDbDocInfo::TableType)>& funcGetTableName)
{
    CDbMasterInfoTableHelper::AddDocumentTiles2D(db, 0, funcGetTableName);
}

/*static*/CDbMasterInfoTableHelper::DocumentInfo CDbMasterInfoTableHelper::GetDocumentInfo(SQLite::Database* db)
{
    throw "not implemented";
}

/*static*/CDbMasterInfoTableHelper::DocumentInfoTile2D CDbMasterInfoTableHelper::GetDocumentInfoTile2D(int idx)
{
    throw "not implemented";
}

/*static*/CDbMasterInfoTableHelper::DocumentInfoTile3D CDbMasterInfoTableHelper::GetDocumentInfoTile3D(int idx)
{
    throw "not implemented";
}

/*static*/void CDbMasterInfoTableHelper::AddDocumentTiles2D(SQLite::Database* db, int no, const std::function<const std::string(IDbDocInfo::TableType)>& funcGetTableName)
{
    static const struct
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

    stringstream ss;
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

    statement.exec();
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
