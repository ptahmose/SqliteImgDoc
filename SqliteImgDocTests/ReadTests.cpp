#include "pch.h"

using namespace std;
using namespace SlImgDoc;

TEST(Test1, ReadTests)
{
    CreateOptions opts;
    opts.dbFilename = ":memory:";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.dimensions.emplace('T');
    opts.dimensions.emplace('M');

    auto db = IDbFactory::CreateNew(opts);
}