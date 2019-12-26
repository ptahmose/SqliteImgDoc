#pragma once

#include <memory>
#include "../external/Interface.h"
#include "Db.h"
#include <SQLiteCpp/Database.h>

class CDbBase
{
private:
    std::shared_ptr<CDb> db;
public:
    CDbBase(std::shared_ptr<CDb> db):db(db){}

    //CDbBase(SQLite::Database* db) : db(db)
    //{
    //}

    virtual ~CDbBase()
    {}

protected:
    SQLite::Database& GetDb() { return /* *this->db.get();*/this->db->GetDb(); }
    const IDbDocInfo& GetDocInfo() { return this->db->GetDocInfo(); }
    const IDbDocInfo3D& GetDocInfo3D() { return this->db->GetDocInfo3D(); }

public:
    /// The name of the table with the "technical document info"
    static const char* TableName_DocumentInfo;

    /// The name of the table with the "tile list"
    static const char* TableName_TileTable;

    /// The name of the table with the "tile data"
    static const char* TableName_TileData;

    /// The name of the table with the "spatial index"
    static const char* VTableName_SpatialTable;

    static const char* DocumentInfoColName_Version;
    static const char* DocumentInfoColName_CoordinateDimensions;
    static const char* DocumentInfoColName_IndexedDimensions;
};

