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

                dbWrite->AddTile(&tc, &posInfo, &tileBaseInfo, &dataCustom);
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
    //discover.DoIt();
    EXPECT_THROW(
        /*auto docInfo = discover.GetDocInfo()*/discover.DoIt(),
        SqliteImgDocDbDiscoverException);
}