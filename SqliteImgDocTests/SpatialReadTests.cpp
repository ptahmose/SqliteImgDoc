#include "pch.h"

using namespace std;
using namespace SlImgDoc;

static shared_ptr<IDb> CreateMosaicTestDatabase(int rows, int columns, int sizeX, int sizeY)
{
    // create a database with dimension 'M' only
    // place tiles on in a checkerboard arrangement

    CreateOptions opts;
    opts.dbFilename = ":memory:";
    //opts.dbFilename = "D:\\unittest.db";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.dimensions.emplace('T');
    opts.dimensions.emplace('M');

    auto db = IDbFactory::CreateNew(opts);

    auto dbWrite = db->GetWriter();

    dbWrite->BeginTransaction();

    LogicalPositionInfo posInfo;
    posInfo.width = sizeX;
    posInfo.height = sizeY;
    posInfo.pyrLvl = 0;
    TileBaseInfo tileBaseInfo;
    tileBaseInfo.pixelWidth = sizeX;
    tileBaseInfo.pixelHeight = sizeY;
    tileBaseInfo.pixelType = PixelType::GRAY8;

    CDataObjCustom dataCustom(16, 1);

    int m = 0;
    TileCoordinate tc({ { 'M',0 } });

    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < columns; ++c)
        {
            posInfo.posX = c * sizeX;
            posInfo.posY = r * sizeY;

            tc.Set('M', m);
            dbWrite->AddTile(&tc, &posInfo, &tileBaseInfo, &dataCustom);
            m++;
        }
    }

    dbWrite->CommitTransaction();
    return db;
}

TEST(SpatialReadTests, QueryRect1)
{
    auto db = CreateMosaicTestDatabase(50, 50, 256, 256);
    auto reader = db->GetReader();
    RectangleD rect;
    rect.x = 100; rect.y = 100; rect.w = 20; rect.h = 1100;
    auto r = reader->GetTilesIntersectingRect(rect);
    ASSERT_TRUE(r.size() == 5) << "Expected exactly five results.";
}