#pragma once

#include <cstdint>
#include <string>
#include <sstream>
#include <SQLiteCpp/SQLiteCpp.h>
#include "DbGlobalInfo.h"

class MiscUtils
{
public:
    static std::string empty_string;
    static bool TryParseUint64(const char* sz, std::uint64_t* val);
    static bool TryParseUint32(const char* sz, std::uint32_t* val);
    static bool TryParseUint64(const std::string& str, std::uint64_t* val) { return MiscUtils::TryParseUint64(str.c_str(), val); }
    static bool TryParseUint32(const std::string& str, std::uint32_t* val) { return MiscUtils::TryParseUint32(str.c_str(), val); }

    template<typename ForwardIterator>
    static void AddCommaSeparatedAndEnclose(std::stringstream& stream, ForwardIterator begin, ForwardIterator end, const std::string& encloseLeft, const std::string& encloseRight)
    {
        bool isFirst = true;
        for (; begin != end; ++begin)
        {
            if (!isFirst)
            {
                stream << ',';
            }

            stream << encloseLeft << *begin << encloseRight;
            isFirst = false;
        }
    }

    static SlImgDoc::KeyVariadicValuePair ReadValue(SQLite::Statement& statement, int colIdx,const ColumnTypeAllInfo& colInfo);
};
