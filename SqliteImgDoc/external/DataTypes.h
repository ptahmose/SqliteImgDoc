#pragma once
#include <cstdint>

namespace SlImgDoc
{
    class DataTypes
    {
    public:
        /// The zero data-type. This tile is to be interpreted as "all pixels are zero", irrespective of the
        /// content of the data-blob (it may not even be present).
        static const std::uint8_t ZERO = 0;

        /// The uncompressed bitmap
        static const std::uint8_t UNCOMPRESSED_BITMAP = 1;

        static const std::uint8_t UNCOMPRESSED_BRICK = 32;
        static const std::uint8_t CUSTOM = 255;
    };
}