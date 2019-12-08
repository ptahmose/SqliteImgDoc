#pragma once

#include <sqlite3.h>

/// Custom Queries are found here - here we register custom extensions.
/// Currently, we extend R-Tree queries (https://sqlite.org/rtree.html).
class CCustomQueries
{
public:
    enum class Query
    {
        RTree_LineSegment2D
    };

    void SetupCustomQueries(sqlite3* db);
};
