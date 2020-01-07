#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <cstdint>

namespace SlImgDoc
{
    struct ColumnDescription
    {
        std::string Name;
        std::string DataType;
    };

    class PerTileDataCreateOptions
    {
    public:
        //struct Description
        //{
        //    std::string Name;
        //    std::string DataType;
        //};

        std::vector<ColumnDescription> descriptions;
    };

    /*class DimensionDataCreateOptions
    {
    public:
        struct Description
        {
            std::string Name;
            std::string DataType;
            bool incAndOffset;
        };
    };*/

    struct VariadicData
    {
        std::string DataType; // FLOAT, INTEGER(1),INTEGER(2),INTEGER(4),INTEGER(8)
        union
        {
            double  doubleValue;
            std::uint8_t ui8Value;
            std::uint16_t ui16Value;
            std::uint32_t ui32Value;
            std::uint64_t ui64Value;
        };
        std::string stringValue;
    };

    struct KeyVariadicValuePair
    {
        std::string Name;
        VariadicData Data;
    };

}
