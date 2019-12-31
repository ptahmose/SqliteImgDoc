#pragma once
#include "Interface.h"

#include <vector>
#include <array>
#include <cstring>

namespace SlImgDoc
{
    class BlobDataDynAlloc :public SlImgDoc::IBlobData
    {
    private:
        std::vector<std::uint8_t> data;
    public:
        virtual bool Set(const void* ptrData, size_t size)
        {
            this->data.resize(size);
            std::memcpy(&this->data[0], ptrData, size);
            return true;
        }

        std::vector<std::uint8_t>& Data() { return this->data; }
    };

    template <size_t s>
    class BlobDataPreAlloc :public SlImgDoc::IBlobData
    {
    private:
        size_t validSize;
        std::array<std::uint8_t, s> data;
    public:
        BlobDataPreAlloc() :validSize(0) {}

        virtual bool Set(const void* ptrData, size_t size)
        {
            if (size > s)
            {
                return false;
            }

            std::memcpy(&this->data[0], ptrData, size);
            this->validSize = size;
            return true;
        }

        std::array<std::uint8_t, s>& Data() { return this->data; }
        size_t ValidSize() const { return this->validSize; }
    };
}
