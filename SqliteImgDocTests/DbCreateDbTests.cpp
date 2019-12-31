#include "pch.h"

using namespace std;
using namespace SlImgDoc;


TEST(DbCreateDbTests, ParameterValidationTest1)
{
    CreateOptions opts;
    opts.dbFilename = ":memory:";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.sizeOfDataBinHdrField = -1;
    EXPECT_THROW(IDbFactory::CreateNew(opts), SlImgDoc::SqliteImgDocInvalidArgumentException) << "An exception was expected here.";

    opts.sizeOfDataBinHdrField = 0;
    EXPECT_THROW(IDbFactory::CreateNew(opts), SlImgDoc::SqliteImgDocInvalidArgumentException) << "An exception was expected here.";
}

TEST(DbCreateDbTests, InvalidSizeOfDataBinHdr1)
{
    CreateOptions opts;
    opts.dbFilename = ":memory:";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.sizeOfDataBinHdrField = 18;
    auto db = IDbFactory::CreateNew(opts);

    auto dbWrite = db->GetWriter();

    LogicalPositionInfo posInfo;
    posInfo.width = 100;
    posInfo.height = 100;
    posInfo.pyrLvl = 0;
    TileBaseInfo tileBaseInfo;
    tileBaseInfo.pixelWidth = 100;
    tileBaseInfo.pixelHeight = 100;
    tileBaseInfo.pixelType = PixelType::GRAY8;

    TileCoordinate tc({ { 'C',0 },{ 'Z',0 } });
    CDataObjCustom dataCustom(19, 1);

    EXPECT_THROW(dbWrite->AddTile(&tc, &posInfo, &tileBaseInfo, &dataCustom), SlImgDoc::SqliteImgDocInvalidArgumentException) << "An exception was expected here.";
}

TEST(DbCreateDbTests, InvalidSizeOfDataBinHdr2)
{
    CreateOptions opts;
    opts.dbFilename = ":memory:";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.sizeOfDataBinHdrField = 18;
    auto db = IDbFactory::CreateNew3D(opts);

    auto dbWrite = db->GetWriter3D();

    LogicalPositionInfo3D posInfo;
    posInfo.width = 100;
    posInfo.height = 100;
    posInfo.depth = 100;
    posInfo.pyrLvl = 0;
    TileBaseInfo3D tileBaseInfo;
    tileBaseInfo.pixelWidth = 100;
    tileBaseInfo.pixelHeight = 100;
    tileBaseInfo.pixelDepth = 100;
    tileBaseInfo.pixelType = PixelType::GRAY8;

    TileCoordinate tc({ { 'C',0 },{ 'Z',0 } });
    CDataObjCustom dataCustom(19, 1);

    EXPECT_THROW(dbWrite->AddBrick(&tc, &posInfo, &tileBaseInfo, &dataCustom), SlImgDoc::SqliteImgDocInvalidArgumentException) << "An exception was expected here.";
}

TEST(DbCreateDbTests, InsufficientCoordinate1)
{
    CreateOptions opts;
    opts.dbFilename = ":memory:";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.sizeOfDataBinHdrField = 4;
    auto db = IDbFactory::CreateNew(opts);

    auto dbWrite = db->GetWriter();

    LogicalPositionInfo posInfo;
    posInfo.width = 100;
    posInfo.height = 100;
    posInfo.pyrLvl = 0;
    TileBaseInfo tileBaseInfo;
    tileBaseInfo.pixelWidth = 100;
    tileBaseInfo.pixelHeight = 100;
    tileBaseInfo.pixelType = PixelType::GRAY8;

    TileCoordinate tc({ { 'C',0 } });
    CDataObjCustom dataCustom(19, 1);

    EXPECT_THROW(dbWrite->AddTile(&tc, &posInfo, &tileBaseInfo, &dataCustom), SlImgDoc::SqliteImgDocInvalidArgumentException) << "An exception was expected here.";
}

TEST(DbCreateDbTests, InsufficientCoordinate2)
{
    CreateOptions opts;
    opts.dbFilename = ":memory:";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.sizeOfDataBinHdrField = 4;
    auto db = IDbFactory::CreateNew(opts);

    auto dbWrite = db->GetWriter();

    LogicalPositionInfo posInfo;
    posInfo.width = 100;
    posInfo.height = 100;
    posInfo.pyrLvl = 0;
    TileBaseInfo tileBaseInfo;
    tileBaseInfo.pixelWidth = 100;
    tileBaseInfo.pixelHeight = 100;
    tileBaseInfo.pixelType = PixelType::GRAY8;

    TileCoordinate tc({ { 'C',0 },{'X',2} });
    CDataObjCustom dataCustom(19, 1);

    EXPECT_THROW(dbWrite->AddTile(&tc, &posInfo, &tileBaseInfo, &dataCustom), SlImgDoc::SqliteImgDocInvalidArgumentException) << "An exception was expected here.";
}