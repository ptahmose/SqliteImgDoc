#pragma once

#include <cstdint>

#include <pshpack1.h>

class PixelType
{
public:
    static const std::uint8_t GRAY8 = 1;
    static const std::uint8_t RGB24 = 2;
    static const std::uint8_t GRAY16 = 3;
    static const std::uint8_t RGB48 = 4;
    static const std::uint8_t GRAYFLT32 = 5;
};

struct BinHdrBase
{
    std::uint8_t type;
};

struct BinHdrUncompressedBitmap : BinHdrBase
{
    std::uint32_t width;
    std::uint32_t height;
    std::uint32_t stride;
    std::uint8_t pixeltype;
};

#include <poppack.h>
