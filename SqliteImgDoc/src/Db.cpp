#include "pch.h"
#include "Db.h"
#include "DbRead.h"
#include "DbWrite.h"

/*virtual*/ CDb::~CDb()
{
}

/*virtual*/std::shared_ptr<SlImgDoc::IDbWrite> CDb::GetWriter()
{
    auto w = this->writer.lock();
    if (w)
    {
        return w;
    }

    w = std::make_shared<CDbWrite>(this->shared_from_this());
    this->writer = w;
    return w;

    //return std::make_shared<CDbWrite>(this->getptr());

    /*if (!this->writer)
    {
        this->writer = std::make_shared<CDbWrite>(this->getptr());
    }

    return this->writer;*/
}

/*virtual*/std::shared_ptr<SlImgDoc::IDbRead> CDb::GetReader()
{
    auto r = this->reader.lock();
    if (r)
    {
        return r;
    }

    r = std::make_shared<CDbRead>(this->shared_from_this());
    this->reader = r;
    return r;

    //return std::make_shared<CDbRead>(this->getptr());
    
    /*
    if (!this->reader)
    {
        this->reader = std::make_shared<CDbRead>(this->getptr());
    }

    return this->reader;
    */
}
