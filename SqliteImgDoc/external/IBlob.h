#pragma once

#include "ImportExport.h"
#include <cstdlib>

namespace SlImgDoc
{
    class SQLITEIMGDOC_API IBlob
    {
    public:
        virtual bool Reserve(size_t s)=0;
        virtual bool GetPointer(size_t s, size_t offset, void*& ptr)=0;

        virtual ~IBlob() {};
    };
}