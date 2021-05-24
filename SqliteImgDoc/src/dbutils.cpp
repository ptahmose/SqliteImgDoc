#include "pch.h"
#include <sstream>
#include "dbutils.h"
#include <iostream>

using namespace SlImgDoc;
using namespace std;

/*static*/void DbUtils::Bind(SQLite::Statement& statement, int idx, const SlImgDoc::VariadicData& data)
{
    ColumnTypeInfo cti;
    bool b = DbUtils::TryParse(data.DataType, &cti);
    switch (cti.type)
    {
    case ColumnType::Integer:
        switch (cti.size)
        {
        case 1:
            statement.bind(idx, data.i8Value);
            break;
        case 2:
            statement.bind(idx, data.i16Value);
            break;
        case 4:
            statement.bind(idx, data.i32Value);
            break;
        case 8:
            statement.bind(idx, (long long)data.i64Value);
            break;
        }

        break;
    case ColumnType::Float:
        statement.bind(idx, data.doubleValue);
        break;
    }
}

/*static*/bool DbUtils::TryParse(const std::string& str, ColumnTypeInfo* colTypeInfo)
{
    ColumnTypeInfo ti;
    if (str == VariadicData::DataType_FLOAT)
    {
        ti.type = ColumnType::Float;
        ti.size = 0;
    }
    else if (str == VariadicData::DataType_INTEGER || str == VariadicData::DataType_INTEGER4)
    {
        ti.type = ColumnType::Integer;
        ti.size = 4;
    }
    else if (str == VariadicData::DataType_INTEGER1)
    {
        ti.type = ColumnType::Integer;
        ti.size = 1;
    }
    else if (str == VariadicData::DataType_INTEGER2)
    {
        ti.type = ColumnType::Integer;
        ti.size = 2;
    }
    else if (str == VariadicData::DataType_INTEGER8)
    {
        ti.type = ColumnType::Integer;
        ti.size = 8;
    }
    else
    {
        return false;
    }

    if (colTypeInfo != nullptr)
    {
        *colTypeInfo = ti;
    }

    return true;
}

/*static*/bool DbUtils::TryParsesSqliteTableInfo(const std::string& str, ColumnTypeInfo* colTypeInfo)
{
    ColumnTypeInfo ti;
    if (str == VariadicData::DataType_FLOAT || str == "DOUBLE")
    {
        ti.type = ColumnType::Float;
        ti.size = 0;
    }
    else if (str == VariadicData::DataType_INTEGER || str == VariadicData::DataType_INTEGER4)
    {
        ti.type = ColumnType::Integer;
        ti.size = 4;
    }
    else if (str == VariadicData::DataType_INTEGER1)
    {
        ti.type = ColumnType::Integer;
        ti.size = 1;
    }
    else if (str == VariadicData::DataType_INTEGER2)
    {
        ti.type = ColumnType::Integer;
        ti.size = 2;
    }
    else if (str == VariadicData::DataType_INTEGER8)
    {
        ti.type = ColumnType::Integer;
        ti.size = 8;
    }
    else
    {
        return false;
    }

    if (colTypeInfo != nullptr)
    {
        *colTypeInfo = ti;
    }

    return true;
}

/*static*/std::string DbUtils::ColumnTypeInfoToStringRepresentation(const ColumnTypeInfo& cti)
{
    switch (cti.type)
    {
    case ColumnType::Float:
        return VariadicData::DataType_FLOAT;
    case ColumnType::Integer:
        switch (cti.size)
        {
        case 1:
            return VariadicData::DataType_INTEGER1;
        case 2:
            return VariadicData::DataType_INTEGER2;
        case 4:
            return VariadicData::DataType_INTEGER4;
        case 8:
            return VariadicData::DataType_INTEGER8;
        }

        break;
    }

    throw "unknown";
}

/*static*/void DbUtils::EnumIndicesForTable(SQLite::Database& db, const std::string& tablename, std::function<bool(const std::string& name)> enumIndices)
{
    stringstream sql;
    sql << "PRAGMA index_list('" << tablename << "')";
    try
    {
        SQLite::Statement query(db, sql.str());

        while (query.executeStep())
        {
            int idxOfNameColumn = query.getColumnIndex("name");

            auto name = string(query.getColumn(idxOfNameColumn).getText());
            if (!enumIndices(name))
            {
                break;
            }
        }
    }
    catch (SQLite::Exception& excp)
    {
        //std::cout << excp.what();
        throw;
    }
}

/*static*/void DbUtils::DropIndexForTable(SQLite::Database& db, const std::string& tablename)
{
    stringstream sql;
    sql << "DROP INDEX IF EXISTS '" << tablename << "'";
    try
    {
        SQLite::Statement query(db, sql.str());

        while (query.executeStep())
        {

        }

    }
    catch (SQLite::Exception& excp)
    {
        std::cout << excp.what();
        throw;
    }
}

/*static*/void DbUtils::CreateIndexForTable(SQLite::Database& db, const std::string& indexName, const std::string& tableName, const std::string& columnName)
{
    stringstream sql;
    sql << "CREATE INDEX  IF NOT EXISTS '" << indexName << "' ON '" << tableName << "' (" << columnName << ");";
    try
    {
        SQLite::Statement query(db, sql.str());

        while (query.executeStep())
        {

        }

    }
    catch (SQLite::Exception& excp)
    {
        std::cout << excp.what();
        throw;
    }
}

/*static*/bool DbUtils::DoesTableExists(SQLite::Database& db, const std::string& tableName)
{
    stringstream sql;
    sql << "SELECT count(*) FROM sqlite_master WHERE type = 'table' AND name = '" << tableName << "'";

    SQLite::Statement query(db, sql.str());

    query.executeStep();

    int r = query.getColumn(0).getInt();

    return r != 0 ? true : false;
}

/*static*/void DbUtils::DropSpatialIndexTableIfExists(SQLite::Database& db, const std::string& tableName)
{
    stringstream sql;
    sql << "DROP TABLE IF EXISTS '" << tableName << "'";

    SQLite::Statement query(db, sql.str());

    int r = query.exec();
}