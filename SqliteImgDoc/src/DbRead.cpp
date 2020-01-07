#include "pch.h"
#include "DbRead.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include "CCustomQueries.h"
//#define SQLITE_ENABLE_RTREE
#include <sqlite3.h>
#include "miscutilities.h"
//#include <sqlite3ext.h>

using namespace std;
using namespace SlImgDoc;

std::vector<dbIndex> IDbRead::GetTilesIntersectingRect(const RectangleD& rect)
{
    std::vector<dbIndex> result;
    this->GetTilesIntersectingRect(rect, [&](dbIndex idx)->bool {result.push_back(idx); return true; });
    return result;
}

std::vector<dbIndex> IDbRead::GetTilesIntersectingWithLine(const LineThruTwoPointsD& line)
{
    std::vector<dbIndex> result;
    this->GetTilesIntersectingWithLine(line, [&](dbIndex idx)->bool {result.push_back(idx); return true; });
    return result;
}

std::vector<dbIndex> IDbRead::Query(const IDimCoordinateQueryClause* clause)
{
    std::vector<dbIndex> result;
    this->Query(clause, [&](dbIndex idx)->bool {result.push_back(idx); return true; });
    return result;
}

/*virtual*/void CDbRead::ReadTileInfo(SlImgDoc::dbIndex idx, SlImgDoc::TileCoordinate* coord, LogicalPositionInfo* info)
{
    stringstream ss;

    ss << "SELECT ";
    bool isFirst = true;

    auto tileDimension = this->GetDocInfo().GetTileDimensions();
    if (coord != nullptr)
    {
        for (const auto& d : tileDimension)
        {
            string colName;
            this->GetDocInfo().GetTileInfoColumnNameForDimension(d, colName);
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

        ss << this->GetDocInfo().GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileX) <<
            "," << this->GetDocInfo().GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileY) <<
            "," << this->GetDocInfo().GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileWidth) <<
            "," << this->GetDocInfo().GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileHeight) <<
            "," << this->GetDocInfo().GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::PyrLvl);
    }

    ss << " FROM '" << this->GetDocInfo().GetTableName(IDbDocInfo::TableType::TilesInfo) << "' ";
    ss << "WHERE " << this->GetDocInfo().GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::Pk) << "= ?1;";

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
        int i = (coord != nullptr) ? tileDimension.size() : 0;
        info->posX = query.getColumn(i).getDouble();
        info->posY = query.getColumn(i + 1).getDouble();
        info->width = query.getColumn(i + 2).getDouble();
        info->height = query.getColumn(i + 3).getDouble();
        info->pyrLvl = query.getColumn(i + 4).getInt();
    }
}

/*virtual*/void CDbRead::ReadTileData(dbIndex idx, TilePixelInfo* pixelInfo, IBlob* data)
{
    stringstream ss;    //ss << "SELECT Pixelwidth,Pixelheight,Pixeltype,Datatype,Data_BinHdr,Data FROM TILESDATA WHERE rowId=(SELECT TileDataId FROM TILESINFO WHERE Pk=?1);";
    ss << "SELECT ";
    if (pixelInfo != nullptr)
    {
        ss << this->GetDocInfo().GetTileDataColumnName(IDbDocInfo::TilesDataColumn::PixelWidth) << "," <<
            this->GetDocInfo().GetTileDataColumnName(IDbDocInfo::TilesDataColumn::PixelHeight) << "," <<
            this->GetDocInfo().GetTileDataColumnName(IDbDocInfo::TilesDataColumn::PixelType) << "," <<
            this->GetDocInfo().GetTileDataColumnName(IDbDocInfo::TilesDataColumn::DataType) << "," <<
            this->GetDocInfo().GetTileDataColumnName(IDbDocInfo::TilesDataColumn::DataBinHdr);
    }

    if (data != nullptr)
    {
        if (pixelInfo != nullptr)
        {
            ss << ",";
        }

        ss << this->GetDocInfo().GetTileDataColumnName(IDbDocInfo::TilesDataColumn::Data);
    }

    ss << " FROM " << this->GetDocInfo().GetTableName(IDbDocInfo::TableType::TilesData) << " WHERE " << this->GetDocInfo().GetTileDataColumnName(IDbDocInfo::TilesDataColumn::Pk) <<
        " = (SELECT " << this->GetDocInfo().GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::TileDataId) << " FROM " << this->GetDocInfo().GetTableName(IDbDocInfo::TableType::TilesInfo) <<
        " WHERE " << this->GetDocInfo().GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::Pk) << " = ?1);";

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

            // TODO: we should not query the dataBinHdr if it isn't necessary in the first place
            if (pixelInfo->dataBinHdr != nullptr)
            {
                const auto& binHdr = query.getColumn(colNo++);
                pixelInfo->dataBinHdr->Set(binHdr.getBlob(), binHdr.getBytes());
            }
            //const auto& binHdr = query.getColumn(colNo++);
            //memcpy(pixelInfo->dataBinHdr, binHdr.getBlob(), (std::min)(binHdr.getBytes(), (int)sizeof(pixelInfo->dataBinHdr)));
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

/*virtual*/void CDbRead::GetTilesIntersectingRect(const RectangleD& rect, std::function<bool(dbIndex)> func)
{
    stringstream ss;    //ss << "SELECT Pixelwidth,Pixelheight,Pixeltype,Datatype,Data_BinHdr,Data FROM TILESDATA WHERE rowId=(SELECT TileDataId FROM TILESINFO WHERE Pk=?1);";
    ss << "SELECT " << this->GetDocInfo().GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::Pk) << " FROM "
        << this->GetDocInfo().GetTableName(IDbDocInfo::TableType::TilesSpatialIndex) << " WHERE " <<
        this->GetDocInfo().GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MaxX) << ">=?1 AND " <<
        this->GetDocInfo().GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MinX) << "<=?2 AND " <<
        this->GetDocInfo().GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MaxY) << ">=?3 AND " <<
        this->GetDocInfo().GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MinY) << "<=?4";

    SQLite::Statement query(this->GetDb(), ss.str());
    query.bind(1, rect.x);
    query.bind(2, rect.x + rect.w);
    query.bind(3, rect.y);
    query.bind(4, rect.y + rect.h);

    while (query.executeStep())
    {
        dbIndex idx = query.getColumn(0).getInt64();
        bool b = func(idx);
        if (!b)
        {
            break;
        }
    }
}

/*virtual*/void CDbRead::GetTilesIntersectingWithLine(const LineThruTwoPointsD& rect, std::function<bool(dbIndex)> func)
{
    stringstream ss;
    //ss << "SELECT id FROM TILESPATIAL_index WHERE id MATCH LineThroughPoints(?1,?2,?3,?4)";
    ss << "SELECT id FROM " << this->GetDocInfo().GetTableName(IDbDocInfo::TableType::TilesSpatialIndex) << " WHERE id MATCH " << CCustomQueries::GetQueryFunctionName(CCustomQueries::Query::RTree_LineSegment2D) << "(?1,?2,?3,?4)";
    SQLite::Statement query(this->GetDb(), ss.str());
    query.bind(1, rect.a.x);
    query.bind(2, rect.a.y);
    query.bind(3, rect.b.x);
    query.bind(4, rect.b.y);

    try
    {
        while (query.executeStep())
        {
            dbIndex idx = query.getColumn(0).getInt64();
            bool b = func(idx);
            if (!b)
            {
                break;
            }
        }
    }
    catch (SQLite::Exception & excp)
    {
        std::cout << excp.what();
        throw;
    }
}

/*virtual*/void CDbRead::ReadPerTileData(SlImgDoc::dbIndex idx, const std::vector<std::string>& columns, std::function<bool(const KeyVariadicValuePair&)> func)
{
    stringstream ss;
    ss << "SELECT ";
    MiscUtils::AddCommaSeparatedAndEnclose(ss, columns.cbegin(), columns.cend(), "[", "]");
    ss << " FROM [" << this->GetDocInfo().GetTableName(IDbDocInfo::TableType::CoordinateData) << "] " <<
        "WHERE [" << this->GetDocInfo().GetPerTilesDataColumnName(IDbDocInfo::PerTileDataColumn::Pk) << "]=?1";
    SQLite::Statement query(this->GetDb(), ss.str());
    query.bind(1, idx);

    const auto& perTileDataColumnInfo = this->GetDocInfo().GetCoordinateDataColumnInfo();
    try
    {
        while (query.executeStep())
        {
            int colCnt = query.getColumnCount();
            for (int i=0;i<colCnt;++i)
            {
                const auto& colName = query.getColumnName(i);
                const auto& it = find_if(perTileDataColumnInfo.cbegin(), perTileDataColumnInfo.cend(), [&](const ColumnTypeAllInfo& info)->bool {return info.columnName == colName; });

                auto kv = MiscUtils::ReadValue(query, i, *it);
                bool b = func(kv);
                if (!b)
                {
                    break;
                }
            }
            
           /* dbIndex idx = query.getColumn(0).getInt64();
            bool b = func(idx);
            if (!b)
            {
                break;
            }*/
        }
    }
    catch (SQLite::Exception & excp)
    {
        std::cout << excp.what();
    }
}

/*virtual*/void CDbRead::Query(const SlImgDoc::IDimCoordinateQueryClause* clause, std::function<bool(dbIndex)> func)
{

    stringstream ss;
    ss << "SELECT " << this->GetDocInfo().GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::Pk) << " FROM " << this->GetDocInfo().GetTableName(IDbDocInfo::TableType::TilesInfo) << " WHERE ";

    auto rangeDims = clause->GetTileDimsForClause();
    bool firstDim = true;
    for (const auto dim : rangeDims)
    {
        string dimColumnName;
        this->GetDocInfo().GetTileInfoColumnNameForDimension(dim, dimColumnName);

        auto ranges = clause->GetRangeClause(dim);
        auto list = clause->GetListClause(dim);

        //if (!ranges.has_value() && !list.has_value())
        if (ranges != nullptr && list != nullptr)
        {
            throw invalid_argument("...TODO...");
        }

        if (!firstDim)
        {
            ss << " AND ";
        }

        ss << "(";
        bool first = true;

        //if (ranges.has_value())
        if (ranges != nullptr)
        {
            //for (const auto& r : ranges.value().get())
            for (const auto& r : *ranges)
            {
                if (!first)
                {
                    ss << " OR ";
                }

                ss << "(" << dimColumnName << ">=" << r.start << " AND " << dimColumnName << "<=" << r.end << ")";
                first = false;
            }
        }

        //if (list.has_value())
        if (list != nullptr)
        {
            //for (const auto& l : list.value().get())
            for (const auto& l : *list)
            {
                if (!first)
                {
                    ss << " OR ";
                }

                ss << "(" << dimColumnName << " IN (";

                bool firstInList = true;
                for (const auto& i : l.list)
                {
                    if (!firstInList)
                    {
                        ss << ",";
                    }

                    ss << i;
                    firstInList = false;
                }

                ss << "))";
                first = false;
            }
        }

        ss << ")";

        firstDim = false;
    }

    try
    {
        SQLite::Statement query(this->GetDb(), ss.str());
        while (query.executeStep())
        {
            dbIndex idx = query.getColumn(0).getInt64();
            bool b = func(idx);
            if (!b)
            {
                break;
            }
        }
    }
    catch (SQLite::Exception & excp)
    {
        std::cout << excp.what();
    }
}
