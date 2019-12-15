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

static bool CheckIfSetsAreEqual(std::vector<LogicalPositionInfo>& a, std::vector<LogicalPositionInfo>& b)
{
    if (a.size()!=b.size())
    {
        return false;
    }

    for (size_t i=0;i<a.size();++i)
    {
        auto f = find(a.cbegin(), a.cend(), b[i]);
        if (f==a.cend())
        {
            return false;
        }
    }

    return true;
}

TEST(SpatialReadTests, QueryRect1)
{
    auto db = CreateMosaicTestDatabase(50, 50, 256, 256);
    auto reader = db->GetReader();
    RectangleD rect;
    rect.x = 100; rect.y = 100; rect.w = 20; rect.h = 1100;
    auto r = reader->GetTilesIntersectingRect(rect);
    ASSERT_TRUE(r.size() == 5) << "Expected exactly five results.";

    std::vector<LogicalPositionInfo> logPositions;
    for (size_t i = 0; i < r.size(); ++i)
    {
        LogicalPositionInfo logPos;
        reader->ReadTileInfo(r[i], nullptr, &logPos);

        logPositions.push_back(logPos);
    }

    std::vector<LogicalPositionInfo> expected;
    expected.push_back(LogicalPositionInfo{ 0,0,256,256,0 });
    expected.push_back(LogicalPositionInfo{ 0,256,256,256,0 });
    expected.push_back(LogicalPositionInfo{ 0,512,256,256,0 });
    expected.push_back(LogicalPositionInfo{ 0,768,256,256,0 });
    expected.push_back(LogicalPositionInfo{ 0,1024,256,256,0 });

    bool correct = CheckIfSetsAreEqual(logPositions, expected);
    EXPECT_TRUE(correct) << "did not get the expected tiles";
}

TEST(SpatialReadTests, QueryLine1)
{
    auto db = CreateMosaicTestDatabase(50, 50, 256, 256);
    auto reader = db->GetReader();
   
    LineThruTwoPointsD line;
    line.a.x = 0; line.a.y = 0;
    line.b.x = 128; line.b.y = 640;
    auto r = reader->GetTilesIntersectingWithLine(line);
    ASSERT_TRUE(r.size() == 3) << "Expected exactly five results.";

    std::vector<LogicalPositionInfo> logPositions;
    for (size_t i = 0; i < r.size(); ++i)
    {
        LogicalPositionInfo logPos;
        reader->ReadTileInfo(r[i], nullptr, &logPos);

        logPositions.push_back(logPos);
    }

    std::vector<LogicalPositionInfo> expected;
    expected.push_back(LogicalPositionInfo{ 0,0,256,256,0 });
    expected.push_back(LogicalPositionInfo{ 0,256,256,256,0 });
    expected.push_back(LogicalPositionInfo{ 0,512,256,256,0 });

    bool correct = CheckIfSetsAreEqual(logPositions, expected);
    EXPECT_TRUE(correct) << "did not get the expected tiles";
}