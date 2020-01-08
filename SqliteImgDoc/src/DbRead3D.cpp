#include "pch.h"
#include "DbRead3D.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include "CCustomQueries.h"
#include <sqlite3.h>
#include "dbutils.h"

using namespace std;
using namespace SlImgDoc;

std::vector<dbIndex> IDbRead3D::GetTilesIntersectingCuboid(const CuboidD& cuboid)
{
    std::vector<dbIndex> result;
    this->GetTilesIntersectingCuboid(cuboid, [&](dbIndex idx)->bool {result.push_back(idx); return true; });
    return result;
}

std::vector<dbIndex> IDbRead3D::GetTilesIntersectingWithPlane(const Plane_NormalAndDistD& plane)
{
    std::vector<dbIndex> result;
    this->GetTilesIntersectingWithPlane(plane, [&](dbIndex idx)->bool {result.push_back(idx); return true; });
    return result;
}

//std::vector<dbIndex> IDbRead3D::Query(const IDimCoordinateQueryClause* clause)
//{
//    std::vector<dbIndex> result;
//    this->Query(clause, [&](dbIndex idx)->bool {result.push_back(idx); return true; });
//    return result;
//}

/*virtual*/void CDbRead3D::ReadTileInfo(SlImgDoc::dbIndex idx, SlImgDoc::TileCoordinate* coord, LogicalPositionInfo3D* info)
{
    stringstream ss;

    ss << "SELECT ";
    bool isFirst = true;

    auto tileDimension = this->GetDocInfo3D().GetTileDimensions();
    if (coord != nullptr)
    {
        for (const auto& d : tileDimension)
        {
            string colName;
            this->GetDocInfo3D().GetTileInfoColumnNameForDimension(d, colName);
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

        ss << this->GetDocInfo3D().GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::TileX) <<
            "," << this->GetDocInfo3D().GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::TileY) <<
            "," << this->GetDocInfo3D().GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::TileZ) <<
            "," << this->GetDocInfo3D().GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::TileWidth) <<
            "," << this->GetDocInfo3D().GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::TileHeight) <<
            "," << this->GetDocInfo3D().GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::TileDepth) <<
            "," << this->GetDocInfo3D().GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::PyrLvl);
    }

    ss << " FROM '" << this->GetDocInfo3D().GetTableName(IDbDocInfo3D::TableType::TilesInfo) << "' ";
    ss << "WHERE " << this->GetDocInfo3D().GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::Pk) << "= ?1;";

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
        info->posZ = query.getColumn(i + 2).getDouble();
        info->width = query.getColumn(i + 3).getDouble();
        info->height = query.getColumn(i + 4).getDouble();
        info->depth = query.getColumn(i + 5).getDouble();
        info->pyrLvl = query.getColumn(i + 6).getInt();
    }
}

/*virtual*/void CDbRead3D::ReadTileData(dbIndex idx, TilePixelInfo3D* pixelInfo, IBlob* data)
{
    stringstream ss;    //ss << "SELECT Pixelwidth,Pixelheight,Pixeltype,Datatype,Data_BinHdr,Data FROM TILESDATA WHERE rowId=(SELECT TileDataId FROM TILESINFO WHERE Pk=?1);";
    ss << "SELECT ";
    if (pixelInfo != nullptr)
    {
        ss << this->GetDocInfo3D().GetTileDataColumnName(IDbDocInfo3D::TilesDataColumn::PixelWidth) << "," <<
            this->GetDocInfo3D().GetTileDataColumnName(IDbDocInfo3D::TilesDataColumn::PixelHeight) << "," <<
            this->GetDocInfo3D().GetTileDataColumnName(IDbDocInfo3D::TilesDataColumn::PixelDepth) << "," <<
            this->GetDocInfo3D().GetTileDataColumnName(IDbDocInfo3D::TilesDataColumn::PixelType) << "," <<
            this->GetDocInfo3D().GetTileDataColumnName(IDbDocInfo3D::TilesDataColumn::DataType) << "," <<
            this->GetDocInfo3D().GetTileDataColumnName(IDbDocInfo3D::TilesDataColumn::DataBinHdr);
    }

    if (data != nullptr)
    {
        if (pixelInfo != nullptr)
        {
            ss << ",";
        }

        ss << this->GetDocInfo3D().GetTileDataColumnName(IDbDocInfo3D::TilesDataColumn::Data);
    }

    ss << " FROM " << this->GetDocInfo3D().GetTableName(IDbDocInfo3D::TableType::TilesData) << " WHERE " << this->GetDocInfo3D().GetTileDataColumnName(IDbDocInfo3D::TilesDataColumn::Pk) <<
        " = (SELECT " << this->GetDocInfo3D().GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::TileDataId) << " FROM " << this->GetDocInfo3D().GetTableName(IDbDocInfo3D::TableType::TilesInfo) <<
        " WHERE " << this->GetDocInfo3D().GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::Pk) << " = ?1);";

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
            pixelInfo->pixelDepth = query.getColumn(colNo++).getInt();
            pixelInfo->pixelType = query.getColumn(colNo++).getInt();
            pixelInfo->dataType = query.getColumn(colNo++).getInt();

            // TODO: we should not query the dataBinHdr if it isn't necessary in the first place
            if (pixelInfo->dataBinHdr != nullptr)
            {
                const auto& binHdr = query.getColumn(colNo++);
                pixelInfo->dataBinHdr->Set(binHdr.getBlob(), binHdr.getBytes());
            }
            /*const auto& binHdr = query.getColumn(colNo++);
            memcpy(pixelInfo->dataBinHdr, binHdr.getBlob(), (std::min)(binHdr.getBytes(), (int)sizeof(pixelInfo->dataBinHdr)));*/
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

/*virtual*/void CDbRead3D::Query(const SlImgDoc::IDimCoordinateQueryClause* clause, std::function<bool(dbIndex)> func)
{
    stringstream ss;
    ss << "SELECT " << this->GetDocInfo3D().GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::Pk) << " FROM " << this->GetDocInfo3D().GetTableName(IDbDocInfo3D::TableType::TilesInfo) << " WHERE ";

    auto rangeDims = clause->GetTileDimsForClause();
    bool firstDim = true;
    for (const auto dim : rangeDims)
    {
        string dimColumnName;
        this->GetDocInfo3D().GetTileInfoColumnNameForDimension(dim, dimColumnName);

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

        if (ranges != nullptr)
        {
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

        if (list != nullptr)
        {
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

/*virtual*/void CDbRead3D::GetTilesIntersectingCuboid(const SlImgDoc::CuboidD& rect, std::function<bool(SlImgDoc::dbIndex)> func)
{
    stringstream ss;
    ss << "SELECT " << this->GetDocInfo3D().GetTilesSpatialIndexColumnName(IDbDocInfo3D::TilesSpatialIndexColumn::Pk) << " FROM "
        << this->GetDocInfo3D().GetTableName(IDbDocInfo3D::TableType::TilesSpatialIndex) << " WHERE " <<
        this->GetDocInfo3D().GetTilesSpatialIndexColumnName(IDbDocInfo3D::TilesSpatialIndexColumn::MaxX) << ">=?1 AND " <<
        this->GetDocInfo3D().GetTilesSpatialIndexColumnName(IDbDocInfo3D::TilesSpatialIndexColumn::MinX) << "<=?2 AND " <<
        this->GetDocInfo3D().GetTilesSpatialIndexColumnName(IDbDocInfo3D::TilesSpatialIndexColumn::MaxY) << ">=?3 AND " <<
        this->GetDocInfo3D().GetTilesSpatialIndexColumnName(IDbDocInfo3D::TilesSpatialIndexColumn::MinY) << "<=?4 AND " <<
        this->GetDocInfo3D().GetTilesSpatialIndexColumnName(IDbDocInfo3D::TilesSpatialIndexColumn::MaxZ) << ">=?5 AND " <<
        this->GetDocInfo3D().GetTilesSpatialIndexColumnName(IDbDocInfo3D::TilesSpatialIndexColumn::MinZ) << "<=?6";

    SQLite::Statement query(this->GetDb(), ss.str());
    query.bind(1, rect.x);
    query.bind(2, rect.x + rect.w);
    query.bind(3, rect.y);
    query.bind(4, rect.y + rect.h);
    query.bind(5, rect.z);
    query.bind(6, rect.z + rect.d);

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

/*virtual*/void CDbRead3D::GetTilesIntersectingWithPlane(const SlImgDoc::Plane_NormalAndDistD& plane, std::function<bool(SlImgDoc::dbIndex)> func)
{
    stringstream ss;
    //ss << "SELECT id FROM TILESPATIAL_index WHERE id MATCH PlaneNormalDistance3d(?1,?2,?3,?4)";
    ss << "SELECT id FROM " << this->GetDocInfo3D().GetTableName(IDbDocInfo3D::TableType::TilesSpatialIndex) << " WHERE id MATCH " << CCustomQueries::GetQueryFunctionName(CCustomQueries::Query::RTree_PlaneAabb3D) << "(?1,?2,?3,?4)";
    SQLite::Statement query(this->GetDb(), ss.str());
    query.bind(1, plane.normal.x);
    query.bind(2, plane.normal.y);
    query.bind(3, plane.normal.z);
    query.bind(4, plane.distance);

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
    }
}

/*virtual*/void CDbRead3D::ReadPerTileData(SlImgDoc::dbIndex idx, const std::vector<std::string>& columns, std::function<bool(const SlImgDoc::KeyVariadicValuePair&)> func)
{
    struct PerTileDataAdapter
    {
        const CDbRead3D& r;
        PerTileDataAdapter(const CDbRead3D& r) :r(r) {}

        const string& GetTableName() const { return this->r.GetDocInfo3D().GetTableName(IDbDocInfo3D::TableType::PerBrickData); }
        const string& GetPkColumnName() const { return this->r.GetDocInfo3D().GetPerTilesDataColumnName(IDbDocInfo3D::PerTileDataColumn::Pk); }
        const std::vector<ColumnTypeAllInfo>& GetPerTileDataColumnInfo() const { return this->r.GetDocInfo3D().GetCoordinateDataColumnInfo(); }
    };

    PerTileDataAdapter adapter(*this);
    this->ReadPerTileDataCommon(
        adapter,
        idx,
        columns, func);
}

/*virtual*/void CDbRead3D::EnumPerTileColumns(const std::function<bool(const SlImgDoc::ColumnDescription&)>& func)
{
    const auto& perTileDataColumnInfo = this->GetDocInfo3D().GetCoordinateDataColumnInfo();
    this->EnumPerTilesColumns(perTileDataColumnInfo.cbegin(), perTileDataColumnInfo.cend(), func);
    /*
    for (const auto i : perTileDataColumnInfo)
    {
        ColumnDescription cd;
        cd.Name = i.columnName;
        cd.DataType = DbUtils::ColumnTypeInfoToStringRepresentation(i);
        bool b = func(cd);
        if (!b)
        {
            break;
        }
    }*/
}