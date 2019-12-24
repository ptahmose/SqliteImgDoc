#pragma once

#include "DataBinHdr.h"

class IDataObjBase
{
public:
    virtual void GetData(const void** p, size_t* s) const = 0;
};

class IDataObjUncompressedBitmap : public IDataObjBase
{
public:
    virtual const BinHdrUncompressedBitmap& GetBinHdr() const = 0;
};

class IDataObjCustom : public IDataObjBase
{
public:
    virtual void GetHeader(const void** p, size_t* s) const = 0;
};

class IDataObjZero : public IDataObjBase
{
public:
    virtual void GetHeader(const void** p, size_t* s) const = 0;
};

//-----------------------------------------------------------------------------

class IDataObjUncompressedBrick : public IDataObjBase
{
public:
    virtual const BinHdrUncompressedBrick& GetBinHdr() const = 0;
};
