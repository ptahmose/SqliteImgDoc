#include "pch.h"

using namespace std;
using namespace SlImgDoc;

static shared_ptr<IDb> CreateTestDatabase()
{
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
    posInfo.posX = 0;
    posInfo.posY = 0;
    posInfo.width = 100;
    posInfo.height = 100;
    posInfo.pyrLvl = 0;
    TileBaseInfo tileBaseInfo;
    tileBaseInfo.pixelWidth = 100;
    tileBaseInfo.pixelHeight = 100;
    tileBaseInfo.pixelType = PixelType::GRAY8;

    CDataObjCustom dataCustom(16, 1);

    TileCoordinate tc({ { 'C', 0 }, { 'Z',0 }, { 'T',0 }, { 'M',0 } });
    for (int c = 0; c < 3; ++c)
    {
        tc.Set('C', c);
        for (int z = 0; z < 20; ++z)
        {
            tc.Set('Z', z);
            for (int t = 0; t < 5; ++t)
            {
                tc.Set('T', t);

                dbWrite->AddSubBlock(&tc, &posInfo, &tileBaseInfo, &dataCustom);
            }
        }
    }

    dbWrite->CommitTransaction();

    return db;
}

TEST(ReadTests, QueryCoordinates1)
{
    auto db = CreateTestDatabase();

    auto reader = db->GetReader();
    CDimCoordinateQueryClause queryClause;
    queryClause.AddListClause('C', IDimCoordinateQueryClause::ListClause{ {1} });
    queryClause.AddListClause('Z', IDimCoordinateQueryClause::ListClause{ {2} });
    queryClause.AddListClause('T', IDimCoordinateQueryClause::ListClause{ {3} });
    queryClause.AddListClause('M', IDimCoordinateQueryClause::ListClause{ {0} });
    auto r = reader->Query(&queryClause);

    ASSERT_TRUE(r.size() == 1) << "Expected exactly one result.";

    TileCoordinate tc;
    LogicalPositionInfo logPos;
    reader->ReadTileInfo(r[0], &tc, &logPos);

    EXPECT_EQ(logPos.posX, 0);
    EXPECT_EQ(logPos.posY, 0);
    EXPECT_EQ(logPos.width, 100);
    EXPECT_EQ(logPos.height, 100);
    EXPECT_EQ(logPos.pyrLvl, 0);

    int coordVal = -1;
    bool b = tc.TryGetCoordinate('C', &coordVal);
    EXPECT_TRUE(b); EXPECT_EQ(coordVal, 1);
    b = tc.TryGetCoordinate('Z', &coordVal);
    EXPECT_TRUE(b); EXPECT_EQ(coordVal, 2);
    b = tc.TryGetCoordinate('T', &coordVal);
    EXPECT_TRUE(b); EXPECT_EQ(coordVal, 3);
    b = tc.TryGetCoordinate('M', &coordVal);
    EXPECT_TRUE(b); EXPECT_EQ(coordVal, 0);
}

TEST(ReadTests, QueryCoordinates2)
{
    auto db = CreateTestDatabase();

    auto reader = db->GetReader();
    CDimCoordinateQueryClause queryClause;
    queryClause.AddListClause('C', IDimCoordinateQueryClause::ListClause{ {1} });
    queryClause.AddListClause('Z', IDimCoordinateQueryClause::ListClause{ {2} });
    queryClause.AddListClause('T', IDimCoordinateQueryClause::ListClause{ {3} });
    queryClause.AddListClause('M', IDimCoordinateQueryClause::ListClause{ {0} });
    auto r = reader->Query(&queryClause);

    ASSERT_TRUE(r.size() == 1) << "Expected exactly one result.";

    TileCoordinate tc;
    reader->ReadTileInfo(r[0], &tc, nullptr);

    int coordVal = -1;
    bool b = tc.TryGetCoordinate('C', &coordVal);
    EXPECT_TRUE(b); EXPECT_EQ(coordVal, 1);
    b = tc.TryGetCoordinate('Z', &coordVal);
    EXPECT_TRUE(b); EXPECT_EQ(coordVal, 2);
    b = tc.TryGetCoordinate('T', &coordVal);
    EXPECT_TRUE(b); EXPECT_EQ(coordVal, 3);
    b = tc.TryGetCoordinate('M', &coordVal);
    EXPECT_TRUE(b); EXPECT_EQ(coordVal, 0);
}

TEST(ReadTests, QueryCoordinates3)
{
    auto db = CreateTestDatabase();

    auto reader = db->GetReader();
    CDimCoordinateQueryClause queryClause;
    queryClause.AddListClause('C', IDimCoordinateQueryClause::ListClause{ {1} });
    queryClause.AddListClause('Z', IDimCoordinateQueryClause::ListClause{ {2} });
    queryClause.AddListClause('T', IDimCoordinateQueryClause::ListClause{ {3} });
    queryClause.AddListClause('M', IDimCoordinateQueryClause::ListClause{ {0} });
    auto r = reader->Query(&queryClause);

    ASSERT_TRUE(r.size() == 1) << "Expected exactly one result.";

    LogicalPositionInfo logPos;
    reader->ReadTileInfo(r[0], nullptr, &logPos);

    EXPECT_EQ(logPos.posX, 0);
    EXPECT_EQ(logPos.posY, 0);
    EXPECT_EQ(logPos.width, 100);
    EXPECT_EQ(logPos.height, 100);
    EXPECT_EQ(logPos.pyrLvl, 0);
}