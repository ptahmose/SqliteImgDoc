#include "pch.h"
#include "miscutilities.h"
#include <cstdlib>
#include <cerrno>
#include <limits>

using namespace std;
using namespace SlImgDoc;

/*static*/std::string MiscUtils::empty_string = string("");

/*static*/bool MiscUtils::TryParseUint64(const char* sz, std::uint64_t* val)
{
    char* end;
    errno = 0;
    std::uint64_t v = std::strtoull(sz, &end, 10);
    if (sz[0] == '\0' || *end != '\0')
    {
        return false;
    }

    if (v == numeric_limits<uint64_t>::max())
    {
        if (errno == ERANGE)
        {
            return false;
        }
    }

    if (val != nullptr)
    {
        *val = v;
    }

    return true;
}

/*static*/bool MiscUtils::TryParseUint32(const char* sz, std::uint32_t* val)
{
    // note: no idea what wrong, but on Linux std::strtoul behaves strange because if does not detect overflows, e.g. 
    // for "4294967296" I get 0 and errno is NOT set to ERANGE
    uint64_t val64;
    if (!MiscUtils::TryParseUint64(sz, &val64))
    {
        return false;
    }

    if (val64 <= numeric_limits<uint32_t>::max())
    {
        if (val != nullptr)
        {
            *val = static_cast<uint32_t>(val64);
        }

        return true;
    }

    return false;
}

/*static*/SlImgDoc::KeyVariadicValuePair MiscUtils::ReadValue(SQLite::Statement& statement, int colIdx, const ColumnTypeAllInfo& colInfo)
{
    KeyVariadicValuePair kv;
    switch (colInfo.type)
    {
    case ColumnType::Integer:
        switch (colInfo.size)
        {
        case 1:
            kv.Data.ui8Value = (std::uint8_t)statement.getColumn(colIdx).getUInt();
            kv.Data.DataType = "INTEGER(1)";
            break;
        case 2:
            kv.Data.ui16Value = (std::uint16_t)statement.getColumn(colIdx).getUInt();
            kv.Data.DataType = "INTEGER(2)";
            break;
        case 4:
            kv.Data.ui32Value = statement.getColumn(colIdx).getUInt();
            kv.Data.DataType = "INTEGER(4)";
            break;
        case 8:
            kv.Data.ui16Value = (std::uint64_t)statement.getColumn(colIdx).getUInt();
            kv.Data.DataType = "INTEGER(8)";
            break;
        }

        break;
    case ColumnType::Float:
        kv.Data.doubleValue = statement.getColumn(colIdx).getDouble();
        kv.Data.DataType = "Float";
        kv.Name = colInfo.columnName;
        break;
    }

    return kv;
}
