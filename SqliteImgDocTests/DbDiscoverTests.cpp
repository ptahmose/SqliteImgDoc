#include "pch.h"

using namespace std;
using namespace SlImgDoc;

static shared_ptr<IDb> CreateTestDatabase()
{
    // create a database with dimensions 'C', 'Z', 'T' and 'M'
    // C=[0,2] Z=[0,5] T=[0,5] M=0

    CreateOptions opts;
    opts.dbFilename = ":memory:";
    //opts.dbFilename = "D:\\unittest.db";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.dimensions.emplace('T');
    opts.dimensions.emplace('M');

    auto db = IDbFactory::CreateNew(opts);

    auto dbWrite = db->GetWriter();

    dbWrite->BeginTransaction();

    LogicalPositionInfo posInfo;
    posInfo.posX = 0;
    posInfo.posY = 0;
    posInfo.width = 100;
    posInfo.height = 100;
    posInfo.pyrLvl = 0;
    TileBaseInfo tileBaseInfo;
    tileBaseInfo.pixelWidth = 100;
    tileBaseInfo.pixelHeight = 100;
    tileBaseInfo.pixelType = PixelType::GRAY8;

    CDataObjCustom dataCustom(16, 1);

    TileCoordinate tc({ { 'C', 0 }, { 'Z',0 }, { 'T',0 }, { 'M',0 } });
    for (int c = 0; c < 3; ++c)
    {
        tc.Set('C', c);
        for (int z = 0; z < 5; ++z)
        {
            tc.Set('Z', z);
            for (int t = 0; t < 5; ++t)
            {
                tc.Set('T', t);

                dbWrite->AddTile(&tc, &posInfo, &tileBaseInfo, DataTypes::CUSTOM, &dataCustom);
            }
        }
    }

    dbWrite->CommitTransaction();

    return db;
}

TEST(DbDiscoverTests, Test1)
{
    auto db = CreateTestDatabase();
    auto internalDb = std::dynamic_pointer_cast<IInternalDb>(db);

    CDbDiscover discover(&internalDb->GetDb());
    discover.DoIt();
    auto docInfo = discover.GetDocInfo();

    auto dataBinHdrSize = docInfo->GetDbParameter(IDbDocInfo::DbParameter::DataBinHdrSize);
    EXPECT_EQ(dataBinHdrSize, 32);
}

TEST(DbDiscoverTests, Test2)
{
    auto db = CreateTestDatabase();
    auto internalDb = std::dynamic_pointer_cast<IInternalDb>(db);

    internalDb->GetDb().exec("ALTER TABLE TILESDATA RENAME COLUMN Data_BinHdr TO Data_BinHdrX");

    CDbDiscover discover(&internalDb->GetDb());
    EXPECT_THROW(
        discover.DoIt(),
        SqliteImgDocDbDiscoverException);
}


TEST(DbDiscoverTests, PerTileDataDoubleAndIntegerOpenExisting)
{
    CreateOptions opts;
    opts.dbFilename = "file:memdb4?mode=memory&cache=shared";
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

    auto idx = dbw->AddTile(&coord, &posInfo, &tileBaseInfo, DataTypes::CUSTOM, &dataCustom);

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

    OpenOptions openOpts;
    openOpts.dbFilename = opts.dbFilename;
    auto db2 = IDbFactory::OpenExisting(openOpts);
    auto dbr = db2->GetReader();

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