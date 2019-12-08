#pragma once

#include <string>
#include <sqlite3.h>
#include "Types.h"

/// Custom Queries are found here - here we register custom extensions.
/// Currently, we extend R-Tree queries (https://sqlite.org/rtree.html).
class CCustomQueries
{
private:
    static const std::string queryFunctionName_rtree_linesegment2d;
public:
    enum class Query
    {
        RTree_LineSegment2D
    };

    static void SetupCustomQueries(sqlite3* db);

    static const std::string& GetQueryFunctionName(Query q);
private:
    static int LineThrough2Points2d_Query(sqlite3_rtree_query_info* info);
    static void Free_LineThruTwoPointsD(void* p);

    static bool DoLinesIntersect(const SlImgDoc::PointD& a1, const SlImgDoc::PointD& a2, const SlImgDoc::PointD& b1, const SlImgDoc::PointD& b2);
};
