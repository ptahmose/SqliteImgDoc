#include "pch.h"

using namespace std;
using namespace SlImgDoc;

TEST(PerTilesDataTests, Test1)
{
    CreateOptions opts;
    opts.dbFilename = ":memory:";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.dimensions.emplace('T');
    opts.dimensions.emplace('M');

    opts.perTileData.descriptions.push_back(ColumnDescription{ "AcquisitionTime","FLOAT" });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "FocusPosition","FLOAT" });

    auto db = IDbFactory::CreateNew(opts);
    auto dbw = db->GetWriter();

    TileCoordinate coord({ { 'C',0 },{ 'Z',0 },{'T',0},{'M',0} });
    LogicalPositionInfo posInfo;
    posInfo.posX = posInfo.posY = 0;
    posInfo.width = 10;
    posInfo.height = 10;
    posInfo.pyrLvl = 0;
    TileBaseInfo tileBaseInfo;
    tileBaseInfo.pixelWidth = 10;
    tileBaseInfo.pixelHeight = 10;
    tileBaseInfo.pixelType = PixelType::GRAY8;
    CDataObjCustom dataCustom(1, 1);

    auto idx = dbw->AddTile(&coord, &posInfo, &tileBaseInfo, &dataCustom);

    dbw->AddPerTileData(
        idx,
        [](int no, SlImgDoc::KeyVariadicValuePair& kv)->bool
    {
        switch (no)
        {
        case 0:
            kv.Data.DataType = "FLOAT";
            kv.Name = "AcquisitionTime";
            kv.Data.doubleValue = 42;
            return true;
        case 1:
            kv.Data.DataType = "FLOAT";
            kv.Name = "FocusPosition";
            kv.Data.doubleValue = 43;
            return true;
        default:
            return false;
        }
    });

    auto dbr = db->GetReader();

    vector<string> cols{ "AcquisitionTime","FocusPosition" };

    vector<KeyVariadicValuePair> results;
    dbr->ReadPerTileData(idx, cols,
        [&](const KeyVariadicValuePair& kv)->bool {results.push_back(kv); return true; });

    ASSERT_EQ(results.size(), 2) << "Expected to have two results.";
    const auto& it = find_if(
        results.cbegin(), 
        results.cend(), 
        [&](const KeyVariadicValuePair& kvvp)->bool {return kvvp.Name == cols[0]; });
    EXPECT_DOUBLE_EQ(it->Data.doubleValue, 42);

    const auto& it2 = find_if(
        results.cbegin(),
        results.cend(),
        [&](const KeyVariadicValuePair& kvvp)->bool {return kvvp.Name == cols[1]; });
    EXPECT_DOUBLE_EQ(it2->Data.doubleValue, 43);
}