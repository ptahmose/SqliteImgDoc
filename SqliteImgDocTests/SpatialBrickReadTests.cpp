#include "pch.h"

using namespace std;
using namespace SlImgDoc;

static shared_ptr<IDb> CreateBrickTestDatabase(int rows, int columns, int stacks, int sizeX, int sizeY, int sizeZ)
{
    // create a database with dimension 'M' only
    // place tiles on in a checkerboard arrangement

    CreateOptions opts;
    opts.dbFilename = ":memory:";
    //opts.dbFilename = "E:\\unittest3d.db";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.dimensions.emplace('T');
    opts.dimensions.emplace('M');

    auto db = IDbFactory::CreateNew3D(opts);

    auto dbWrite = db->GetWriter3D();

    dbWrite->BeginTransaction();

    LogicalPositionInfo3D posInfo;
    posInfo.width = sizeX;
    posInfo.height = sizeY;
    posInfo.depth = sizeZ;
    posInfo.pyrLvl = 0;
    TileBaseInfo3D tileBaseInfo;
    tileBaseInfo.pixelWidth = sizeX;
    tileBaseInfo.pixelHeight = sizeY;
    tileBaseInfo.pixelDepth = sizeZ;
    tileBaseInfo.pixelType = PixelType::GRAY8;

    CDataObjCustom dataCustom(16, 1);

    int m = 0;
    TileCoordinate tc({ { 'M',0 } });

    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < columns; ++c)
        {
            for (int z = 0; z < stacks; ++z)
            {
                posInfo.posX = c * sizeX;
                posInfo.posY = r * sizeY;
                posInfo.posZ = z * sizeZ;

                tc.Set('M', m);
                dbWrite->AddBrick(&tc, &posInfo, &tileBaseInfo, &dataCustom);
                m++;
            }
        }
    }

    dbWrite->CommitTransaction();
    return db;
}

static bool CheckIfSetsAreEqual(std::vector<LogicalPositionInfo3D>& a, std::vector<LogicalPositionInfo3D>& b)
{
    if (a.size() != b.size())
    {
        return false;
    }

    for (size_t i = 0; i < a.size(); ++i)
    {
        auto f = find(a.cbegin(), a.cend(), b[i]);
        if (f == a.cend())
        {
            return false;
        }
    }

    return true;
}

TEST(SpatialBrickReadTests, QueryCuboid1)
{
    auto db = CreateBrickTestDatabase(10, 10, 10, 256, 256, 256);

    auto reader = db->GetReader3D();
    CuboidD cuboid;
    cuboid.x = 100; cuboid.y = 100; cuboid.w = 20; cuboid.h = 1100; cuboid.z = 0; cuboid.d = 10;
    auto r = reader->GetTilesIntersectingCuboid(cuboid);
    ASSERT_TRUE(r.size() == 5) << "Expected exactly five results.";

    std::vector<LogicalPositionInfo3D> logPositions;
    for (size_t i = 0; i < r.size(); ++i)
    {
        LogicalPositionInfo3D logPos;
        reader->ReadTileInfo(r[i], nullptr, &logPos);

        logPositions.push_back(logPos);
    }

    std::vector<LogicalPositionInfo3D> expected;
    expected.push_back(LogicalPositionInfo3D{ 0,0,0,256,256,256,0 });
    expected.push_back(LogicalPositionInfo3D{ 0,256,0,256,256,256,0 });
    expected.push_back(LogicalPositionInfo3D{ 0,512,0,256,256,256,0 });
    expected.push_back(LogicalPositionInfo3D{ 0,768,0,256,256,256,0 });
    expected.push_back(LogicalPositionInfo3D{ 0,1024,0,256,256,256,0 });

    bool correct = CheckIfSetsAreEqual(logPositions, expected);
    EXPECT_TRUE(correct) << "did not get the expected tiles";
}

TEST(SpatialBrickReadTests, QueryCuboid2)
{
    auto db = CreateBrickTestDatabase(10, 10, 10, 256, 256, 256);

    auto reader = db->GetReader3D();
    CuboidD cuboid;
    cuboid.x = 100; cuboid.y = 100; cuboid.w = 20; cuboid.h = 1100; cuboid.z = 0; cuboid.d = 500;
    auto r = reader->GetTilesIntersectingCuboid(cuboid);
    ASSERT_TRUE(r.size() == 5 * 2) << "Expected exactly five results.";

    std::vector<LogicalPositionInfo3D> logPositions;
    for (size_t i = 0; i < r.size(); ++i)
    {
        LogicalPositionInfo3D logPos;
        reader->ReadTileInfo(r[i], nullptr, &logPos);

        logPositions.push_back(logPos);
    }

    std::vector<LogicalPositionInfo3D> expected;
    expected.push_back(LogicalPositionInfo3D{ 0,0,0,256,256,256,0 });
    expected.push_back(LogicalPositionInfo3D{ 0,256,0,256,256,256,0 });
    expected.push_back(LogicalPositionInfo3D{ 0,512,0,256,256,256,0 });
    expected.push_back(LogicalPositionInfo3D{ 0,768,0,256,256,256,0 });
    expected.push_back(LogicalPositionInfo3D{ 0,1024,0,256,256,256,0 });
    expected.push_back(LogicalPositionInfo3D{ 0,0,256,256,256,256,0 });
    expected.push_back(LogicalPositionInfo3D{ 0,256,256,256,256,256,0 });
    expected.push_back(LogicalPositionInfo3D{ 0,512,256,256,256,256,0 });
    expected.push_back(LogicalPositionInfo3D{ 0,768,256,256,256,256,0 });
    expected.push_back(LogicalPositionInfo3D{ 0,1024,256,256,256,256,0 });

    bool correct = CheckIfSetsAreEqual(logPositions, expected);
    EXPECT_TRUE(correct) << "did not get the expected tiles";
}

TEST(SpatialBrickReadTests, QueryPlane1)
{
    auto db = CreateBrickTestDatabase(10, 10, 10, 256, 256, 256);

    auto reader = db->GetReader3D();
    //Plane_NormalAndDistD plane{ Vector3dD{1,1,-1},5 * 256 };
    Plane_NormalAndDistD plane{ Vector3dD{0,0,1},128 };
    auto r = reader->GetTilesIntersectingWithPlane(plane);
    ASSERT_TRUE(r.size() == 100) << "Expected exactly 100 results.";

    std::vector<LogicalPositionInfo3D> logPositions;
    for (size_t i = 0; i < r.size(); ++i)
    {
        LogicalPositionInfo3D logPos;
        reader->ReadTileInfo(r[i], nullptr, &logPos);

        logPositions.push_back(logPos);
    }

    std::vector<LogicalPositionInfo3D> expected;
    expected.reserve(100);
    for (int x = 0; x < 10; ++x)
    {
        for (int y = 0; y < 10; ++y)
        {
            expected.push_back(LogicalPositionInfo3D{ x * 256.0,y * 256.0,0,256,256,256,0 });
        }
    }

    bool correct = CheckIfSetsAreEqual(logPositions, expected);
    EXPECT_TRUE(correct) << "did not get the expected tiles";
}