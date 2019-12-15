#pragma once

#include "ImportExport.h"
#include "IBlob.h"

namespace SlImgDoc
{
    class SQLITEIMGDOC_API BlobOnHeap : public SlImgDoc::IBlob
    {
    private:
        void* ptr;
        size_t size;
    public:
        BlobOnHeap() : ptr(nullptr), size(0) {}

        virtual bool Reserve(size_t s);
        virtual bool GetPointer(size_t s, size_t offset, void*& ptr);

        virtual ~BlobOnHeap();
    };
}