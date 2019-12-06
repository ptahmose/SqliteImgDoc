#include "pch.h"
#include "DbWrite.h"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>

#include <iostream>
#include <sstream>

using namespace std;

/*virtual*/CDbWrite::~CDbWrite()
{
    if (this->transactionPending == true)
    {
        this->RollbackTransaction();
    }
}

void CDbWrite::AddSubBlock(const SlImgDoc::ITileCoordinate* coord, const LogicalPositionInfo* info, const IDataObjUncompressedBitmap* data)
{
    // TODO: check if coord contains all dimensions (as required by this->docInfo->GetTileDimensions())
    try
    {
        auto idSbBlk = this->AddSubBlk(data);

        const auto dims = this->docInfo->GetTileDimensions();
        /*stringstream ss;
        ss << "INSERT INTO " << this->docInfo->GetTableName(IDbDocInfo::TableType::TilesInfo) << "(";

        // add table-names for "dimensions" first
        for (const auto dim : dims)
        {
            string tableName;
            this->docInfo->GetTileInfoColumnNameForDimension(dim, tableName);
            ss << tableName << ",";
        }

        // and then the "fixed" columns
        ss << this->docInfo->GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileX) << "," <<
            this->docInfo->GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileY) << "," <<
            this->docInfo->GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileWidth) << "," <<
            this->docInfo->GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileHeight) << "," <<
            this->docInfo->GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::PyrLvl) << "," <<
            this->docInfo->GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileDataId) << ")"
            "VALUES (";

        // we have 6 "fixes" columns
        for (int i = 1; i <= dims.size() + 6; ++i)
        {
            ss << "?" << i;
            if (i < dims.size() + 6)
            {
                ss << ",";
            }
        }

        ss << ");";

        SQLite::Statement query(this->GetDb(), ss.str());*/

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
        this->addTilesInfoRowStatement->bind(i++, info->width);
        this->addTilesInfoRowStatement->bind(i++, info->height);
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

/*virtual*/void CDbWrite::BeginTransaction()
{
    if (this->transactionPending != false)
    {
        throw runtime_error("A transaction was already pending.");
    }

    this->GetDb().exec("BEGIN");
    this->transactionPending = true;
}

/*virtual*/void CDbWrite::CommitTransaction()
{
    if (this->transactionPending != true)
    {
        throw runtime_error("There is no pending transaction.");
    }

    this->GetDb().exec("COMMIT");
    this->transactionPending = false;
}

/*virtual*/void CDbWrite::RollbackTransaction()
{
    if (this->transactionPending != true)
    {
        throw runtime_error("There is no pending transaction.");
    }

    try
    {
        this->GetDb().exec("ROLLBACK");
    }
    catch (SQLite::Exception&)
    {
        // TODO: deal with error, see https://www.sqlite.org/lang_transaction.html for the rules...
    }

    this->transactionPending = false;
}

std::int64_t CDbWrite::AddSubBlk(const IDataObjUncompressedBitmap* data)
{
    try
    {
        this->EnsureAddTilesDataRowStatement();
        this->addTilesDataRowStatement->reset();

        //stringstream ss;
        ////ss << "INSERT INTO " << CDbBase::TableName_TileData << " (PIXELWIDTH,PIXELHEIGHT,PIXELTYPE,DATATYPE,DATA_BINHDR,DATA) VALUES (?1,?2,?3,?4,?5,?6);";
        //ss << "INSERT INTO " << this->docInfo->GetTableName(IDbDocInfo::TableType::TilesData) << " ("
        //    << this->docInfo->GetTileDataColumnName(IDbDocInfo::TilesDataColumn::PixelWidth) << ","
        //    << this->docInfo->GetTileDataColumnName(IDbDocInfo::TilesDataColumn::PixelHeight) << ","
        //    << this->docInfo->GetTileDataColumnName(IDbDocInfo::TilesDataColumn::PixelType) << ","
        //    << this->docInfo->GetTileDataColumnName(IDbDocInfo::TilesDataColumn::DataType) << ","
        //    << this->docInfo->GetTileDataColumnName(IDbDocInfo::TilesDataColumn::DataBinHdr) << ","
        //    << this->docInfo->GetTileDataColumnName(IDbDocInfo::TilesDataColumn::Data) << ") "
        //    " VALUES (?1,?2,?3,?4,?5,?6);";
        //SQLite::Statement query(this->GetDb(), ss.str());
        // 
        const auto hdr = data->GetBinHdr();

        std::uint8_t binhdr[32];
        for (int i = 0; i < sizeof(binhdr); ++i) { binhdr[i] = (uint8_t)i; }

        this->addTilesDataRowStatement->bind(1, hdr.width);
        this->addTilesDataRowStatement->bind(2, hdr.height);
        this->addTilesDataRowStatement->bind(3, hdr.pixeltype);
        this->addTilesDataRowStatement->bind(4, 1);
        this->addTilesDataRowStatement->bind(5, binhdr, sizeof(binhdr));
        const void* p; size_t s;
        data->GetData(&p, &s);
        this->addTilesDataRowStatement->bindNoCopy(6, p, (int)s);

        this->addTilesDataRowStatement->exec();

        return this->GetDb().getLastInsertRowid();
    }
    catch (SQLite::Exception & excp)
    {
        std::cout << excp.what();
        throw;
    }
}

void CDbWrite::AddToSpatialIndexTable(std::int64_t id, const LogicalPositionInfo* info)
{
    try
    {
        this->EnsureAddTilesSpatialIndexRowStatement();
        this->addTilesSpatialIndexRowStatement->reset();
        /*stringstream ss;
        ss << "INSERT INTO " << this->docInfo->GetTableName(IDbDocInfo::TableType::TilesSpatialIndex) << "(" <<
            this->docInfo->GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::Pk) << "," <<
            this->docInfo->GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MinX) << "," <<
            this->docInfo->GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MaxX) << "," <<
            this->docInfo->GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MinY) << "," <<
            this->docInfo->GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MaxY) << ")"
            " VALUES(?1,?2,?3,?4,?5);";

        SQLite::Statement query(this->GetDb(), ss.str());*/
        this->addTilesSpatialIndexRowStatement->bind(1, id);
        this->addTilesSpatialIndexRowStatement->bind(2, info->posX);
        this->addTilesSpatialIndexRowStatement->bind(3, info->posX + info->width);
        this->addTilesSpatialIndexRowStatement->bind(4, info->posY);
        this->addTilesSpatialIndexRowStatement->bind(5, info->posY + info->height);
        this->addTilesSpatialIndexRowStatement->exec();
    }
    catch (SQLite::Exception & excp)
    {
        std::cout << excp.what();
    }
}

void CDbWrite::EnsureAddTilesDataRowStatement()
{
    if (this->addTilesDataRowStatement)
    {
        return;
    }
    else
    {
        stringstream ss;
        ss << "INSERT INTO " << this->docInfo->GetTableName(IDbDocInfo::TableType::TilesData) << " ("
            << this->docInfo->GetTileDataColumnName(IDbDocInfo::TilesDataColumn::PixelWidth) << ","
            << this->docInfo->GetTileDataColumnName(IDbDocInfo::TilesDataColumn::PixelHeight) << ","
            << this->docInfo->GetTileDataColumnName(IDbDocInfo::TilesDataColumn::PixelType) << ","
            << this->docInfo->GetTileDataColumnName(IDbDocInfo::TilesDataColumn::DataType) << ","
            << this->docInfo->GetTileDataColumnName(IDbDocInfo::TilesDataColumn::DataBinHdr) << ","
            << this->docInfo->GetTileDataColumnName(IDbDocInfo::TilesDataColumn::Data) << ") "
            " VALUES (?1,?2,?3,?4,?5,?6);";

        this->addTilesDataRowStatement = std::make_unique<SQLite::Statement>(this->GetDb(), ss.str());
    }
}

void CDbWrite::EnsureAddTilesInfoRowStatement()
{
    if (this->addTilesInfoRowStatement)
    {
        return;
    }
    else
    {
        const auto dims = this->docInfo->GetTileDimensions();
        stringstream ss;
        ss << "INSERT INTO " << this->docInfo->GetTableName(IDbDocInfo::TableType::TilesInfo) << "(";

        // add table-names for "dimensions" first
        for (const auto dim : dims)
        {
            string tableName;
            this->docInfo->GetTileInfoColumnNameForDimension(dim, tableName);
            ss << tableName << ",";
        }

        // and then the "fixed" columns
        ss << this->docInfo->GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileX) << "," <<
            this->docInfo->GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileY) << "," <<
            this->docInfo->GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileWidth) << "," <<
            this->docInfo->GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileHeight) << "," <<
            this->docInfo->GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::PyrLvl) << "," <<
            this->docInfo->GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileDataId) << ")"
            "VALUES (";

        // we have 6 "fixes" columns
        for (int i = 1; i <= dims.size() + 6; ++i)
        {
            ss << "?" << i;
            if (i < dims.size() + 6)
            {
                ss << ",";
            }
        }

        ss << ");";

        this->addTilesInfoRowStatement = std::make_unique<SQLite::Statement>(this->GetDb(), ss.str());
    }
}

void CDbWrite::EnsureAddTilesSpatialIndexRowStatement()
{
    if (this->addTilesSpatialIndexRowStatement)
    {
        return;
    }
    else
    {
        stringstream ss;
        ss << "INSERT INTO " << this->docInfo->GetTableName(IDbDocInfo::TableType::TilesSpatialIndex) << "(" <<
            this->docInfo->GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::Pk) << "," <<
            this->docInfo->GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MinX) << "," <<
            this->docInfo->GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MaxX) << "," <<
            this->docInfo->GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MinY) << "," <<
            this->docInfo->GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MaxY) << ")"
            " VALUES(?1,?2,?3,?4,?5);";

        this->addTilesSpatialIndexRowStatement = std::make_unique<SQLite::Statement>(this->GetDb(), ss.str());
    }
}