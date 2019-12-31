#include "pch.h"
#include <cstdlib>
#include "../external/BlobOnHeap.h"

using namespace SlImgDoc;

/*virtual*/bool BlobOnHeap::Reserve(size_t s)
{
    if (this->ptr != nullptr)
    {
        return false;
    }

    this->ptr = malloc(s);
    if (this->ptr == nullptr)
    {
        return false;
    }

    this->size = s;
    return true;
}

/*virtual*/bool BlobOnHeap::GetPointer(size_t s, size_t offset, void*& ptr)
{
    if (this->ptr == nullptr || s + offset > this->size)
    {
        ptr = nullptr;
        return false;
    }

    ptr = ((char*)this->ptr) + offset;
    return true;
}

BlobOnHeap::~BlobOnHeap()
{
    if (this->ptr != nullptr)
    {
        free(this->ptr);
    }
}
