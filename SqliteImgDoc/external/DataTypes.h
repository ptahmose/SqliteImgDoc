#pragma once
#include <cstdint>

namespace SlImgDoc
{
    class DataTypes
    {
    public:
        static const std::uint8_t ZERO = 0;
        static const std::uint8_t UNCOMPRESSED_BITMAP = 1;

        static const std::uint8_t UNCOMPRESSED_BRICK = 32;
        static const std::uint8_t CUSTOM = 255;
    };
}