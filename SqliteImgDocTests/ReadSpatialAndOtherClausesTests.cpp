#include "pch.h"
#include "testUtilities.h"

using namespace std;
using namespace SlImgDoc;

/**
 * Create a document with 10 T's and 10x10 tiles (with overlap) with T indexed and a spatial index, then query for T=3 and
 * for overlap with a query-rectangle.
 */
TEST(ReadSpatialAndOtherClausesTests, QueryRectAndDimensionClauseAllIndexed)
{
    // create a document with T in the range from 0 to 9, on each plane 10x10 tiles of 
    // extent 1024x1024 with 10% overlap.
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

    // query for all tiles with T=3 which overlap with the query-rectangle (0,0)-(1024,1024). We expect to find
    // 4 tiles.
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

/**
* Create a document with 10 T's and 10x10 tiles (with overlap) without and index, then query for T=3 and
* for overlap with a query-rectangle.
*/
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

/**
* Create a document with 10 T's and 10x10 tiles (with overlap) with only a spatial index, then query for T=3 and
* for overlap with a query-rectangle.
*/
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

/**
* Create a document with 10 T's and 10x10 tiles (with overlap) with only an index for T (and no spatial index),
* then query for T=3 and for overlap with a query-rectangle.
*/
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
