#include "pch.h"

using namespace std;
using namespace SlImgDoc;

class CustomQueriesTest : public ::testing::Test
{
protected:
    static bool DoAabbAndPlaneIntersect(const SlImgDoc::CuboidD& aabb, const SlImgDoc::Plane_NormalAndDistD& plane) { return CCustomQueries::DoAabbAndPlaneIntersect(aabb, plane); }
};

TEST_F(CustomQueriesTest, aabbAndPlaneIntersection1)
{
    CuboidD aabb(10, 10, 10, 10, 10, 10);
    Plane_NormalAndDistD plane{ Vector3dD(0,0,1), 0 };
    bool b = CustomQueriesTest::DoAabbAndPlaneIntersect(aabb, plane);
    EXPECT_FALSE(b) << "Aabb and plane should not be reported intersecting.";
}

TEST_F(CustomQueriesTest, aabbAndPlaneIntersection2)
{
    CuboidD aabb(10, 10, 10, 10, 10, 10);
    Plane_NormalAndDistD plane{ Vector3dD(0,0,1), 10 };
    bool b = CustomQueriesTest::DoAabbAndPlaneIntersect(aabb, plane);
    EXPECT_TRUE(b) << "Aabb and plane should be reported intersecting.";
}

TEST_F(CustomQueriesTest, aabbAndPlaneIntersection3)
{
    CuboidD aabb(10, 10, 10, 10, 10, 10);
    Plane_NormalAndDistD plane{ Vector3dD(1,0,0), 10 };
    bool b = CustomQueriesTest::DoAabbAndPlaneIntersect(aabb, plane);
    EXPECT_TRUE(b) << "Aabb and plane should be reported intersecting.";
}

TEST_F(CustomQueriesTest, aabbAndPlaneIntersection4)
{
    CuboidD aabb(10, 10, 10, 10, 10, 10);
    Plane_NormalAndDistD plane{ Vector3dD(1, 1, -1).Normalize(), 0 };
    bool b = CustomQueriesTest::DoAabbAndPlaneIntersect(aabb, plane);
    EXPECT_TRUE(b) << "Aabb and plane should be reported intersecting.";
}