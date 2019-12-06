#pragma once

#include <string>
#include <unordered_set>
#include "ImportExport.h"
//#include "SubBlkCoordinate.h"
#include "LogicalPositionInfo.h"
#include "IDataObj.h"
#include "ITileCoordinate.h"
#include "TileCoordinate.h"

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

    class SQLITEIMGDOC_API IDbRead
    {
    public:
        virtual void ReadTileInfo(dbIndex idx, SlImgDoc::TileCoordinate* coord, LogicalPositionInfo* info) = 0;
    };


    class SQLITEIMGDOC_API IDbFactory
    {
    public:
        static IDbWrite* CreateNew(const CreateOptions& opts);
        static IDbRead* OpenExisting(const OpenOptions& opts);
    };
}