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

/*
static unique_ptr<SQLite::Statement> spatialIndexQuery;

static bool HasSpatialIndex(SQLite::Database& db, const std::string& tableName)
{
    if (!spatialIndexQuery)
    {
        stringstream sql;
        sql << "SELECT count(*) FROM sqlite_master WHERE type = 'table' AND name = '" << tableName << "'";
        spatialIndexQuery = make_unique<SQLite::Statement>(db,sql.str());
    }

    spatialIndexQuery->reset();

    spatialIndexQuery->executeStep();

    int r = spatialIndexQuery->getColumn(0).getInt();

    return r != 0 ? true : false;
}
*/

/*virtual*/dbIndex CDbWrite::AddTile(const SlImgDoc::ITileCoordinate* coord, const SlImgDoc::LogicalPositionInfo* info, const SlImgDoc::TileBaseInfo* tileInfo, SlImgDoc::DataTypes datatype, const IDataObjBase* data)
{
    this->CheckIfAllDimensionGivenAndThrow(this->GetDocInfo().GetTileDimensions(), coord);

    try
    {
        bool transactionInitiated = false;

        // TODO: if there isn't a transaction, we need to enclose all INSERTS into one transaction
        if (!this->GetIsTransactionPending())
        {
            this->BeginTransaction();
            transactionInitiated = true;
        }

        const void* ptrHdrData; size_t sizeHdrData;
        data->GetHeader(&ptrHdrData, &sizeHdrData);
        const auto idSbBlk = this->AddTileData(tileInfo->pixelWidth, tileInfo->pixelHeight, tileInfo->pixelType, datatype, sizeHdrData, ptrHdrData, data);

        const auto dims = this->GetDocInfo().GetTileDimensions();

        this->EnsureAddTilesInfoRowStatement();
        this->addTilesInfoRowStatement->reset();

        for (int i = 1; i <= static_cast<int>(dims.size()); ++i)
        {
            int v;
            coord->TryGetCoordinate(dims[i - 1], &v);
            this->addTilesInfoRowStatement->bind(i, v);
        }

        int i = static_cast<int>(dims.size()) + 1;
        this->addTilesInfoRowStatement->bind(i++, info->posX);
        this->addTilesInfoRowStatement->bind(i++, info->posY);
        this->addTilesInfoRowStatement->bind(i++, info->width);
        this->addTilesInfoRowStatement->bind(i++, info->height);
        this->addTilesInfoRowStatement->bind(i++, info->pyrLvl);
        this->addTilesInfoRowStatement->bind(i++, idSbBlk);

        this->addTilesInfoRowStatement->exec();

        auto id = this->GetDb().getLastInsertRowid();

        // TODO: is there a better way instead of querying the database here?
        //if (DbUtils::DoesTableExists(this->GetDb(), this->GetDocInfo().GetTableName(IDbDocInfo::TableType::TilesSpatialIndex)))
        if (this->IsSpatialIndexActive())
            // if (HasSpatialIndex(this->GetDb(), this->GetDocInfo().GetTableName(IDbDocInfo::TableType::TilesSpatialIndex)))
        {
            this->AddToSpatialIndexTable(id, info);
        }

        if (transactionInitiated)
        {
            // TODO: this must be "exception-safe"
            this->CommitTransaction();
        }

        return id;
    }
    catch (SQLite::Exception& excp)
    {
        std::cout << excp.what();
        throw;
    }
}

std::int64_t CDbWrite::AddTileData(std::uint32_t width, std::uint32_t height, std::uint8_t pixeltype, SlImgDoc::DataTypes datatype, size_t sizeBinHdr, const void* binHdr, const IDataObjBase* data)
{
    this->CheckSizeOfBinHdrAndThrow(sizeBinHdr, this->GetDocInfo().GetDbParameter(IDbDocInfo::DbParameter::DataBinHdrSize));

    try
    {
        this->EnsureAddTilesDataRowStatement();
        this->addTilesDataRowStatement->reset();
        this->addTilesDataRowStatement->bind(1, width);
        this->addTilesDataRowStatement->bind(2, height);
        this->addTilesDataRowStatement->bind(3, pixeltype);
        this->addTilesDataRowStatement->bind(4, static_cast<underlying_type<DataTypes>::type>(datatype));
        this->addTilesDataRowStatement->bind(5, binHdr, sizeBinHdr);
        const void* p; size_t s;
        data->GetData(&p, &s);
        this->addTilesDataRowStatement->bindNoCopy(6, p, (int)s);

        this->addTilesDataRowStatement->exec();

        return this->GetDb().getLastInsertRowid();
    }
    catch (SQLite::Exception& excp)
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
    catch (SQLite::Exception& excp)
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

/*virtual*/void CDbWrite::DropIndexOnCoordinate(TileDim dim)
{
    string indexName;
    bool b = this->GetDocInfo().GetTileInfoIndexNameForDimension(dim, indexName);
    if (!b)
    {
        throw runtime_error("Invalid dimension");
    }

    DbUtils::DropIndexForTable(this->GetDb(), indexName);
}

/*virtual*/void CDbWrite::CreateIndexOnCoordinate(TileDim dim)
{
    string indexName, columnName;
    bool b = this->GetDocInfo().GetTileInfoIndexNameForDimension(dim, indexName);
    b = this->GetDocInfo().GetTileInfoColumnNameForDimension(dim, columnName);

    DbUtils::CreateIndexForTable(
        this->GetDb(),
        indexName,
        this->GetDocInfo().GetTableName(IDbDocInfo::TableType::TilesInfo),
        columnName);
}

/*virtual*/void CDbWrite::CreateSpatialIndex()
{
    // ok, what we need to do is:
    // - create the Virtual Table
    // - put all the information into it
    // - done

    auto sql = stringstream();
    sql << "CREATE VIRTUAL TABLE " << this->GetDocInfo().GetTableName(IDbDocInfo::TableType::TilesSpatialIndex) << " USING rtree(" <<
        this->GetDocInfo().GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::Pk) << "," <<         // Integer primary key
        this->GetDocInfo().GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MinX) << "," <<       // Minimum X coordinate"
        this->GetDocInfo().GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MaxX) << "," <<       // Maximum X coordinate"
        this->GetDocInfo().GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MinY) << "," <<       // Minimum Y coordinate"
        this->GetDocInfo().GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MaxY) << ");";        // Maximum Y coordinate"

    sql << "INSERT INTO " << this->GetDocInfo().GetTableName(IDbDocInfo::TableType::TilesSpatialIndex) << "(" <<
        this->GetDocInfo().GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::Pk) << "," <<
        this->GetDocInfo().GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MinX) << "," <<
        this->GetDocInfo().GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MaxX) << "," <<
        this->GetDocInfo().GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MinY) << "," <<
        this->GetDocInfo().GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MaxY) << ") " <<
        "SELECT " <<
        this->GetDocInfo().GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::Pk) << "," <<
        this->GetDocInfo().GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileX) << "," <<
        this->GetDocInfo().GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileX) << "+" << this->GetDocInfo().GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileWidth) << "," <<
        this->GetDocInfo().GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileY) << "," <<
        this->GetDocInfo().GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileY) << "+" << this->GetDocInfo().GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileHeight) << " " <<
        "FROM " << this->GetDocInfo().GetTableName(IDbDocInfo::TableType::TilesInfo) << ";";

    /*
       "INSERT INTO TILESPATIAL_index ( id,minX,maxX,minY,maxY)
        SELECT Pk, TilePosX, TilePosX + TileWidth, TilePosY, TilePosY + TileHeight
        FROM TILESINFO"
    */

    SQLite::Statement query(this->GetDb(), sql.str());

    int r = query.exec();

    this->UpdateSpatialIndexAvailability();
}

/*virtual*/void CDbWrite::DropSpatialIndex()
{
    DbUtils::DropSpatialIndexTableIfExists(this->GetDb(), this->GetDocInfo().GetTableName(IDbDocInfo::TableType::TilesSpatialIndex));

    // TODO: take note that there is no spatial index
    this->UpdateSpatialIndexAvailability();
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