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

    virtual ~CDbBase()
    {}

protected:
    SQLite::Database& GetDb() { return /* *this->db.get();*/this->db->GetDb(); }
    const IDbDocInfo& GetDocInfo() const { return this->db->GetDocInfo(); }
    const IDbDocInfo3D& GetDocInfo3D() const { return this->db->GetDocInfo3D(); }

    void CheckSizeOfBinHdrAndThrow(size_t s, std::uint32_t maxSize);
    void CheckIfAllDimensionGivenAndThrow(const std::vector<SlImgDoc::TileDim>&, const SlImgDoc::ITileCoordinate* coord);

    bool IsSpatialIndexActive() const { return this->db->IsSpatialIndexActive(); }
    void UpdateSpatialIndexAvailability() { this->db->UpdateSpatialIndexActive(); }
public:
 /*   /// The name of the table with the "technical document info"
    static const char* TableName_DocumentInfo;

    /// The name of the table with the "tile list"
    static const char* TableName_TileTable;

    /// The name of the table with the "tile data"
    static const char* TableName_TileData;

    /// The name of the table with the "spatial index"
    static const char* VTableName_SpatialTable;*/

    //static const char* DocumentInfoColName_Version;
    //static const char* DocumentInfoColName_CoordinateDimensions;
    //static const char* DocumentInfoColName_IndexedDimensions;
};

