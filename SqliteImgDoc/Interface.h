#pragma once

#include <string>
#include <unordered_set>
#include "ImportExport.h"
#include "SubBlkCoordinate.h"
#include "LogicalPositionInfo.h"
#include "IDataObj.h"
#include "ITileCoordinate.h"
#include "TileCoordinate.h"

namespace SlImgDoc
{
    class CreateOptions
    {
    public:
        std::string dbFilename;

        std::unordered_set<char> dimensions;
    };

    class SQLITEIMGDOC_API IDbWrite
    {
    public:
        virtual void BeginTransaction()=0;
        virtual void CommitTransaction()=0;
        virtual void AddSubBlock(const ISubBlkCoordinate* coord, const LogicalPositionInfo* info, const IDataObjUncompressedBitmap* data) = 0;

        virtual ~IDbWrite() {};
    };


    class SQLITEIMGDOC_API IDbFactory
    {
    public:
        static IDbWrite* CreateNew(const CreateOptions& opts);
    };


}