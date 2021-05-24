#include "pch.h"
#include "Db.h"
#include "DbRead.h"
#include "DbWrite.h"
#include "DbWrite3D.h"
#include "DbRead3D.h"

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

    this->UpdateSpatialIndexActive();

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

    this->UpdateSpatialIndexActive();

    auto r = this->reader.lock();
    if (r)
    {
        return r;
    }

    r = std::make_shared<CDbRead>(this->shared_from_this());
    this->reader = r;
    return r;
}

/*virtual*/std::shared_ptr<SlImgDoc::IDbWrite3D> CDb::GetWriter3D()
{
    if (!this->Is3D())
    {
        return shared_ptr<SlImgDoc::IDbWrite3D>();
    }

    this->UpdateSpatialIndexActive();

    auto w = this->writer3D.lock();
    if (w)
    {
        return w;
    }

    w = std::make_shared<CDbWrite3D>(this->shared_from_this());
    this->writer3D = w;
    return w;
}

/*virtual*/std::shared_ptr<SlImgDoc::IDbRead3D> CDb::GetReader3D()
{
    if (!this->Is3D())
    {
        return shared_ptr<SlImgDoc::IDbRead3D>();
    }

    this->UpdateSpatialIndexActive();

    auto r = this->reader3D.lock();
    if (r)
    {
        return r;
    }

    r = std::make_shared<CDbRead3D>(this->shared_from_this());
    this->reader3D = r;
    return r;
}

bool CDb::IsSpatialIndexActive() const
{
    return this->spatialIndexActive;
}

void CDb::UpdateSpatialIndexActive()
{
    bool doesSpatialIndexExist=false;
    if (this->Is2D())
    {
        doesSpatialIndexExist = DbUtils::DoesTableExists(
            this->GetDb(),
            this->GetDocInfo().GetTableName(IDbDocInfo::TableType::TilesSpatialIndex));
    }
    else if (this->Is3D())
    {
        doesSpatialIndexExist = DbUtils::DoesTableExists(
            this->GetDb(),
            this->GetDocInfo3D().GetTableName(IDbDocInfo3D::TableType::TilesSpatialIndex));
    }

    this->spatialIndexActive = doesSpatialIndexExist;
}