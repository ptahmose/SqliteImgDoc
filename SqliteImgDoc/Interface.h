#pragma once

#include <string>
#include <unordered_set>
#include "ImportExport.h"
#include "Types.h"
#include "LogicalPositionInfo.h"
#include "IDataObj.h"
#include "ITileCoordinate.h"
#include "TileCoordinate.h"
#include "IBlob.h"
#include "BlobOnHeap.h"
#include "IDimCoordinateQueryClause.h"
#include "DimCoordinateQueryClause.h"

namespace SlImgDoc
{
    typedef long long dbIndex;

    class CreateOptions
    {
    public:
        std::string dbFilename;

        std::unordered_set<char> dimensions;
    };

    class OpenOptions
    {
    public:
        std::string dbFilename;
    };

    class SQLITEIMGDOC_API IDbWrite
    {
    public:
        virtual void BeginTransaction() = 0;
        virtual void CommitTransaction() = 0;
        virtual void RollbackTransaction() = 0;

        virtual void AddSubBlock(const ITileCoordinate* coord, const LogicalPositionInfo* info, const IDataObjUncompressedBitmap* data) = 0;

        virtual ~IDbWrite() {};
    };

    struct TilePixelInfo
    {
        int pixelWidth;
        int pixelHeight;
        std::uint8_t pixelType;
        int dataType;
        std::uint8_t dataBinHdr[32];
    };

    class SQLITEIMGDOC_API IDbRead
    {
    public:
        virtual void ReadTileInfo(dbIndex idx, SlImgDoc::TileCoordinate* coord, LogicalPositionInfo* info) = 0;
        virtual void ReadTileData(dbIndex ix, TilePixelInfo* pixelInfo, IBlob* data) = 0;

        virtual void Query(const IDimCoordinateQueryClause* clause, std::function<bool(dbIndex)> func) = 0;

        virtual void GetTilesIntersectingRect(const RectangleD& rect, std::function<bool(dbIndex)> func) = 0;
        virtual void GetTilesIntersectingWithLine(const LineThruTwoPointsD& line, std::function<bool(dbIndex)> func) = 0;

        std::vector<dbIndex> GetTilesIntersectingRect(const RectangleD& rect);
        std::vector<dbIndex> GetTilesIntersectingWithLine(const LineThruTwoPointsD& line);
        std::vector<dbIndex>  Query(const IDimCoordinateQueryClause* clause);

        virtual ~IDbRead() {};
    };


    class SQLITEIMGDOC_API IDbFactory
    {
    public:
        static IDbWrite* CreateNew(const CreateOptions& opts);
        static IDbRead* OpenExisting(const OpenOptions& opts);
    };
}