#include "pch.h"
#include "DbWrite3D.h"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>

#include <iostream>
#include <sstream>
#include <memory>
#include "dbutils.h"

using namespace std;
using namespace SlImgDoc;

/*virtual*/CDbWrite3D::~CDbWrite3D()
{
}

/*virtual*/dbIndex CDbWrite3D::AddBrick(const ITileCoordinate* coord, const LogicalPositionInfo3D* info, const TileBaseInfo3D* tileInfo, SlImgDoc::DataTypes datatype, const IDataObjBase* data)
{
    this->CheckIfAllDimensionGivenAndThrow(this->GetDocInfo3D().GetTileDimensions(), coord);

    try
    {
        const void* ptrHdrData; size_t sizeHdrData;
        data->GetHeader(&ptrHdrData, &sizeHdrData);
        auto idSbBlk = this->AddBrickData(tileInfo->pixelWidth, tileInfo->pixelHeight, tileInfo->pixelDepth, tileInfo->pixelType, datatype, sizeHdrData, ptrHdrData, data);

        const auto dims = this->GetDocInfo3D().GetTileDimensions();

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
        return id;
    }
    catch (SQLite::Exception & excp)
    {
        std::cout << excp.what();
        throw "error";
    }
}

std::int64_t CDbWrite3D::AddBrickData(std::uint32_t width, std::uint32_t height, std::uint32_t depth, std::uint8_t pixeltype, SlImgDoc::DataTypes datatype, size_t sizeBinHdr, const void* binHdr, const IDataObjBase* data)
{
    this->CheckSizeOfBinHdrAndThrow(sizeBinHdr, this->GetDocInfo3D().GetDbParameter(IDbDocInfo3D::DbParameter::DataBinHdrSize));

    try
    {
        this->EnsureAddTilesDataRowStatement();
        this->addTilesDataRowStatement->reset();
        this->addTilesDataRowStatement->bind(1, width);
        this->addTilesDataRowStatement->bind(2, height);
        this->addTilesDataRowStatement->bind(3, depth);
        this->addTilesDataRowStatement->bind(4, pixeltype);
        this->addTilesDataRowStatement->bind(5, static_cast<underlying_type<DataTypes>::type>(datatype));
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
        const auto dims = this->GetDocInfo3D().GetTileDimensions();
        stringstream ss;
        ss << "INSERT INTO " << this->GetDocInfo3D().GetTableName(IDbDocInfo3D::TableType::TilesInfo) << "(";

        // add table-names for "dimensions" first
        for (const auto dim : dims)
        {
            string tableName;
            this->GetDocInfo3D().GetTileInfoColumnNameForDimension(dim, tableName);
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

/*virtual*/void CDbWrite3D::AddPerTileData(SlImgDoc::dbIndex index, std::function<bool(int, SlImgDoc::KeyVariadicValuePair&)> funcGetData)
{
    stringstream ss;
    ss << "INSERT INTO " << this->GetDocInfo3D().GetTableName(IDbDocInfo3D::TableType::PerBrickData) << "(";
    ss << this->GetDocInfo3D().GetPerTilesDataColumnName(IDbDocInfo3D::PerTileDataColumn::Pk);

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