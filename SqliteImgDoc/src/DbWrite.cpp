#include "pch.h"
#include "DbWrite.h"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>
#include "dbutils.h"

#include <iostream>
#include <sstream>
#include <memory>

using namespace std;
using namespace SlImgDoc;

/*virtual*/CDbWrite::~CDbWrite()
{
}

dbIndex CDbWrite::AddTile(const SlImgDoc::ITileCoordinate* coord, const SlImgDoc::LogicalPositionInfo* info, const IDataObjUncompressedBitmap* data)
{
    this->CheckIfAllDimensionGivenAndThrow(this->GetDocInfo().GetTileDimensions(), coord);

    try
    {
        auto idSbBlk = this->AddTileUncompressed(data);

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
        this->addTilesInfoRowStatement->bind(i++, info->width);
        this->addTilesInfoRowStatement->bind(i++, info->height);
        this->addTilesInfoRowStatement->bind(i++, info->pyrLvl);
        this->addTilesInfoRowStatement->bind(i++, idSbBlk);

        this->addTilesInfoRowStatement->exec();

        auto id = this->GetDb().getLastInsertRowid();
        this->AddToSpatialIndexTable(id, info);
        return id;
    }
    catch (SQLite::Exception & excp)
    {
        std::cout << excp.what();
        throw;
    }
}

/*virtual*/dbIndex CDbWrite::AddTile(const SlImgDoc::ITileCoordinate* coord, const SlImgDoc::LogicalPositionInfo* info, const SlImgDoc::TileBaseInfo* tileInfo, const IDataObjCustom* data)
{
    this->CheckIfAllDimensionGivenAndThrow(this->GetDocInfo().GetTileDimensions(), coord);

    try
    {
        const void* ptrHdrData; size_t sizeHdrData;
        data->GetHeader(&ptrHdrData, &sizeHdrData);
        auto idSbBlk = this->AddTileData(tileInfo->pixelWidth, tileInfo->pixelHeight, tileInfo->pixelType, SlImgDoc::DataTypes::CUSTOM, sizeHdrData, ptrHdrData, data);

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
        this->addTilesInfoRowStatement->bind(i++, info->width);
        this->addTilesInfoRowStatement->bind(i++, info->height);
        this->addTilesInfoRowStatement->bind(i++, info->pyrLvl);
        this->addTilesInfoRowStatement->bind(i++, idSbBlk);

        this->addTilesInfoRowStatement->exec();

        auto id = this->GetDb().getLastInsertRowid();
        this->AddToSpatialIndexTable(id, info);
        return id;
    }
    catch (SQLite::Exception & excp)
    {
        std::cout << excp.what();
        throw;
    }
}

///*virtual*/void CDbWrite::BeginTransaction()
//{
//    if (this->transactionPending != false)
//    {
//        throw runtime_error("A transaction was already pending.");
//    }
//
//    this->GetDb().exec("BEGIN");
//    this->transactionPending = true;
//}
//
///*virtual*/void CDbWrite::CommitTransaction()
//{
//    if (this->transactionPending != true)
//    {
//        throw runtime_error("There is no pending transaction.");
//    }
//
//    this->GetDb().exec("COMMIT");
//    this->transactionPending = false;
//}
//
///*virtual*/void CDbWrite::RollbackTransaction()
//{
//    if (this->transactionPending != true)
//    {
//        throw runtime_error("There is no pending transaction.");
//    }
//
//    try
//    {
//        this->GetDb().exec("ROLLBACK");
//    }
//    catch (SQLite::Exception&)
//    {
//        // TODO: deal with error, see https://www.sqlite.org/lang_transaction.html for the rules...
//    }
//
//    this->transactionPending = false;
//}

std::int64_t CDbWrite::AddTileUncompressed(const IDataObjUncompressedBitmap* data)
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
        this->addTilesDataRowStatement->bind(3, hdr.pixeltype);
        this->addTilesDataRowStatement->bind(4, SlImgDoc::DataTypes::UNCOMPRESSED_BITMAP);
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

std::int64_t CDbWrite::AddTileData(std::uint32_t width, std::uint32_t height, std::uint8_t pixeltype, std::uint8_t datatype, size_t sizeBinHdr, const void* binHdr, const IDataObjBase* data)
{
    //auto dataBinHDrSize = this->GetDocInfo().GetDbParameter(IDbDocInfo::DbParameter::DataBinHdrSize);
    //if (sizeBinHdr > dataBinHDrSize)
    //{
    //    stringstream ss;
    //    ss << "Size of 'BinHdr'-field is limited to " << dataBinHDrSize << " bytes, and " << sizeBinHdr << " bytes was requested.";
    //    throw SqliteImgDocInvalidArgumentException(ss.str());
    //}
    this->CheckSizeOfBinHdrAndThrow(sizeBinHdr, this->GetDocInfo().GetDbParameter(IDbDocInfo::DbParameter::DataBinHdrSize));

    try
    {
        this->EnsureAddTilesDataRowStatement();
        this->addTilesDataRowStatement->reset();
        this->addTilesDataRowStatement->bind(1, width);
        this->addTilesDataRowStatement->bind(2, height);
        this->addTilesDataRowStatement->bind(3, pixeltype);
        this->addTilesDataRowStatement->bind(4, datatype);
        this->addTilesDataRowStatement->bind(5, binHdr, sizeBinHdr);
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

void CDbWrite::AddToSpatialIndexTable(std::int64_t id, const SlImgDoc::LogicalPositionInfo* info)
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
        ss << "INSERT INTO " << this->GetDocInfo().GetTableName(IDbDocInfo::TableType::TilesData) << " ("
            << this->GetDocInfo().GetTileDataColumnName(IDbDocInfo::TilesDataColumn::PixelWidth) << ","
            << this->GetDocInfo().GetTileDataColumnName(IDbDocInfo::TilesDataColumn::PixelHeight) << ","
            << this->GetDocInfo().GetTileDataColumnName(IDbDocInfo::TilesDataColumn::PixelType) << ","
            << this->GetDocInfo().GetTileDataColumnName(IDbDocInfo::TilesDataColumn::DataType) << ","
            << this->GetDocInfo().GetTileDataColumnName(IDbDocInfo::TilesDataColumn::DataBinHdr) << ","
            << this->GetDocInfo().GetTileDataColumnName(IDbDocInfo::TilesDataColumn::Data) << ") "
            " VALUES (?1,?2,?3,?4,?5,?6);";

        //this->addTilesDataRowStatement = std::make_unique<SQLite::Statement>(this->GetDb(), ss.str()); // C++14
        this->addTilesDataRowStatement = std::unique_ptr<SQLite::Statement>(new SQLite::Statement(this->GetDb(), ss.str()));
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
        ss << this->GetDocInfo().GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileX) << "," <<
            this->GetDocInfo().GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileY) << "," <<
            this->GetDocInfo().GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileWidth) << "," <<
            this->GetDocInfo().GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileHeight) << "," <<
            this->GetDocInfo().GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::PyrLvl) << "," <<
            this->GetDocInfo().GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileDataId) << ")"
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

        //this->addTilesInfoRowStatement = std::make_unique<SQLite::Statement>(this->GetDb(), ss.str()); // C++14
        this->addTilesInfoRowStatement = std::unique_ptr<SQLite::Statement>(new SQLite::Statement(this->GetDb(), ss.str()));
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
        ss << "INSERT INTO " << this->GetDocInfo().GetTableName(IDbDocInfo::TableType::TilesSpatialIndex) << "(" <<
            this->GetDocInfo().GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::Pk) << "," <<
            this->GetDocInfo().GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MinX) << "," <<
            this->GetDocInfo().GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MaxX) << "," <<
            this->GetDocInfo().GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MinY) << "," <<
            this->GetDocInfo().GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MaxY) << ")"
            " VALUES(?1,?2,?3,?4,?5);";

        //this->addTilesSpatialIndexRowStatement = std::make_unique<SQLite::Statement>(this->GetDb(), ss.str()); // C++14
        this->addTilesSpatialIndexRowStatement = std::unique_ptr<SQLite::Statement>(new SQLite::Statement(this->GetDb(), ss.str()));
    }
}

/*virtual*/void CDbWrite::AddPerTileData(SlImgDoc::dbIndex index, std::function<bool(int, SlImgDoc::KeyVariadicValuePair&)> funcGetData)
{
    stringstream ss;
    ss << "INSERT INTO " << this->GetDocInfo().GetTableName(IDbDocInfo::TableType::PerBrickData) << "(";
    ss << this->GetDocInfo().GetPerTilesDataColumnName(IDbDocInfo::PerTileDataColumn::Pk);

    int coordDataCnt;
    for (coordDataCnt = 0;; ++coordDataCnt)
    {
        KeyVariadicValuePair kv;
        bool b = funcGetData(coordDataCnt, kv);
        if (b == false)
        {
            break;
        }

        ss << "," << kv.Name;
    }

    ss << ") VALUES(";

    bool isFirst = true;
    for (int i = 0; i < coordDataCnt + 1; ++i)
    {
        if (isFirst != true)
        {
            ss << ",";
        }

        ss << "?" << i + 1;
        isFirst = false;
    }

    ss << ");";

    SQLite::Statement statement(this->GetDb(), ss.str());
    statement.bind(1, index);

    for (coordDataCnt = 0;; ++coordDataCnt)
    {
        KeyVariadicValuePair kv;
        bool b = funcGetData(coordDataCnt, kv);
        if (b == false)
        {
            break;
        }

        ColumnTypeInfo cti;
        DbUtils::TryParse(kv.Data.DataType, &cti);
        DbUtils::Bind(statement, coordDataCnt + 2, kv.Data);
    }

    statement.exec();
}

#if false
/*virtual*/void CDbWrite::AddCoordinateData(const SlImgDoc::ITileCoordinate* coord, std::function<bool(int, SlImgDoc::CoordinateData&)> funcGetData)
{
    stringstream ss;
    ss << "INSERT INTO " << this->GetDocInfo().GetTableName(IDbDocInfo::TableType::CoordinateData) << "(";

    auto dims = coord->GetDimensions();

    bool isFirst = true;

    for (auto d : dims)
    {
        string colName;
        this->GetDocInfo().GetCoordinateDataColumnNameForDimension(d, colName);
        if (isFirst != true)
        {
            ss << ",";
        }

        ss << colName;
        isFirst = false;
    }

    int coordDataCnt;
    for (coordDataCnt = 0;; ++coordDataCnt)
    {
        CoordinateData cd;
        bool b = funcGetData(coordDataCnt, cd);
        if (b == false)
        {
            break;
        }

        if (isFirst != true)
        {
            ss << ",";
        }

        ss << cd.Name;
        isFirst = false;
    }

    ss << ") VALUES(";
    isFirst = true;
    for (int i = 0; i < coordDataCnt + dims.size(); ++i)
    {
        if (isFirst != true)
        {
            ss << ",";
        }

        ss << "?" << i + 1;
        isFirst = false;
    }

    ss << ");";

    SQLite::Statement statement(this->GetDb(), ss.str());

    int index = 1;
    for (auto d : dims)
    {
        int v;
        coord->TryGetCoordinate(d, &v);
        statement.bind(index, v);
        ++index;
    }

    for (coordDataCnt = 0;; ++coordDataCnt)
    {
        CoordinateData cd;
        bool b = funcGetData(coordDataCnt, cd);
        if (b == false)
        {
            break;
        }

        statement.bind(index, cd.doubleValue);
        ++index;
    }

    statement.exec();
}
#endif