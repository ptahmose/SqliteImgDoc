#include "pch.h"
#include "dbutils.h"

using namespace SlImgDoc;

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