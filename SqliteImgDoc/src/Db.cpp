#include "pch.h"
#include "Db.h"
#include "DbRead.h"
#include "DbWrite.h"

using namespace std;

/*virtual*/ CDb::~CDb()
{
}

/*virtual*/std::shared_ptr<SlImgDoc::IDbWrite> CDb::GetWriter()
{
    if (!this->Is2D())
    {
        return shared_ptr<SlImgDoc::IDbWrite>();
    }

    auto w = this->writer.lock();
    if (w)
    {
        return w;
    }

    w = std::make_shared<CDbWrite>(this->shared_from_this());
    this->writer = w;
    return w;
}

/*virtual*/std::shared_ptr<SlImgDoc::IDbRead> CDb::GetReader()
{
    if (!this->Is2D())
    {
        return shared_ptr<SlImgDoc::IDbRead>();
    }

    auto r = this->reader.lock();
    if (r)
    {
        return r;
    }

    r = std::make_shared<CDbRead>(this->shared_from_this());
    this->reader = r;
    return r;
}
