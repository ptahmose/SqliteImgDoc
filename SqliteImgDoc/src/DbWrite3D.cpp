#include "pch.h"
#include "DbWrite3D.h"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>

#include <iostream>
#include <sstream>
#include <memory>

using namespace std;

/*virtual*/CDbWrite3D::~CDbWrite3D()
{
}

/*virtual*/void CDbWrite3D::AddBrick(const SlImgDoc::ITileCoordinate* coord, const SlImgDoc::LogicalPositionInfo3D* info, const IDataObjUncompressedBrick* data)
{
    // TODO: check if coord contains all dimensions (as required by this->docInfo->GetTileDimensions())
    try
    {
        auto idSbBlk = this->AddBrickUncompressed(data);

        const auto dims = this->GetDocInfo().GetTileDimensions();

        this->EnsureAddTilesInfoRowStatement();
        this->addTilesInfoRowStatement->reset();

        for (int i = 1; i <= dims.size(); ++i)
        {
            int v;
            coord->TryGetCoordinate(dims[i - 1], &v);
            this->addTilesInfoRowStatement->bind(i, v);
        }

        int i = dims.size() + 1;
        this->addTilesInfoRowStatement->bind(i++, info->posX);
        this->addTilesInfoRowStatement->bind(i++, info->posY);
        this->addTilesInfoRowStatement->bind(i++, info->posZ);
        this->addTilesInfoRowStatement->bind(i++, info->width);
        this->addTilesInfoRowStatement->bind(i++, info->height);
        this->addTilesInfoRowStatement->bind(i++, info->depth);
        this->addTilesInfoRowStatement->bind(i++, info->pyrLvl);
        this->addTilesInfoRowStatement->bind(i++, idSbBlk);

        this->addTilesInfoRowStatement->exec();

        auto id = this->GetDb().getLastInsertRowid();
        this->AddToSpatialIndexTable(id, info);
    }
    catch (SQLite::Exception & excp)
    {
        std::cout << excp.what();
    }
}

/*virtual*/void CDbWrite3D::AddBrick(const SlImgDoc::ITileCoordinate* coord, const SlImgDoc::LogicalPositionInfo3D* info, const SlImgDoc::TileBaseInfo3D* tileInfo, const IDataObjCustom* data)
{
    // TODO: check if coord contains all dimensions (as required by this->docInfo->GetTileDimensions())
    try
    {
        const void* ptrHdrData; size_t sizeHdrData;
        data->GetHeader(&ptrHdrData, &sizeHdrData);
        auto idSbBlk = this->AddBrickData(tileInfo->pixelWidth, tileInfo->pixelHeight, tileInfo->pixelDepth ,tileInfo->pixelType, SlImgDoc::DataTypes::CUSTOM, sizeHdrData, ptrHdrData, data);

        const auto dims = this->GetDocInfo().GetTileDimensions();

        this->EnsureAddTilesInfoRowStatement();
        this->addTilesInfoRowStatement->reset();

        for (int i = 1; i <= dims.size(); ++i)
        {
            int v;
            coord->TryGetCoordinate(dims[i - 1], &v);
            this->addTilesInfoRowStatement->bind(i, v);
        }

        int i = dims.size() + 1;
        this->addTilesInfoRowStatement->bind(i++, info->posX);
        this->addTilesInfoRowStatement->bind(i++, info->posY);
        this->addTilesInfoRowStatement->bind(i++, info->posZ);
        this->addTilesInfoRowStatement->bind(i++, info->width);
        this->addTilesInfoRowStatement->bind(i++, info->height);
        this->addTilesInfoRowStatement->bind(i++, info->depth);
        this->addTilesInfoRowStatement->bind(i++, info->pyrLvl);
        this->addTilesInfoRowStatement->bind(i++, idSbBlk);

        this->addTilesInfoRowStatement->exec();

        auto id = this->GetDb().getLastInsertRowid();
        this->AddToSpatialIndexTable(id, info);
    }
    catch (SQLite::Exception & excp)
    {
        std::cout << excp.what();
    }
}

std::int64_t CDbWrite3D::AddBrickUncompressed(const IDataObjUncompressedBrick* data)
{
    try
    {
        this->EnsureAddTilesDataRowStatement();
        this->addTilesDataRowStatement->reset();

        const auto hdr = data->GetBinHdr();

        std::uint8_t binhdr[32];
        for (int i = 0; i < sizeof(binhdr); ++i) { binhdr[i] = (uint8_t)i; }

        this->addTilesDataRowStatement->bind(1, hdr.width);
        this->addTilesDataRowStatement->bind(2, hdr.height);
        this->addTilesDataRowStatement->bind(3, hdr.depth);
        this->addTilesDataRowStatement->bind(4, hdr.pixeltype);
        this->addTilesDataRowStatement->bind(5, SlImgDoc::DataTypes::UNCOMPRESSED_BRICK);
        this->addTilesDataRowStatement->bind(6, binhdr, sizeof(binhdr));
        const void* p; size_t s;
        data->GetData(&p, &s);
        this->addTilesDataRowStatement->bindNoCopy(7, p, (int)s);

        this->addTilesDataRowStatement->exec();

        return this->GetDb().getLastInsertRowid();
    }
    catch (SQLite::Exception & excp)
    {
        std::cout << excp.what();
        throw;
    }
}

std::int64_t CDbWrite3D::AddBrickData(std::uint32_t width, std::uint32_t height, std::uint32_t depth, std::uint8_t pixeltype, std::uint8_t datatype, size_t sizeBinHdr, const void* binHdr, const IDataObjBase* data)
{
    try
    {
        this->EnsureAddTilesDataRowStatement();
        this->addTilesDataRowStatement->reset();
        this->addTilesDataRowStatement->bind(1, width);
        this->addTilesDataRowStatement->bind(2, height);
        this->addTilesDataRowStatement->bind(3, depth);
        this->addTilesDataRowStatement->bind(4, pixeltype);
        this->addTilesDataRowStatement->bind(5, datatype);
        this->addTilesDataRowStatement->bind(6, binHdr, sizeBinHdr);
        const void* p; size_t s;
        data->GetData(&p, &s);
        this->addTilesDataRowStatement->bindNoCopy(7, p, (int)s);

        this->addTilesDataRowStatement->exec();

        return this->GetDb().getLastInsertRowid();
    }
    catch (SQLite::Exception & excp)
    {
        std::cout << excp.what();
        throw;
    }
}

void CDbWrite3D::AddToSpatialIndexTable(std::int64_t id, const SlImgDoc::LogicalPositionInfo3D* info)
{
    try
    {
        this->EnsureAddTilesSpatialIndexRowStatement();
        this->addTilesSpatialIndexRowStatement->reset();
        this->addTilesSpatialIndexRowStatement->bind(1, id);
        this->addTilesSpatialIndexRowStatement->bind(2, info->posX);
        this->addTilesSpatialIndexRowStatement->bind(3, info->posX + info->width);
        this->addTilesSpatialIndexRowStatement->bind(4, info->posY);
        this->addTilesSpatialIndexRowStatement->bind(5, info->posY + info->height);
        this->addTilesSpatialIndexRowStatement->bind(6, info->posZ);
        this->addTilesSpatialIndexRowStatement->bind(7, info->posZ + info->depth);
        this->addTilesSpatialIndexRowStatement->exec();
    }
    catch (SQLite::Exception & excp)
    {
        std::cout << excp.what();
    }
}

void CDbWrite3D::EnsureAddTilesDataRowStatement()
{
    if (this->addTilesDataRowStatement)
    {
        return;
    }
    else
    {
        stringstream ss;
        ss << "INSERT INTO " << this->GetDocInfo3D().GetTableName(IDbDocInfo3D::TableType::TilesData) << " ("
            << this->GetDocInfo3D().GetTileDataColumnName(IDbDocInfo3D::TilesDataColumn::PixelWidth) << ","
            << this->GetDocInfo3D().GetTileDataColumnName(IDbDocInfo3D::TilesDataColumn::PixelHeight) << ","
            << this->GetDocInfo3D().GetTileDataColumnName(IDbDocInfo3D::TilesDataColumn::PixelDepth) << ","
            << this->GetDocInfo3D().GetTileDataColumnName(IDbDocInfo3D::TilesDataColumn::PixelType) << ","
            << this->GetDocInfo3D().GetTileDataColumnName(IDbDocInfo3D::TilesDataColumn::DataType) << ","
            << this->GetDocInfo3D().GetTileDataColumnName(IDbDocInfo3D::TilesDataColumn::DataBinHdr) << ","
            << this->GetDocInfo3D().GetTileDataColumnName(IDbDocInfo3D::TilesDataColumn::Data) << ") "
            " VALUES (?1,?2,?3,?4,?5,?6,?7);";

        //this->addTilesDataRowStatement = std::make_unique<SQLite::Statement>(this->GetDb(), ss.str()); // C++14
        this->addTilesDataRowStatement = std::unique_ptr<SQLite::Statement>(new SQLite::Statement(this->GetDb(), ss.str()));
    }
}

void CDbWrite3D::EnsureAddTilesInfoRowStatement()
{
    if (this->addTilesInfoRowStatement)
    {
        return;
    }
    else
    {
        const auto dims = this->GetDocInfo().GetTileDimensions();
        stringstream ss;
        ss << "INSERT INTO " << this->GetDocInfo().GetTableName(IDbDocInfo::TableType::TilesInfo) << "(";

        // add table-names for "dimensions" first
        for (const auto dim : dims)
        {
            string tableName;
            this->GetDocInfo().GetTileInfoColumnNameForDimension(dim, tableName);
            ss << tableName << ",";
        }

        // and then the "fixed" columns
        ss << this->GetDocInfo3D().GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::TileX) << "," <<
            this->GetDocInfo3D().GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::TileY) << "," <<
            this->GetDocInfo3D().GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::TileZ) << "," <<
            this->GetDocInfo3D().GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::TileWidth) << "," <<
            this->GetDocInfo3D().GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::TileHeight) << "," <<
            this->GetDocInfo3D().GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::TileDepth) << "," <<
            this->GetDocInfo3D().GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::PyrLvl) << "," <<
            this->GetDocInfo3D().GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::TileDataId) << ")"
            "VALUES (";

        // we have 8 "fixes" columns
        for (int i = 1; i <= dims.size() + 8; ++i)
        {
            ss << "?" << i;
            if (i < dims.size() + 8)
            {
                ss << ",";
            }
        }

        ss << ");";

        //this->addTilesInfoRowStatement = std::make_unique<SQLite::Statement>(this->GetDb(), ss.str()); // C++14
        this->addTilesInfoRowStatement = std::unique_ptr<SQLite::Statement>(new SQLite::Statement(this->GetDb(), ss.str()));
    }
}

void CDbWrite3D::EnsureAddTilesSpatialIndexRowStatement()
{
    if (this->addTilesSpatialIndexRowStatement)
    {
        return;
    }
    else
    {
        stringstream ss;
        ss << "INSERT INTO " << this->GetDocInfo3D().GetTableName(IDbDocInfo3D::TableType::TilesSpatialIndex) << "(" <<
            this->GetDocInfo3D().GetTilesSpatialIndexColumnName(IDbDocInfo3D::TilesSpatialIndexColumn::Pk) << "," <<
            this->GetDocInfo3D().GetTilesSpatialIndexColumnName(IDbDocInfo3D::TilesSpatialIndexColumn::MinX) << "," <<
            this->GetDocInfo3D().GetTilesSpatialIndexColumnName(IDbDocInfo3D::TilesSpatialIndexColumn::MaxX) << "," <<
            this->GetDocInfo3D().GetTilesSpatialIndexColumnName(IDbDocInfo3D::TilesSpatialIndexColumn::MinY) << "," <<
            this->GetDocInfo3D().GetTilesSpatialIndexColumnName(IDbDocInfo3D::TilesSpatialIndexColumn::MaxY) << "," <<
            this->GetDocInfo3D().GetTilesSpatialIndexColumnName(IDbDocInfo3D::TilesSpatialIndexColumn::MinZ) << "," <<
            this->GetDocInfo3D().GetTilesSpatialIndexColumnName(IDbDocInfo3D::TilesSpatialIndexColumn::MaxZ) << ")"
            " VALUES(?1,?2,?3,?4,?5,?6,?7);";

        //this->addTilesSpatialIndexRowStatement = std::make_unique<SQLite::Statement>(this->GetDb(), ss.str()); // C++14
        this->addTilesSpatialIndexRowStatement = std::unique_ptr<SQLite::Statement>(new SQLite::Statement(this->GetDb(), ss.str()));
    }
}