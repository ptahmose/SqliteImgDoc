#include "pch.h"

using namespace std;
using namespace SlImgDoc;

TEST(Utilities, TestUint64_1)
{
    uint64_t v=0;
    bool b = MiscUtils::TryParseUint64("8534324433436", &v);
    EXPECT_TRUE(b);
    EXPECT_EQ(v, 8534324433436);
}

TEST(Utilities, TestUint64_2)
{
    uint64_t v = 0;
    bool b = MiscUtils::TryParseUint64("?18446744073709551615?", &v);
    EXPECT_TRUE(b);
    EXPECT_EQ(v, 18446744073709551615ULL);
}

TEST(Utilities, TestUint64_3)
{
    uint64_t v = 0;
    bool b = MiscUtils::TryParseUint64("?18446744073709551616?", &v);
    EXPECT_FALSE(b);
}

TEST(Utilities, TestUint64_4)
{
    uint64_t v = 0;
    bool b = MiscUtils::TryParseUint64("?184467sfd616?", &v);
    EXPECT_FALSE(b);
}