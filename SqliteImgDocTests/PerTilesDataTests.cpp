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
            kv.Data.DataType = VariadicData::DataType_FLOAT;
            kv.Name = "AcquisitionTime";
            kv.Data.doubleValue = 42;
            return true;
        case 1:
            kv.Data.DataType = VariadicData::DataType_FLOAT;
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
    ASSERT_FALSE(it == results.cend());
    ASSERT_TRUE(it->Data.IsFloat());
    EXPECT_DOUBLE_EQ(it->Data.doubleValue, 42);

    const auto& it2 = find_if(
        results.cbegin(),
        results.cend(),
        [&](const KeyVariadicValuePair& kvvp)->bool {return kvvp.Name == cols[1]; });
    ASSERT_FALSE(it2 == results.cend());
    ASSERT_TRUE(it2->Data.IsFloat());
    EXPECT_DOUBLE_EQ(it2->Data.doubleValue, 43);
}

TEST(PerTilesDataTests, Test2)
{
    CreateOptions opts;
    opts.dbFilename = ":memory:";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.dimensions.emplace('T');
    opts.dimensions.emplace('M');

    opts.perTileData.descriptions.push_back(ColumnDescription{ "DOUBLE",VariadicData::DataType_FLOAT });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "UI8",VariadicData::DataType_UINTEGER1 });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "UI16",VariadicData::DataType_UINTEGER2 });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "UI32",VariadicData::DataType_UINTEGER4 });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "UI64",VariadicData::DataType_UINTEGER8 });

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

    vector< KeyVariadicValuePair> input;
    input.emplace_back(KeyVariadicValuePair{ "DOUBLE",VariadicData(42.5) });
    input.emplace_back(KeyVariadicValuePair{ "UI8",VariadicData((uint8_t)2) });
    input.emplace_back(KeyVariadicValuePair{ "UI16",VariadicData((uint16_t)3) });
    input.emplace_back(KeyVariadicValuePair{ "UI32",VariadicData((uint32_t)4) });
    input.emplace_back(KeyVariadicValuePair{ "UI64",VariadicData((uint64_t)5) });

    dbw->AddPerTileData(
        idx,
        [&](int no, SlImgDoc::KeyVariadicValuePair& kv)->bool
    {
        if (no < input.size())
        {
            kv = input[no];
            return true;
        }

        return false;
    });

    auto dbr = db->GetReader();

    vector<string> cols{ "DOUBLE","UI8","UI16","UI32","UI64" };

    vector<KeyVariadicValuePair> results;
    dbr->ReadPerTileData(idx, cols,
        [&](const KeyVariadicValuePair& kv)->bool {results.push_back(kv); return true; });

    ASSERT_EQ(results.size(), input.size()) << "Expected to have " << input.size() << " results.";

    const auto& it = find_if(
        results.cbegin(),
        results.cend(),
        [&](const KeyVariadicValuePair& kvvp)->bool {return kvvp.Name == cols[0]; });
    ASSERT_FALSE(it == results.cend());
    ASSERT_TRUE(it->Data.IsFloat());
    EXPECT_DOUBLE_EQ(it->Data.doubleValue, 42.5);

    const auto& it2 = find_if(
        results.cbegin(),
        results.cend(),
        [&](const KeyVariadicValuePair& kvvp)->bool {return kvvp.Name == cols[1]; });
    ASSERT_FALSE(it2 == results.cend());
    ASSERT_TRUE(it2->Data.IsUInteger1());
    EXPECT_EQ(it2->Data.ui8Value, 2);

    const auto& it3 = find_if(
        results.cbegin(),
        results.cend(),
        [&](const KeyVariadicValuePair& kvvp)->bool {return kvvp.Name == cols[2]; });
    ASSERT_FALSE(it3 == results.cend());
    ASSERT_TRUE(it3->Data.IsUInteger2());
    EXPECT_EQ(it3->Data.ui16Value, 3);

    const auto& it4 = find_if(
        results.cbegin(),
        results.cend(),
        [&](const KeyVariadicValuePair& kvvp)->bool {return kvvp.Name == cols[3]; });
    ASSERT_FALSE(it4 == results.cend());
    ASSERT_TRUE(it4->Data.IsUInteger4());
    EXPECT_EQ(it4->Data.ui32Value, 4);

    const auto& it5 = find_if(
        results.cbegin(),
        results.cend(),
        [&](const KeyVariadicValuePair& kvvp)->bool {return kvvp.Name == cols[4]; });
    ASSERT_FALSE(it5 == results.cend());
    ASSERT_TRUE(it5->Data.IsUInteger8());
    EXPECT_EQ(it5->Data.ui64Value, 5);
}

TEST(PerTilesDataTests, Test3)
{
    CreateOptions opts;
    opts.dbFilename = ":memory:";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.dimensions.emplace('T');
    opts.dimensions.emplace('M');

    opts.perTileData.descriptions.push_back(ColumnDescription{ "DOUBLE",VariadicData::DataType_FLOAT });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "UI8",VariadicData::DataType_UINTEGER1 });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "UI16",VariadicData::DataType_UINTEGER2 });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "UI32",VariadicData::DataType_UINTEGER4 });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "UI64",VariadicData::DataType_UINTEGER8 });

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

    vector< KeyVariadicValuePair> input;
    input.emplace_back(KeyVariadicValuePair{ "DOUBLE",VariadicData(numeric_limits<double>::max()) });
    input.emplace_back(KeyVariadicValuePair{ "UI8",VariadicData(numeric_limits<uint8_t>::max()) });
    input.emplace_back(KeyVariadicValuePair{ "UI16",VariadicData(numeric_limits<uint16_t>::max()) });
    input.emplace_back(KeyVariadicValuePair{ "UI32",VariadicData(numeric_limits<uint32_t>::max()) });
    input.emplace_back(KeyVariadicValuePair{ "UI64",VariadicData(numeric_limits<uint64_t>::max()) });

    dbw->AddPerTileData(
        idx,
        [&](int no, SlImgDoc::KeyVariadicValuePair& kv)->bool
    {
        if (no < input.size())
        {
            kv = input[no];
            return true;
        }

        return false;
    });

    auto dbr = db->GetReader();

    vector<string> cols{ "DOUBLE","UI8","UI16","UI32","UI64" };

    vector<KeyVariadicValuePair> results;
    dbr->ReadPerTileData(idx, cols,
        [&](const KeyVariadicValuePair& kv)->bool {results.push_back(kv); return true; });

    ASSERT_EQ(results.size(), input.size()) << "Expected to have " << input.size() << " results.";

    const auto& it = find_if(
        results.cbegin(),
        results.cend(),
        [&](const KeyVariadicValuePair& kvvp)->bool {return kvvp.Name == cols[0]; });
    ASSERT_FALSE(it == results.cend());
    ASSERT_TRUE(it->Data.IsFloat());
    EXPECT_DOUBLE_EQ(it->Data.doubleValue, numeric_limits<double>::max());

    const auto& it2 = find_if(
        results.cbegin(),
        results.cend(),
        [&](const KeyVariadicValuePair& kvvp)->bool {return kvvp.Name == cols[1]; });
    ASSERT_FALSE(it2 == results.cend());
    ASSERT_TRUE(it2->Data.IsUInteger1());
    EXPECT_EQ(it2->Data.ui8Value, numeric_limits<uint8_t>::max());

    const auto& it3 = find_if(
        results.cbegin(),
        results.cend(),
        [&](const KeyVariadicValuePair& kvvp)->bool {return kvvp.Name == cols[2]; });
    ASSERT_FALSE(it3 == results.cend());
    ASSERT_TRUE(it3->Data.IsUInteger2());
    EXPECT_EQ(it3->Data.ui16Value, numeric_limits<uint16_t>::max());

    const auto& it4 = find_if(
        results.cbegin(),
        results.cend(),
        [&](const KeyVariadicValuePair& kvvp)->bool {return kvvp.Name == cols[3]; });
    ASSERT_FALSE(it4 == results.cend());
    ASSERT_TRUE(it4->Data.IsUInteger4());
    EXPECT_EQ(it4->Data.ui32Value, numeric_limits<uint32_t>::max());

    const auto& it5 = find_if(
        results.cbegin(),
        results.cend(),
        [&](const KeyVariadicValuePair& kvvp)->bool {return kvvp.Name == cols[4]; });
    ASSERT_FALSE(it5 == results.cend());
    ASSERT_TRUE(it5->Data.IsUInteger8());
    EXPECT_EQ(it5->Data.ui64Value, numeric_limits<uint64_t>::max());
}
