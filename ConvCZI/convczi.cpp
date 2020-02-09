#include "cmdlineopts.h"
#include <iostream>
#include "../SqliteImgDoc/external/Interface.h"
#include "inc_libczi.h"
#include "utils.h"

using namespace SlImgDoc;
using namespace std;
using namespace libCZI;

static int CalcPyramidLayerNo(const libCZI::IntRect& logicalRect, const libCZI::IntSize& physicalSize, int minificationFactorPerLayer)
{
    double minFactor;
    if (physicalSize.w > physicalSize.h)
    {
        minFactor = double(logicalRect.w) / physicalSize.w;
    }
    else
    {
        minFactor = double(logicalRect.h) / physicalSize.h;
    }

    int minFactorInt = (int)round(minFactor);
    int f = 1;
    int layerNo = -1;
    for (int layer = 0;; layer++)
    {
        if (f >= minFactorInt)
        {
            layerNo = layer;
            break;
        }

        f *= minificationFactorPerLayer;
    }

    return layerNo;
}

static void ConvertDimCoordinate(const CDimCoordinate& dimCoordinate, TileCoordinate& tc)
{
    dimCoordinate.EnumValidDimensions(
        [&](DimensionIndex dim, int v)->bool
    {
        if (dim != DimensionIndex::B)
        {
            tc.Set(Utils::DimensionToChar(dim), v);
        }

        return true;
    });
}

static void Convert(const SubBlockInfo& sbBlkInfo, TileBaseInfo& tileBaseInfo)
{
    switch (sbBlkInfo.pixelType)
    {
    case libCZI::PixelType::Bgr24:
        tileBaseInfo.pixelType = ::PixelType::BGR24;
        break;
    case libCZI::PixelType::Bgr48:
        tileBaseInfo.pixelType = ::PixelType::BGR48;
        break;
    case libCZI::PixelType::Gray8:
        tileBaseInfo.pixelType = ::PixelType::GRAY8;
        break;
    case libCZI::PixelType::Gray16:
        tileBaseInfo.pixelType = ::PixelType::GRAY16;
        break;
    default:
        tileBaseInfo.pixelType = 0;
        break;
    }

    tileBaseInfo.pixelWidth = sbBlkInfo.physicalSize.w;
    tileBaseInfo.pixelHeight = sbBlkInfo.physicalSize.h;
}

class DataObjOnSubBlk :public IDataObjBase
{
private:
    const shared_ptr<ISubBlock>& sbBlk;
    BinHdrUncompressedBitmap hdr;
    DataTypes dataType;
public:
    DataObjOnSubBlk(const shared_ptr<ISubBlock>& sbblk) :sbBlk(sbblk)
    {
        hdr.width = this->sbBlk->GetSubBlockInfo().physicalSize.w;
        hdr.height = this->sbBlk->GetSubBlockInfo().physicalSize.h;
        hdr.stride = this->sbBlk->GetSubBlockInfo().physicalSize.w * 2;
        switch (this->sbBlk->GetSubBlockInfo().mode)
        {
        case CompressionMode::UnCompressed:
            this->dataType = DataTypes::UNCOMPRESSED_BITMAP;
            break;
        case CompressionMode::JpgXr:
            this->dataType = DataTypes::JPGXRCOMPRESSED_BITMAP;
            break;
        default:
            throw invalid_argument("Unknown mode");
        }
    }

    DataTypes GetDataType() const { return this->dataType; }

    virtual void GetData(const void** p, size_t* s) const
    {
        const void* ptr; size_t size;
        this->sbBlk->DangerousGetRawData(ISubBlock::Data, ptr, size);
        *p = ptr;
        *s = size;
    }

    virtual void GetHeader(const void** p, size_t* s) const
    {
        *p = &this->hdr;
        *s = sizeof(this->hdr);
    }
};

int main(int argc, char** argv)
{
    CmdlineOpts opts;
    if (!opts.ParseArguments(argc, argv))
    {
        return EXIT_FAILURE;
    }

    auto stream = CreateStreamFromFile(CUtils::convertUtf8ToUCS2(opts.GetCziFilename()).c_str());
    auto czireader = CreateCZIReader();
    czireader->Open(stream);
    auto subBlkStatistics = czireader->GetStatistics();
    auto pyrInfo = czireader->GetPyramidStatistics();

    CreateOptions createOpts;
    createOpts.SetDefaultValues();
    createOpts.dbFilename = opts.GetDstFilename();

    subBlkStatistics.dimBounds.EnumValidDimensions(
        [&](DimensionIndex dim, int start, int end)->bool
    {
        if (dim != DimensionIndex::B)
        {
            char d = Utils::DimensionToChar(dim);
            createOpts.dimensions.emplace(d);
        }

        return true;
    });

    TileCoordinate tc;
    TileBaseInfo tileBaseInfo;
    LogicalPositionInfo logicalPosInfo;
    auto db = IDbFactory::CreateNew(createOpts);
    auto dbWrite = db->GetWriter();

    dbWrite->BeginTransaction();
    czireader->EnumerateSubBlocks(
        [&](int idx, const SubBlockInfo& info)->bool
    {
        ConvertDimCoordinate(info.coordinate, tc);

        logicalPosInfo.posX = info.logicalRect.x;
        logicalPosInfo.posY = info.logicalRect.y;
        logicalPosInfo.width = info.logicalRect.w;
        logicalPosInfo.height = info.logicalRect.h;
        logicalPosInfo.pyrLvl = CalcPyramidLayerNo(info.logicalRect, info.physicalSize, 2);

        Convert(info, tileBaseInfo);

        auto sbBlk = czireader->ReadSubBlock(idx);

        DataObjOnSubBlk dataobj(sbBlk);
        dbWrite->AddTile(&tc, &logicalPosInfo, &tileBaseInfo, dataobj.GetDataType(), &dataobj);


        return true;
    });

    dbWrite->CommitTransaction();
}