#pragma once

#include <cstdint>

#if defined(_MSC_VER) || defined(__GNUC__)
#pragma pack(push,1)
#else
#error "Need to define struct-alignment for this compiler"
#endif

class PixelType
{
public:
    static const std::uint8_t GRAY8 = 1;
    static const std::uint8_t BGR24 = 2;
    static const std::uint8_t GRAY16 = 3;
    static const std::uint8_t BGR48 = 4;
    static const std::uint8_t GRAYFLT32 = 5;
};

struct BinHdrBase
{
    //std::uint16_t type;
};

struct BinHdrBitmapBase : BinHdrBase
{
    std::uint32_t width;
    std::uint32_t height;
    //std::uint8_t pixeltype;
};

struct BinHdrUncompressedBitmap : BinHdrBitmapBase
{
    std::uint32_t stride;
};

struct BinHdrBrickBase : BinHdrBase
{
    std::uint32_t width;
    std::uint32_t height;
    std::uint32_t depth;
    std::uint8_t pixeltype;
};

struct BinHdrUncompressedBrick : BinHdrBitmapBase
{
    std::uint32_t depth;
    std::uint32_t strideX;
    std::uint32_t strideXY;
};

#if defined(_MSC_VER) || defined(__GNUC__)
#pragma pack(pop)
#endif
//#include <poppack.h>
