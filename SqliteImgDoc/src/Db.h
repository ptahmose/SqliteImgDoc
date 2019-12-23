#pragma once

#include <memory>
#include <utility>
#include <SQLiteCpp/Database.h> 
#include "../external/Interface.h"
#include "DbGlobalInfo.h"
#include "IInternalDb.h"

class CDb : public SlImgDoc::IDb, public IInternalDb, public std::enable_shared_from_this<CDb>
{
private:
    std::unique_ptr<SQLite::Database> db;
    std::shared_ptr<IDbDocInfo> docInfo;
    //std::shared_ptr< SlImgDoc::IDbWrite> writer;
    std::weak_ptr<SlImgDoc::IDbWrite> writer;
    //std::shared_ptr< SlImgDoc::IDbRead> reader;
    std::weak_ptr<SlImgDoc::IDbRead> reader;
public:
    CDb(SQLite::Database* db, std::shared_ptr<IDbDocInfo> docInfo) : db(db), docInfo(std::move(docInfo)) {}
    virtual ~CDb();

    virtual std::shared_ptr<SlImgDoc::IDbWrite> GetWriter();
    virtual std::shared_ptr<SlImgDoc::IDbRead> GetReader();
public:
    virtual SQLite::Database& GetDb() { return *this->db.get(); }
    const IDbDocInfo& GetDocInfo() { return *this->docInfo.get(); }
private:
};
