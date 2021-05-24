#pragma once

#include <string>
#include <SQLiteCpp/SQLiteCpp.h>
#include "../external/CoordinateData.h"
#include "DbGlobalInfo.h"

class DbUtils
{
public:
    static void Bind(SQLite::Statement& statement, int idx, const SlImgDoc::VariadicData& data);
    static bool TryParse(const std::string& str, ColumnTypeInfo* colTypeInfo);
    static bool TryParsesSqliteTableInfo(const std::string& str, ColumnTypeInfo* colTypeInfo);
    static std::string ColumnTypeInfoToStringRepresentation(const ColumnTypeInfo& cti);

    /// Enumerates all indices for the specified table. This is using the SQLite-specific command
    /// 'pragma index_list' (https://www.sqlite.org/pragma.html#pragma_index_list).
    /// \param [in,out] db          The database.
    /// \param          tablename   The table name.
    /// \param          enumIndices Functor which will be called for each index, passing in the name
    ///                             of the index. If returning false, the enumeration will stop.
    static void EnumIndicesForTable(SQLite::Database& db,const std::string& tablename, std::function<bool(const std::string& name)> enumIndices);


    static void DropIndexForTable(SQLite::Database& db, const std::string& indexname);

    static void CreateIndexForTable(SQLite::Database& db, const std::string& indexName, const std::string& tableName, const std::string& columnName);

    static bool DoesTableExists(SQLite::Database& db, const std::string& tableName);

    static void DropSpatialIndexTableIfExists(SQLite::Database& db, const std::string& tableName);
};
