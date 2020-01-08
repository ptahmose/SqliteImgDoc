#include "pch.h"

using namespace std;
using namespace SlImgDoc;

TEST(PerTilesDataTests, BasicDouble)
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

TEST(PerTilesDataTests, DoubleAndInteger)
{
    CreateOptions opts;
    opts.dbFilename = ":memory:";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.dimensions.emplace('T');
    opts.dimensions.emplace('M');

    opts.perTileData.descriptions.push_back(ColumnDescription{ "DOUBLE",VariadicData::DataType_FLOAT });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "I8",VariadicData::DataType_INTEGER1 });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "I16",VariadicData::DataType_INTEGER2 });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "I32",VariadicData::DataType_INTEGER4 });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "I64",VariadicData::DataType_INTEGER8 });

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
    input.emplace_back(KeyVariadicValuePair{ "I8",VariadicData((int8_t)2) });
    input.emplace_back(KeyVariadicValuePair{ "I16",VariadicData((int16_t)3) });
    input.emplace_back(KeyVariadicValuePair{ "I32",VariadicData((int32_t)4) });
    input.emplace_back(KeyVariadicValuePair{ "I64",VariadicData((int64_t)5) });

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

    vector<string> cols{ "DOUBLE","I8","I16","I32","I64" };

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
    ASSERT_TRUE(it2->Data.IsInteger1());
    EXPECT_EQ(it2->Data.i8Value, 2);

    const auto& it3 = find_if(
        results.cbegin(),
        results.cend(),
        [&](const KeyVariadicValuePair& kvvp)->bool {return kvvp.Name == cols[2]; });
    ASSERT_FALSE(it3 == results.cend());
    ASSERT_TRUE(it3->Data.IsInteger2());
    EXPECT_EQ(it3->Data.i16Value, 3);

    const auto& it4 = find_if(
        results.cbegin(),
        results.cend(),
        [&](const KeyVariadicValuePair& kvvp)->bool {return kvvp.Name == cols[3]; });
    ASSERT_FALSE(it4 == results.cend());
    ASSERT_TRUE(it4->Data.IsInteger4());
    EXPECT_EQ(it4->Data.i32Value, 4);

    const auto& it5 = find_if(
        results.cbegin(),
        results.cend(),
        [&](const KeyVariadicValuePair& kvvp)->bool {return kvvp.Name == cols[4]; });
    ASSERT_FALSE(it5 == results.cend());
    ASSERT_TRUE(it5->Data.IsInteger8());
    EXPECT_EQ(it5->Data.i64Value, 5);
}

TEST(PerTilesDataTests, DoubleAndIntegerMax)
{
    CreateOptions opts;
    opts.dbFilename = ":memory:";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.dimensions.emplace('T');
    opts.dimensions.emplace('M');

    opts.perTileData.descriptions.push_back(ColumnDescription{ "DOUBLE",VariadicData::DataType_FLOAT });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "I8",VariadicData::DataType_INTEGER1 });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "I16",VariadicData::DataType_INTEGER2 });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "I32",VariadicData::DataType_INTEGER4 });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "I64",VariadicData::DataType_INTEGER8 });

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
    input.emplace_back(KeyVariadicValuePair{ "I8",VariadicData(numeric_limits<int8_t>::max()) });
    input.emplace_back(KeyVariadicValuePair{ "I16",VariadicData(numeric_limits<int16_t>::max()) });
    input.emplace_back(KeyVariadicValuePair{ "I32",VariadicData(numeric_limits<int32_t>::max()) });
    input.emplace_back(KeyVariadicValuePair{ "I64",VariadicData(numeric_limits<int64_t>::max()) });

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

    vector<string> cols{ "DOUBLE","I8","I16","I32","I64" };

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
    ASSERT_TRUE(it2->Data.IsInteger1());
    EXPECT_EQ(it2->Data.i8Value, numeric_limits<int8_t>::max());

    const auto& it3 = find_if(
        results.cbegin(),
        results.cend(),
        [&](const KeyVariadicValuePair& kvvp)->bool {return kvvp.Name == cols[2]; });
    ASSERT_FALSE(it3 == results.cend());
    ASSERT_TRUE(it3->Data.IsInteger2());
    EXPECT_EQ(it3->Data.i16Value, numeric_limits<int16_t>::max());

    const auto& it4 = find_if(
        results.cbegin(),
        results.cend(),
        [&](const KeyVariadicValuePair& kvvp)->bool {return kvvp.Name == cols[3]; });
    ASSERT_FALSE(it4 == results.cend());
    ASSERT_TRUE(it4->Data.IsInteger4());
    EXPECT_EQ(it4->Data.i32Value, numeric_limits<int32_t>::max());

    const auto& it5 = find_if(
        results.cbegin(),
        results.cend(),
        [&](const KeyVariadicValuePair& kvvp)->bool {return kvvp.Name == cols[4]; });
    ASSERT_FALSE(it5 == results.cend());
    ASSERT_TRUE(it5->Data.IsInteger8());
    EXPECT_EQ(it5->Data.i64Value, numeric_limits<int64_t>::max());
}

TEST(PerTilesDataTests, DoubleAndIntegerMin)
{
    CreateOptions opts;
    opts.dbFilename = ":memory:";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.dimensions.emplace('T');
    opts.dimensions.emplace('M');

    opts.perTileData.descriptions.push_back(ColumnDescription{ "DOUBLE",VariadicData::DataType_FLOAT });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "I8",VariadicData::DataType_INTEGER1 });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "I16",VariadicData::DataType_INTEGER2 });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "I32",VariadicData::DataType_INTEGER4 });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "I64",VariadicData::DataType_INTEGER8 });

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
    input.emplace_back(KeyVariadicValuePair{ "DOUBLE",VariadicData(numeric_limits<double>::min()) });
    input.emplace_back(KeyVariadicValuePair{ "I8",VariadicData(numeric_limits<int8_t>::min()) });
    input.emplace_back(KeyVariadicValuePair{ "I16",VariadicData(numeric_limits<int16_t>::min()) });
    input.emplace_back(KeyVariadicValuePair{ "I32",VariadicData(numeric_limits<int32_t>::min()) });
    input.emplace_back(KeyVariadicValuePair{ "I64",VariadicData(numeric_limits<int64_t>::min()) });

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

    vector<string> cols{ "DOUBLE","I8","I16","I32","I64" };

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
    EXPECT_DOUBLE_EQ(it->Data.doubleValue, numeric_limits<double>::min());

    const auto& it2 = find_if(
        results.cbegin(),
        results.cend(),
        [&](const KeyVariadicValuePair& kvvp)->bool {return kvvp.Name == cols[1]; });
    ASSERT_FALSE(it2 == results.cend());
    ASSERT_TRUE(it2->Data.IsInteger1());
    EXPECT_EQ(it2->Data.i8Value, numeric_limits<int8_t>::min());

    const auto& it3 = find_if(
        results.cbegin(),
        results.cend(),
        [&](const KeyVariadicValuePair& kvvp)->bool {return kvvp.Name == cols[2]; });
    ASSERT_FALSE(it3 == results.cend());
    ASSERT_TRUE(it3->Data.IsInteger2());
    EXPECT_EQ(it3->Data.i16Value, numeric_limits<int16_t>::min());

    const auto& it4 = find_if(
        results.cbegin(),
        results.cend(),
        [&](const KeyVariadicValuePair& kvvp)->bool {return kvvp.Name == cols[3]; });
    ASSERT_FALSE(it4 == results.cend());
    ASSERT_TRUE(it4->Data.IsInteger4());
    EXPECT_EQ(it4->Data.i32Value, numeric_limits<int32_t>::min());

    const auto& it5 = find_if(
        results.cbegin(),
        results.cend(),
        [&](const KeyVariadicValuePair& kvvp)->bool {return kvvp.Name == cols[4]; });
    ASSERT_FALSE(it5 == results.cend());
    ASSERT_TRUE(it5->Data.IsInteger8());
    EXPECT_EQ(it5->Data.i64Value, numeric_limits<int64_t>::min());
}

TEST(PerTilesDataTests, EnumPerTilesDataColumns1)
{
    CreateOptions opts;
    opts.dbFilename = ":memory:";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.dimensions.emplace('T');
    opts.dimensions.emplace('M');

    opts.perTileData.descriptions.push_back(ColumnDescription{ "A","FLOAT" });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "B","INTEGER(1)" });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "C","INTEGER(2)" });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "D","INTEGER(4)" });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "E","INTEGER(8)" });

    auto db = IDbFactory::CreateNew(opts);

    auto reader = db->GetReader();

    vector<ColumnDescription> colDescr;
    reader->EnumPerTileColumns([&](const ColumnDescription& cd)->bool {colDescr.emplace_back(cd); return true; });

    for (const auto d : opts.perTileData.descriptions)
    {
        const auto& it = find_if(
            colDescr.cbegin(),
            colDescr.cend(),
            [&](const ColumnDescription& cd)->bool {return cd.Name == d.Name; });
        ASSERT_TRUE(it->DataType == d.DataType);
    }
}

TEST(PerTilesDataTests, EnumPerTilesDataColumns2)
{
    CreateOptions opts;
    opts.dbFilename = "file:memdb3?mode=memory&cache=shared";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.dimensions.emplace('T');
    opts.dimensions.emplace('M');

    opts.perTileData.descriptions.push_back(ColumnDescription{ "A", VariadicData::DataType_FLOAT });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "B", VariadicData::DataType_INTEGER1 });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "C", VariadicData::DataType_INTEGER2 });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "D", VariadicData::DataType_INTEGER4 });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "E", VariadicData::DataType_INTEGER8 });

    auto db = IDbFactory::CreateNew(opts);

    OpenOptions openOpts;
    openOpts.dbFilename = opts.dbFilename;
    auto db2 = IDbFactory::OpenExisting(openOpts);
    auto reader = db2->GetReader();

    vector<ColumnDescription> colDescr;
    reader->EnumPerTileColumns([&](const ColumnDescription& cd)->bool {colDescr.emplace_back(cd); return true; });

    for (const auto d : opts.perTileData.descriptions)
    {
        const auto& it = find_if(
            colDescr.cbegin(),
            colDescr.cend(),
            [&](const ColumnDescription& cd)->bool {return cd.Name == d.Name; });
        ASSERT_TRUE(it->DataType == d.DataType);
    }
}

//-----------------------------------------------------------------------------

TEST(PerTilesDataTests, BricksBasicDouble)
{
    CreateOptions opts;
    opts.dbFilename = ":memory:";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.dimensions.emplace('T');
    opts.dimensions.emplace('M');

    opts.perTileData.descriptions.push_back(ColumnDescription{ "AcquisitionTime","FLOAT" });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "FocusPosition","FLOAT" });

    auto db = IDbFactory::CreateNew3D(opts);
    auto dbw = db->GetWriter3D();

    TileCoordinate coord({ { 'C',0 },{ 'Z',0 },{'T',0},{'M',0} });
    LogicalPositionInfo3D posInfo;
    posInfo.posX = posInfo.posY = posInfo.posZ = 0;
    posInfo.width = 10;
    posInfo.height = 10;
    posInfo.depth= 10;
    posInfo.pyrLvl = 0;
    TileBaseInfo3D tileBaseInfo;
    tileBaseInfo.pixelWidth = 10;
    tileBaseInfo.pixelHeight = 10;
    tileBaseInfo.pixelDepth = 10;
    tileBaseInfo.pixelType = PixelType::GRAY8;
    CDataObjCustom dataCustom(1, 1);

    auto idx = dbw->AddBrick(&coord, &posInfo, &tileBaseInfo, &dataCustom);

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

    auto dbr = db->GetReader3D();

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
