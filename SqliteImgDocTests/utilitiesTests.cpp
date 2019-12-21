#include "pch.h"

using namespace std;
using namespace SlImgDoc;

TEST(Utilities, TestUint64_1)
{
    uint64_t v = 0;
    bool b = MiscUtils::TryParseUint64("8534324433436", &v);
    EXPECT_TRUE(b);
    EXPECT_EQ(v, 8534324433436);
}

TEST(Utilities, TestUint64_2)
{
    uint64_t v = 0;
    bool b = MiscUtils::TryParseUint64("18446744073709551615", &v);
    EXPECT_TRUE(b);
    EXPECT_EQ(v, 18446744073709551615ULL);
}

TEST(Utilities, TestUint64_3)
{
    uint64_t v = 0;
    bool b = MiscUtils::TryParseUint64("18446744073709551616", &v);
    EXPECT_FALSE(b);
}

TEST(Utilities, TestUint64_4)
{
    uint64_t v = 0;
    bool b = MiscUtils::TryParseUint64("184467sdf616", &v);
    EXPECT_FALSE(b);
}

TEST(Utilities, TestUint32_1)
{
    uint32_t v = 0;
    bool b = MiscUtils::TryParseUint32("834353255", &v);
    EXPECT_TRUE(b);
    EXPECT_EQ(v, 834353255);
}

TEST(Utilities, TestUint32_2)
{
    uint32_t v = 0;
    bool b = MiscUtils::TryParseUint32("4294967295", &v);
    EXPECT_TRUE(b);
    EXPECT_EQ(v, 4294967295UL);
}

TEST(Utilities, TestUint32_3)
{
    uint32_t v = 0;
    bool b = MiscUtils::TryParseUint32("4294967296", &v);
    EXPECT_FALSE(b);
}

TEST(Utilities, TestUint32_4)
{
    uint32_t v = 0;
    bool b = MiscUtils::TryParseUint32("4467sdf616", &v);
    EXPECT_FALSE(b);
}
