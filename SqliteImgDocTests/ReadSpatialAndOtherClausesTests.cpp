#include "pch.h"
#include "testUtilities.h"

using namespace std;
using namespace SlImgDoc;

TEST(ReadSpatialAndOtherClausesTests, QueryRectAndDimensionClauseAllIndexed)
{
    CTestUtilities::MosaicTestDataBaseOptions testDbCreateOptions;
    testDbCreateOptions.withSpatialIndex = true;
    testDbCreateOptions.withDimensionIndex = true;
    testDbCreateOptions.withMindex = true;
    testDbCreateOptions.rows = 10;
    testDbCreateOptions.columns = 10;
    testDbCreateOptions.sizeTileX = 1024;
    testDbCreateOptions.sizeTileY = 1024;
    testDbCreateOptions.overlap = 0.1;
    testDbCreateOptions.dimAndRange.emplace_back(CTestUtilities::DimAndRange{ 'T',0,10 });
    auto db = CTestUtilities::CreateMosaicTestDatabase(testDbCreateOptions);

    auto reader = db->GetReader();

    CDimCoordinateQueryClause queryClause;
    queryClause.AddRangeClause('T', IDimCoordinateQueryClause::RangeClause{ 3, 3 });
    RectangleD queryRect{ 0,0,1024,1024 };
    auto result = reader->GetTilesIntersectingRect(queryRect, &queryClause, nullptr);

    EXPECT_EQ(result.size(), 4) << "Expected to have 4 matching tiles.";

    std::vector<LogicalPositionInfo> expected;
    expected.emplace_back(0,0,1024,1024,0);
    expected.emplace_back(0,1024 * 0.9,1024,1024,0);
    expected.emplace_back(1024 * 0.9,0,1024,1024,0);
    expected.emplace_back(1024 * 0.9,1024 * 0.9,1024,1024,0);

    std::vector<LogicalPositionInfo> logPositions;
    for (auto idx : result)
    {
        TileCoordinate tc;
        LogicalPositionInfo lpi;
        reader->ReadTileInfo(idx, &tc, &lpi);
        int tCoord;
        bool b = tc.TryGetCoordinate('T', &tCoord);
        EXPECT_TRUE(b) << "Expected to find a T-coordinate";
        EXPECT_EQ(tCoord, 3) << "Expected to only find tiles with T=3";
        logPositions.push_back(lpi);
    }

    bool correct = CTestUtilities::CheckIfSetsAreEqual(logPositions, expected);
    EXPECT_TRUE(correct) << "did not get the expected tiles";
}

TEST(ReadSpatialAndOtherClausesTests, QueryRectAndDimensionClauseNoneIndexed)
{
    CTestUtilities::MosaicTestDataBaseOptions testDbCreateOptions;
    testDbCreateOptions.withSpatialIndex = false;
    testDbCreateOptions.withDimensionIndex = false;
    testDbCreateOptions.withMindex = true;
    testDbCreateOptions.rows = 10;
    testDbCreateOptions.columns = 10;
    testDbCreateOptions.sizeTileX = 1024;
    testDbCreateOptions.sizeTileY = 1024;
    testDbCreateOptions.overlap = 0.1;
    testDbCreateOptions.dimAndRange.emplace_back(CTestUtilities::DimAndRange{ 'T',0,10 });
    auto db = CTestUtilities::CreateMosaicTestDatabase(testDbCreateOptions);

    auto reader = db->GetReader();

    CDimCoordinateQueryClause queryClause;
    queryClause.AddRangeClause('T', IDimCoordinateQueryClause::RangeClause{ 3, 3 });
    RectangleD queryRect{ 0,0,1024,1024 };
    auto result = reader->GetTilesIntersectingRect(queryRect, &queryClause, nullptr);

    EXPECT_EQ(result.size(), 4) << "Expected to have 4 matching tiles.";

    std::vector<LogicalPositionInfo> expected;
    expected.emplace_back(0, 0, 1024, 1024, 0);
    expected.emplace_back(0, 1024 * 0.9, 1024, 1024, 0);
    expected.emplace_back(1024 * 0.9, 0, 1024, 1024, 0);
    expected.emplace_back(1024 * 0.9, 1024 * 0.9, 1024, 1024, 0);

    std::vector<LogicalPositionInfo> logPositions;
    for (auto idx : result)
    {
        TileCoordinate tc;
        LogicalPositionInfo lpi;
        reader->ReadTileInfo(idx, &tc, &lpi);
        int tCoord;
        bool b = tc.TryGetCoordinate('T', &tCoord);
        EXPECT_TRUE(b) << "Expected to find a T-coordinate";
        EXPECT_EQ(tCoord, 3) << "Expected to only find tiles with T=3";
        logPositions.push_back(lpi);
    }

    bool correct = CTestUtilities::CheckIfSetsAreEqual(logPositions, expected);
    EXPECT_TRUE(correct) << "did not get the expected tiles";
}

TEST(ReadSpatialAndOtherClausesTests, QueryRectAndDimensionClauseSpatialIndexedDimensionNotIndexed)
{
    CTestUtilities::MosaicTestDataBaseOptions testDbCreateOptions;
    testDbCreateOptions.withSpatialIndex = true;
    testDbCreateOptions.withDimensionIndex = false;
    testDbCreateOptions.withMindex = true;
    testDbCreateOptions.rows = 10;
    testDbCreateOptions.columns = 10;
    testDbCreateOptions.sizeTileX = 1024;
    testDbCreateOptions.sizeTileY = 1024;
    testDbCreateOptions.overlap = 0.1;
    testDbCreateOptions.dimAndRange.emplace_back(CTestUtilities::DimAndRange{ 'T',0,10 });
    auto db = CTestUtilities::CreateMosaicTestDatabase(testDbCreateOptions);

    auto reader = db->GetReader();

    CDimCoordinateQueryClause queryClause;
    queryClause.AddRangeClause('T', IDimCoordinateQueryClause::RangeClause{ 3, 3 });
    RectangleD queryRect{ 0,0,1024,1024 };
    auto result = reader->GetTilesIntersectingRect(queryRect, &queryClause, nullptr);

    EXPECT_EQ(result.size(), 4) << "Expected to have 4 matching tiles.";

    std::vector<LogicalPositionInfo> expected;
    expected.emplace_back(0, 0, 1024, 1024, 0);
    expected.emplace_back(0, 1024 * 0.9, 1024, 1024, 0);
    expected.emplace_back(1024 * 0.9, 0, 1024, 1024, 0);
    expected.emplace_back(1024 * 0.9, 1024 * 0.9, 1024, 1024, 0);

    std::vector<LogicalPositionInfo> logPositions;
    for (auto idx : result)
    {
        TileCoordinate tc;
        LogicalPositionInfo lpi;
        reader->ReadTileInfo(idx, &tc, &lpi);
        int tCoord;
        bool b = tc.TryGetCoordinate('T', &tCoord);
        EXPECT_TRUE(b) << "Expected to find a T-coordinate";
        EXPECT_EQ(tCoord, 3) << "Expected to only find tiles with T=3";
        logPositions.push_back(lpi);
    }

    bool correct = CTestUtilities::CheckIfSetsAreEqual(logPositions, expected);
    EXPECT_TRUE(correct) << "did not get the expected tiles";
}

TEST(ReadSpatialAndOtherClausesTests, QueryRectAndDimensionClauseDimensionIndexedSpatialNotIndexed)
{
    CTestUtilities::MosaicTestDataBaseOptions testDbCreateOptions;
    testDbCreateOptions.withSpatialIndex = false;
    testDbCreateOptions.withDimensionIndex = true;
    testDbCreateOptions.withMindex = true;
    testDbCreateOptions.rows = 10;
    testDbCreateOptions.columns = 10;
    testDbCreateOptions.sizeTileX = 1024;
    testDbCreateOptions.sizeTileY = 1024;
    testDbCreateOptions.overlap = 0.1;
    testDbCreateOptions.dimAndRange.emplace_back(CTestUtilities::DimAndRange{ 'T',0,10 });
    auto db = CTestUtilities::CreateMosaicTestDatabase(testDbCreateOptions);

    auto reader = db->GetReader();

    CDimCoordinateQueryClause queryClause;
    queryClause.AddRangeClause('T', IDimCoordinateQueryClause::RangeClause{ 3, 3 });
    RectangleD queryRect{ 0,0,1024,1024 };
    auto result = reader->GetTilesIntersectingRect(queryRect, &queryClause, nullptr);

    EXPECT_EQ(result.size(), 4) << "Expected to have 4 matching tiles.";

    std::vector<LogicalPositionInfo> expected;
    expected.emplace_back(0, 0, 1024, 1024, 0);
    expected.emplace_back(0, 1024 * 0.9, 1024, 1024, 0);
    expected.emplace_back(1024 * 0.9, 0, 1024, 1024, 0);
    expected.emplace_back(1024 * 0.9, 1024 * 0.9, 1024, 1024, 0);

    std::vector<LogicalPositionInfo> logPositions;
    for (auto idx : result)
    {
        TileCoordinate tc;
        LogicalPositionInfo lpi;
        reader->ReadTileInfo(idx, &tc, &lpi);
        int tCoord;
        bool b = tc.TryGetCoordinate('T', &tCoord);
        EXPECT_TRUE(b) << "Expected to find a T-coordinate";
        EXPECT_EQ(tCoord, 3) << "Expected to only find tiles with T=3";
        logPositions.push_back(lpi);
    }

    bool correct = CTestUtilities::CheckIfSetsAreEqual(logPositions, expected);
    EXPECT_TRUE(correct) << "did not get the expected tiles";
}
