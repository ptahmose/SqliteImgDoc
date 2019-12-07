#include "pch.h"
#include "DbRead.h"
#include <sstream>
#include <iostream>
#include <algorithm> 

using namespace std;
using namespace SlImgDoc;

/*virtual*/void CDbRead::ReadTileInfo(SlImgDoc::dbIndex idx, SlImgDoc::TileCoordinate* coord, LogicalPositionInfo* info)
{
    stringstream ss;

    ss << "SELECT ";
    bool isFirst = true;

    auto tileDimension = this->docInfo->GetTileDimensions();
    if (coord != nullptr)
    {
        for (const auto& d : tileDimension)
        {
            string colName;
            this->docInfo->GetTileInfoColumnNameForDimension(d, colName);
            if (isFirst == false)
            {
                ss << ",";
            }

            ss << colName;
            isFirst = false;
        }
    }

    if (info != nullptr)
    {
        if (coord != nullptr)
        {
            ss << ",";
        }

        ss << this->docInfo->GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileX) <<
            "," << this->docInfo->GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileY) <<
            "," << this->docInfo->GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileWidth) <<
            "," << this->docInfo->GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileHeight);
    }

    ss << " FROM '" << this->docInfo->GetTableName(IDbDocInfo::TableType::TilesInfo) << "' ";
    ss << "WHERE " << this->docInfo->GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::Pk) << "= ?1;";

    SQLite::Statement query(this->GetDb(), ss.str());
    query.bind(1, idx);

    bool b = query.executeStep();

    if (coord != nullptr)
    {
        for (int i = 0; i < tileDimension.size(); ++i)
        {
            coord->Set(tileDimension[i], query.getColumn(i).getInt());
        }
    }

    if (info != nullptr)
    {
        int i = tileDimension.size();
        info->posX = query.getColumn(i).getDouble();
        info->posY = query.getColumn(i + 1).getDouble();
        info->width = query.getColumn(i + 2).getDouble();
        info->height = query.getColumn(i + 3).getDouble();
    }
}

/*virtual*/void CDbRead::ReadTileData(dbIndex idx, TilePixelInfo* pixelInfo, IBlob* data)
{
    stringstream ss;    //ss << "SELECT Pixelwidth,Pixelheight,Pixeltype,Datatype,Data_BinHdr,Data FROM TILESDATA WHERE rowId=(SELECT TileDataId FROM TILESINFO WHERE Pk=?1);";
    ss << "SELECT ";
    if (pixelInfo != nullptr)
    {
        ss << this->docInfo->GetTileDataColumnName(IDbDocInfo::TilesDataColumn::PixelWidth) << "," <<
            this->docInfo->GetTileDataColumnName(IDbDocInfo::TilesDataColumn::PixelHeight) << "," <<
            this->docInfo->GetTileDataColumnName(IDbDocInfo::TilesDataColumn::PixelType) << "," <<
            this->docInfo->GetTileDataColumnName(IDbDocInfo::TilesDataColumn::DataType) << "," <<
            this->docInfo->GetTileDataColumnName(IDbDocInfo::TilesDataColumn::DataBinHdr);
    }

    if (data != nullptr)
    {
        if (pixelInfo != nullptr)
        {
            ss << ",";
        }

        ss << this->docInfo->GetTileDataColumnName(IDbDocInfo::TilesDataColumn::Data);
    }

    ss << " FROM " << this->docInfo->GetTableName(IDbDocInfo::TableType::TilesData) << " WHERE " << this->docInfo->GetTileDataColumnName(IDbDocInfo::TilesDataColumn::Pk) <<
        " = (SELECT " << this->docInfo->GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileDataId) << " FROM " << this->docInfo->GetTableName(IDbDocInfo::TableType::TilesInfo) <<
        " WHERE " << this->docInfo->GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::Pk) << " = ?1);";

    try
    {
        SQLite::Statement query(this->GetDb(), ss.str());
        query.bind(1, idx);

        bool b = query.executeStep();

        int colNo = 0;
        if (pixelInfo != nullptr)
        {
            pixelInfo->pixelWidth = query.getColumn(colNo++).getInt();
            pixelInfo->pixelHeight = query.getColumn(colNo++).getInt();
            pixelInfo->pixelType = query.getColumn(colNo++).getInt();
            pixelInfo->dataType = query.getColumn(colNo++).getInt();

            const auto& binHdr = query.getColumn(colNo++);
            memcpy(pixelInfo->dataBinHdr, binHdr.getBlob(), (std::min)(binHdr.getBytes(), (int)sizeof(pixelInfo->dataBinHdr)));
        }

        if (data != nullptr)
        {
            const auto& dataBlob = query.getColumn(colNo++);
            auto size = dataBlob.getBytes();
            data->Reserve(size);

            void* pDst;
            data->GetPointer(size, 0, pDst);

            memcpy(pDst, dataBlob.getBlob(), size);
        }
    }
    catch (SQLite::Exception & excp)
    {
        std::cout << excp.what();
        throw;
    }
}
