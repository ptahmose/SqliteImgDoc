#pragma once

#include <memory>
#include <utility>
#include <SQLiteCpp/Database.h> 
#include "../external/Interface.h"
#include "DbGlobalInfo.h"
#include "IInternalDb.h"
#include "DbGlobalInfo3D.h"

class CDb : public SlImgDoc::IDb, public IInternalDb, public std::enable_shared_from_this<CDb>
{
private:
    bool spatialIndexActive;

    std::unique_ptr<SQLite::Database> db;
    std::shared_ptr<IDbDocInfo> docInfo;

    std::shared_ptr<IDbDocInfo3D> docInfo3D;


    std::weak_ptr<SlImgDoc::IDbWrite> writer;
    std::weak_ptr<SlImgDoc::IDbRead> reader;

    std::weak_ptr<SlImgDoc::IDbWrite3D> writer3D;
    std::weak_ptr<SlImgDoc::IDbRead3D> reader3D;
public:
    CDb(SQLite::Database* db, std::shared_ptr<IDbDocInfo> docInfo) : db(db), docInfo(std::move(docInfo)) {}
    CDb(SQLite::Database* db, std::shared_ptr<IDbDocInfo3D> docInfo3D) : db(db), docInfo3D(std::move(docInfo3D)) {}
    virtual ~CDb();

    virtual std::shared_ptr<SlImgDoc::IDbWrite> GetWriter();
    virtual std::shared_ptr<SlImgDoc::IDbRead> GetReader();
    virtual std::shared_ptr<SlImgDoc::IDbWrite3D> GetWriter3D();
    virtual std::shared_ptr<SlImgDoc::IDbRead3D> GetReader3D();

    bool IsSpatialIndexActive() const;

    void UpdateSpatialIndexActive();
public:
    virtual SQLite::Database& GetDb() { return *this->db.get(); }
    const IDbDocInfo& GetDocInfo() { return *this->docInfo.get(); }
    const IDbDocInfo3D& GetDocInfo3D() { return *this->docInfo3D.get(); }
private:
    bool Is2D() { return !!this->docInfo; }
    bool Is3D() { return !!this->docInfo3D; }
};
