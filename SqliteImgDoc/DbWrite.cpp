#include "pch.h"
#include "DbWrite.h"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>

#include <iostream>

void CDbWrite::AddSubBlock(const ISubBlkCoordinate* coord, const LogicalPositionInfo* info, const IDataObjUncompressedBitmap* data)
{
    try {
        auto idSbBlk = this->AddSubBlk(data);

        SQLite::Statement query(this->GetDb(), "INSERT INTO TILETABLE (C,T,Z,M,X,Y,WIDTH,HEIGHT,PYRLVL,SUBBLK) VALUES (?2,?3,?4,?5,?6,?7,?8,?9,?10,?11);");
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
        SQLite::Statement query(this->GetDb(), "INSERT INTO SUBBLKTABLE (PIXELWIDTH,PIXELHEIGHT,PIXELTYPE,DATATYPE,DATA_BINHDR,DATA) VALUES (?1,?2,?3,?4,?5,?6);");
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

