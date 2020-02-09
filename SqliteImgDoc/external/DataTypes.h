#pragma once
#include <cstdint>

namespace SlImgDoc
{
    enum class DataTypes : std::uint8_t
    {
        /// The zero data-type. This tile is to be interpreted as "all pixels are zero", irrespective of the
        /// content of the data-blob (it may not even be present).
        ZERO = 0,

        /// The uncompressed bitmap
        UNCOMPRESSED_BITMAP = 1,

        JPGXRCOMPRESSED_BITMAP = 2,

        UNCOMPRESSED_BRICK = 32,
        CUSTOM = 255
    };
}