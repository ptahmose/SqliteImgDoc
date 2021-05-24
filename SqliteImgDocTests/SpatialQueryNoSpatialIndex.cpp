#include "pch.h"
#include "testUtilities.h"

using namespace std;
using namespace SlImgDoc;

TEST(SpatialQueryNoSpatialIndex, QueryRect1)
{
    auto db = CTestUtilities::CreateMosaicTestDatabaseWithoutSpatialIndex(50, 50, 256, 256);
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

    const bool correct = CTestUtilities::CheckIfSetsAreEqual(logPositions, expected);
    EXPECT_TRUE(correct) << "did not get the expected tiles";
}