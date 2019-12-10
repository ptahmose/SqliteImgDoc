#include "pch.h"
#include "DbRead.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include "CCustomQueries.h"
//#define SQLITE_ENABLE_RTREE
#include <sqlite3.h>
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

std::vector<dbIndex> IDbRead:: Query(const IDimCoordinateQueryClause* clause)
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


/*virtual*/void CDbRead::GetTilesIntersectingRect(const RectangleD& rect, std::function<bool(dbIndex)> func)
{
    stringstream ss;    //ss << "SELECT Pixelwidth,Pixelheight,Pixeltype,Datatype,Data_BinHdr,Data FROM TILESDATA WHERE rowId=(SELECT TileDataId FROM TILESINFO WHERE Pk=?1);";
    ss << "SELECT " << this->docInfo->GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::Pk) << " FROM "
        << this->docInfo->GetTableName(IDbDocInfo::TableType::TilesSpatialIndex) << " WHERE " <<
        this->docInfo->GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MaxX) << ">=?1 AND " <<
        this->docInfo->GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MinX) << "<=?2 AND " <<
        this->docInfo->GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MaxY) << ">=?3 AND " <<
        this->docInfo->GetTilesSpatialIndexColumnName(IDbDocInfo::TilesSpatialIndexColumn::MinY) << "<=?4";

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


//static void LineThruTwoPoints_del(void* p)
//{
//    sqlite3_free(p);
//}

/*bool intersect(const LineThruTwoPoints* line,    const RectangleF* box)
{
    double st, et, fst = 0, fet = 1;
    double const* bmin = &box.min.x;
    F32 const* bmax = &box.max.x;
    F32 const* si = &start.x;
    F32 const* ei = &end.x;

    for (int i = 0; i < 3; i++) {
        if (*si < *ei) {
            if (*si > * bmax || *ei < *bmin)
                return false;
            F32 di = *ei - *si;
            st = (*si < *bmin) ? (*bmin - *si) / di : 0;
            et = (*ei > * bmax) ? (*bmax - *si) / di : 1;
        }
        else {
            if (*ei > * bmax || *si < *bmin)
                return false;
            F32 di = *ei - *si;
            st = (*si > * bmax) ? (*bmax - *si) / di : 0;
            et = (*ei < *bmin) ? (*bmin - *si) / di : 1;
        }

        if (st > fst) fst = st;
        if (et < fet) fet = et;
        if (fet < fst)
            return false;
        bmin++; bmax++;
        si++; ei++;
    }

    *time = fst;
    return true;
}*/

//// a1 is line1 start, a2 is line1 end, b1 is line2 start, b2 is line2 end
//static bool Intersects(const PointF& a1, const PointF& a2, const PointF& b1, const PointF& b2)
//{
//    PointF b{ a2.x - a1.x,a2.y - a1.y };
//    PointF d{ b2.x - b1.x,b2.y - b1.y };
//
//    float bDotDPerp = b.x * d.y - b.y * d.x;
//
//    // if b dot d == 0, it means the lines are parallel so have infinite intersection points
//    if (bDotDPerp == 0)
//        return false;
//
//    PointF c{ b1.x - a1.x ,b1.y - a1.y };// = b1 - a1;
//    float t = (c.x * d.y - c.y * d.x) / bDotDPerp;
//    if (t < 0 || t > 1)
//        return false;
//
//    float u = (c.x * b.y - c.y * b.x) / bDotDPerp;
//    if (u < 0 || u > 1)
//        return false;
//
//    return true;
//}

//static bool IsPointInsideRect(const PointF& p, const RectangleF& r)
//{
//    if (r.x <= p.x && (r.x + r.w) >= p.x && r.y <= p.y && (r.y + r.h) >= p.y)
//    {
//        return true;
//    }
//
//    return false;
//}

//static bool DoesIntersect(const LineThruTwoPoints* line, double xmin, double xmax, double ymin, double ymax)
//{
//    PointF bottomLeft{ xmin,ymin };
//    PointF topRight{ xmax,ymax };
//    PointF bottomRight{ xmax,ymin };
//    PointF topLeft{ xmin,ymax };
//    RectangleF rect{ xmin,ymin,xmax - xmin,ymax - ymin };
//    return
//        IsPointInsideRect(line->a, rect) ||
//        IsPointInsideRect(line->b, rect) ||
//        Intersects(line->a, line->b, bottomLeft, topRight) ||
//        Intersects(line->a, line->b, bottomRight, topLeft);
//}

//static int VisibilityTest(const LineThruTwoPoints* line, double xmin, double xmax, double ymin, double ymax)
//{
//    PointF bottomLeft{ xmin,ymin };
//    PointF topRight{ xmax,ymax };
//    PointF bottomRight{ xmax,ymin };
//    PointF topLeft{ xmin,ymax };
//    RectangleF rect{ xmin,ymin,xmax - xmin,ymax - ymin };
//
//    bool b = IsPointInsideRect(line->a, rect);
//    if (b == true)
//    {
//        if (IsPointInsideRect(line->b, rect))
//        {
//            return FULLY_WITHIN;
//        }
//        else
//        {
//            return PARTLY_WITHIN;
//        }
//    }
//
//    if (Intersects(line->a, line->b, bottomLeft, topRight) ||
//        Intersects(line->a, line->b, bottomRight, topLeft))
//    {
//        return PARTLY_WITHIN;
//    }
//
//    return NOT_WITHIN;
//}

//static int linethru2points_geom(
//    sqlite3_rtree_geometry* p,
//    int nCoord,
//    sqlite3_rtree_dbl* aCoord,
//    int* pRes)
//{
//    LineThruTwoPoints* pLine = (LineThruTwoPoints*)p->pUser;
//    if (pLine == nullptr)
//    {
//        /* If pUser is still 0, then the parameter values have not been tested
//        ** for correctness or stored into a "LineThruTwoPoints" structure yet. Do this now. */
//
//        /* This geometry callback is for use with a 2-dimensional r-tree table.
//        ** Return an error if the table does not have exactly 2 dimensions. */
//        if (nCoord != 4) return SQLITE_ERROR;
//
//        /* Test that the correct number of parameters (4) have been supplied,
//        */
//        if (p->nParam != 4) return SQLITE_ERROR;
//
//        /*Allocate a structure to cache parameter data in.Return SQLITE_NOMEM
//        ** if the allocation fails.*/
//        pLine = (LineThruTwoPoints*)(p->pUser = sqlite3_malloc(sizeof(LineThruTwoPoints)));
//        if (!pLine) return SQLITE_NOMEM;
//
//        p->xDelUser = LineThruTwoPoints_del;
//        pLine->a.x = p->aParam[0];
//        pLine->a.y = p->aParam[1];
//        pLine->b.x = p->aParam[2];
//        pLine->b.y = p->aParam[3];
//    }
//
//    double xmin{ aCoord[0] };
//    double xmax{ aCoord[1] };              /* X dimensions of box being tested */
//    double ymin{ aCoord[2] };
//    double ymax{ aCoord[3] };              /* Y dimensions of box being tested */
//
//    bool intersect = DoesIntersect(pLine, xmin, xmax, ymin, ymax);
//
//    *pRes = intersect == true ? 1 : 0;
//
//    return SQLITE_OK;
//}

//static int linethru2points_query(sqlite3_rtree_query_info* info)
//{
//    LineThruTwoPoints* pLine = (LineThruTwoPoints*)info->pUser;
//    if (pLine == nullptr)
//    {
//        /* If pUser is still 0, then the parameter values have not been tested
//        ** for correctness or stored into a "LineThruTwoPoints" structure yet. Do this now. */
//
//        /* This geometry callback is for use with a 2-dimensional r-tree table.
//        ** Return an error if the table does not have exactly 2 dimensions. */
//        if (info->nCoord != 4) return SQLITE_ERROR;
//
//        /* Test that the correct number of parameters (4) have been supplied,
//        */
//        if (info->nParam != 4) return SQLITE_ERROR;
//
//        /*Allocate a structure to cache parameter data in.Return SQLITE_NOMEM
//        ** if the allocation fails.*/
//        pLine = (LineThruTwoPoints*)(info->pUser = sqlite3_malloc(sizeof(LineThruTwoPoints)));
//        if (!pLine) return SQLITE_NOMEM;
//
//        info->xDelUser = LineThruTwoPoints_del;
//        pLine->a.x = info->aParam[0];
//        pLine->a.y = info->aParam[1];
//        pLine->b.x = info->aParam[2];
//        pLine->b.y = info->aParam[3];
//    }
//
//    double xmin{ info->aCoord[0] };
//    double xmax{ info->aCoord[1] };              /* X dimensions of box being tested */
//    double ymin{ info->aCoord[2] };
//    double ymax{ info->aCoord[3] };              /* Y dimensions of box being tested */
//
//    info->eWithin = VisibilityTest(pLine, xmin, xmax, ymin, ymax);
//    info->rScore = info->iLevel;
//
//    return SQLITE_OK;
//}

/*virtual*/void CDbRead::GetTilesIntersectingWithLine(const LineThruTwoPointsD& rect, std::function<bool(dbIndex)> func)
{
    //auto rc = sqlite3_rtree_geometry_callback(
    //    this->GetDb().getHandle(),
    //    "LineThroughPoints",
    //    linethru2points_geom,
    //    0);

    //auto rc = sqlite3_rtree_query_callback(
    //    this->GetDb().getHandle(),
    //    "LineThroughPoints",
    //    linethru2points_query,
    //    0, 0);

    stringstream ss;
    //ss << "SELECT id FROM TILESPATIAL_index WHERE id MATCH LineThroughPoints(?1,?2,?3,?4)";
    ss << "SELECT id FROM " << this->docInfo->GetTableName(IDbDocInfo::TableType::TilesSpatialIndex) << " WHERE id MATCH " << CCustomQueries::GetQueryFunctionName(CCustomQueries::Query::RTree_LineSegment2D) << "(?1,?2,?3,?4)";
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
    }
}

/*virtual*/void CDbRead::Query(const SlImgDoc::IDimCoordinateQueryClause* clause, std::function<bool(dbIndex)> func)
{
    stringstream ss;
    ss << "SELECT "<< this->docInfo->GetTileInfoColumnName(IDbDocInfo::TilesInfoColumn::Pk)<<" FROM " << this->docInfo->GetTableName(IDbDocInfo::TableType::TilesInfo) << " WHERE ";

    auto rangeDims = clause->GetTileDimsForRangeClause();
    for (const auto dim : rangeDims)
    {
        auto ranges = clause->GetRangeClause(dim);
        string dimColumnName;
        this->docInfo->GetTileInfoColumnNameForDimension(dim, dimColumnName);

        if (!ranges.empty())
        {
            ss << "(";
            bool first = true;
            for (const auto& r : ranges)
            {
                if (!first)
                {
                    ss << " OR ";
                }

                ss << "(" << dimColumnName << ">=" << r.start << " AND " << dimColumnName << "<=" << r.end << ")";
                first = false;
            }

            ss << ")";
        }
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
