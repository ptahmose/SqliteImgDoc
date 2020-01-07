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
    static std::string ColumnTypeInfoToStringRepresentation(const ColumnTypeInfo& cti);
};