#pragma once
#include "IDataObj.h"
#include <cstdlib>

namespace SlImgDoc
{
    class CDataObjCustom : public IDataObjCustom
    {
    private:
        size_t sizeHdr;
        size_t sizeData;
        void* ptrHdr;
        void* ptrData;
    public:
        CDataObjCustom(size_t sizeHdr, size_t sizeData) :sizeHdr(sizeHdr), sizeData(sizeData)
        {
            this->ptrHdr = malloc(sizeHdr);
            this->ptrData = malloc(sizeData);
        }

        virtual ~CDataObjCustom()
        {
            free(this->ptrHdr);
            free(this->ptrData);
        }

        virtual void GetData(const void** p, size_t* s) const
        {
            if (p != nullptr) { *p = this->ptrData; }
            if (s != nullptr) { *s = this->sizeData; }
        }

        virtual void GetHeader(const void** p, size_t* s) const
        {
            if (p != nullptr) { *p = this->ptrHdr; }
            if (s != nullptr) { *s = this->sizeHdr; }
        }
    };


}
