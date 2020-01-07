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
        static const std::string DataType_INVALID;
        static const std::string DataType_FLOAT;
        static const std::string DataType_INTEGER1;
        static const std::string DataType_INTEGER2;
        static const std::string DataType_INTEGER4;
        static const std::string DataType_INTEGER;
        static const std::string DataType_INTEGER8;

        VariadicData() : DataType(VariadicData::DataType_INVALID) {}
        VariadicData(double dbl) { this->doubleValue = dbl; this->DataType = VariadicData::DataType_FLOAT; }
        VariadicData(std::int8_t v) { this->i8Value = v; this->DataType = VariadicData::DataType_INTEGER1; }
        VariadicData(std::int16_t v) { this->i16Value = v; this->DataType = VariadicData::DataType_INTEGER2; }
        VariadicData(std::int32_t v) { this->i32Value = v; this->DataType = VariadicData::DataType_INTEGER4; }
        VariadicData(std::int64_t v) { this->i64Value = v; this->DataType = VariadicData::DataType_INTEGER8; }


        std::string DataType; // FLOAT, INTEGER(1),INTEGER(2),INTEGER(4),INTEGER(8)
        union
        {
            double  doubleValue;
            std::int8_t i8Value;
            std::int16_t i16Value;
            std::int32_t i32Value;
            std::int64_t i64Value;
        };
        std::string stringValue;

        bool IsFloat() const { return this->DataType == VariadicData::DataType_FLOAT; }
        bool IsInteger1() const { return this->DataType == VariadicData::DataType_INTEGER1; }
        bool IsInteger2() const { return this->DataType == VariadicData::DataType_INTEGER2; }
        bool IsInteger4() const { return this->DataType == VariadicData::DataType_INTEGER4; }
        bool IsInteger8() const { return this->DataType == VariadicData::DataType_INTEGER8; }
    };

    struct KeyVariadicValuePair
    {
        std::string Name;
        VariadicData Data;
    };

}
