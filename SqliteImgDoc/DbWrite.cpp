#include "pch.h"
#include "DbWrite.h"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>

#include <iostream>
#include <sstream>

using namespace std;

void CDbWrite::AddSubBlock(const ISubBlkCoordinate* coord, const LogicalPositionInfo* info, const IDataObjUncompressedBitmap* data)
{
    try {
        auto idSbBlk = this->AddSubBlk(data);

        stringstream ss;
        ss << "INSERT INTO " << CDbBase::TableName_TileTable << " (C,T,Z,M,X,Y,WIDTH,HEIGHT,PYRLVL,SUBBLK) VALUES (?2,?3,?4,?5,?6,?7,?8,?9,?10,?11);";

        SQLite::Statement query(this->GetDb(), ss.str()/*"INSERT INTO TILETABLE (C,T,Z,M,X,Y,WIDTH,HEIGHT,PYRLVL,SUBBLK) VALUES (?2,?3,?4,?5,?6,?7,?8,?9,?10,?11);"*/);
        //query.bind(1+0, CDbBase::TableName_TileTable);
        int i;
        coord->TryGetCoordinate('C', &i);
        query.bind(1 + 1, i);
        coord->TryGetCoordinate('T', &i);
        query.bind(1 + 2, i);
        coord->TryGetCoordinate('Z', &i);
        query.bind(1 + 3, i);
        coord->TryGetCoordinate('M', &i);
        query.bind(1 + 4, i);

        query.bind(1 + 5, info->posX);
        query.bind(1 + 6, info->posY);
        query.bind(1 + 7, info->width);
        query.bind(1 + 8, info->height);

        query.bind(1 + 9, info->pyrLvl);
        query.bind(11, idSbBlk);

        query.exec();

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
    this->transaction = new SQLite::Transaction(this->GetDb());
}

/*virtual*/void CDbWrite::CommitTransaction()
{
    this->transaction->commit();
    delete this->transaction;
    this->transaction = nullptr;
}

std::int64_t CDbWrite::AddSubBlk(const IDataObjUncompressedBitmap* data)
{
    try
    {
        stringstream ss;
        //ss << "INSERT INTO " << CDbBase::TableName_TileData << " (PIXELWIDTH,PIXELHEIGHT,PIXELTYPE,DATATYPE,DATA_BINHDR,DATA) VALUES (?1,?2,?3,?4,?5,?6);";
        ss << "INSERT INTO " << this->docInfo->GetTableName(IDbDocInfo::TableType::TilesData) << " (" 
            << this->docInfo->GetTileDataColumnName(IDbDocInfo::TilesDataColumn::PixelWidth) << ","
            << this->docInfo->GetTileDataColumnName(IDbDocInfo::TilesDataColumn::PixelHeight) << ","
            << this->docInfo->GetTileDataColumnName(IDbDocInfo::TilesDataColumn::PixelType) << ","
            << this->docInfo->GetTileDataColumnName(IDbDocInfo::TilesDataColumn::DataType) << ","
            << this->docInfo->GetTileDataColumnName(IDbDocInfo::TilesDataColumn::DataBinHdr) << ","
            << this->docInfo->GetTileDataColumnName(IDbDocInfo::TilesDataColumn::Data) << ") "
            " VALUES (?1,?2,?3,?4,?5,?6);"; 
        SQLite::Statement query(this->GetDb(), ss.str()/*"INSERT INTO SUBBLKTABLE (PIXELWIDTH,PIXELHEIGHT,PIXELTYPE,DATATYPE,DATA_BINHDR,DATA) VALUES (?1,?2,?3,?4,?5,?6);"*/);
        const auto hdr = data->GetBinHdr();

        std::uint8_t binhdr[32];
        for (int i = 0; i < sizeof(binhdr); ++i) { binhdr[i] = (uint8_t)i; }

        query.bind(1, hdr.width);
        query.bind(2, hdr.height);
        query.bind(3, hdr.pixeltype);
        query.bind(4, 1);
        query.bind(5, binhdr, sizeof(binhdr));
        const void* p; size_t s;
        data->GetData(&p, &s);
        query.bindNoCopy(6, p, (int)s);

        query.exec();

        return this->GetDb().getLastInsertRowid();
    }
    catch (SQLite::Exception & excp)
    {
        std::cout << excp.what();
    }
}

void CDbWrite::AddToSpatialIndexTable(std::int64_t id, const LogicalPositionInfo* info)
{
    try
    {
        stringstream ss;
        ss << "INSERT INTO " << /*CDbBase::VTableName_SpatialTable*/this->docInfo->GetTableName(IDbDocInfo::TableType::TilesSpatialIndex) << " VALUES(?1,?2,?3,?4,?5);";

        SQLite::Statement query(this->GetDb(), ss.str());
        query.bind(1, id);
        query.bind(2, info->posX);
        query.bind(3, info->posX + info->width);
        query.bind(4, info->posY);
        query.bind(5, info->posY + info->height);
        query.exec();
    }
    catch (SQLite::Exception & excp)
    {
        std::cout << excp.what();
    }
}

