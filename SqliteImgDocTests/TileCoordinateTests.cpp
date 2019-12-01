#include "pch.h"

using namespace std;
using namespace SlImgDoc;
TEST(Test1, TileCoordinate)
{
    TileCoordinate tc{ { 'T',1 }, { 'C',2 } };

    int v;
    bool b = tc.TryGetCoordinate('T', &v);
    EXPECT_EQ(v, 1);
    b = tc.TryGetCoordinate('C', &v);
    EXPECT_EQ(v, 2);
    b = tc.TryGetCoordinate('K', &v);
    EXPECT_FALSE(b);
}