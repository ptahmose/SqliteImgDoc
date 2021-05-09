#include "pch.h"

using namespace std;
using namespace SlImgDoc;

TEST(IndexTests, CreateDbWithAnIndexForTThenCheckForIndexPresence)
{
    static const char* DatabaseFilename = "file:memdbindextest1?mode=memory&cache=shared";

    CreateOptions opts;

    // Note: the syntax below allows to have multiple connections to the "in-memory-data database",
    //        cf. https://sqlite.org/inmemorydb.html
    opts.dbFilename = DatabaseFilename;
    opts.dimensions.emplace('T');
    opts.sizeOfDataBinHdrField = 4;
    auto db = IDbFactory::CreateNew(opts);
    auto dbWrite = db->GetWriter();

    dbWrite->BeginTransaction();
    for (int t = 0; t < 1000; ++t)
    {
        LogicalPositionInfo posInfo;
        posInfo.width = 100;
        posInfo.height = 100;
        posInfo.pyrLvl = 0;
        TileBaseInfo tileBaseInfo;
        tileBaseInfo.pixelWidth = 100;
        tileBaseInfo.pixelHeight = 100;
        tileBaseInfo.pixelType = PixelType::GRAY8;

        // and now we add a coordinate which has dimension 'T'
        TileCoordinate tc({ { 'T', t } });
        CDataObjCustom dataCustom(4, 1);

        dbWrite->AddTile(&tc, &posInfo, &tileBaseInfo, DataTypes::CUSTOM, &dataCustom);
    }

    dbWrite->CommitTransaction();

    dbWrite->CreateIndexOnCoordinate('T');

    dbWrite.reset();

    OpenOptions openOpts(DatabaseFilename);
    auto db2 = IDbFactory::OpenExisting(openOpts);
    auto dbRead = db2->GetReader();
    EXPECT_TRUE(dbRead) << "Expecting to have a valid data-base here.";

    bool isIndexed = dbRead->IsDimensionIndexIndexed('T');
    EXPECT_TRUE(isIndexed) << "Expected to find an index for dimension 'T'.";
}

TEST(IndexTests, CreateDbWithAnIndexForTThenDeleteIndexAndCheckForIndexPresence)
{
    static const char* DatabaseFilename = "file:memdbindextest2?mode=memory&cache=shared";

    CreateOptions opts;

    // Note: the syntax below allows to have multiple connections to the "in-memory-data database",
    //        cf. https://sqlite.org/inmemorydb.html
    opts.dbFilename = DatabaseFilename;
    opts.dimensions.emplace('T');
    opts.sizeOfDataBinHdrField = 4;
    auto db = IDbFactory::CreateNew(opts);
    auto dbWrite = db->GetWriter();

    dbWrite->BeginTransaction();
    for (int t = 0; t < 1000; ++t)
    {
        LogicalPositionInfo posInfo;
        posInfo.width = 100;
        posInfo.height = 100;
        posInfo.pyrLvl = 0;
        TileBaseInfo tileBaseInfo;
        tileBaseInfo.pixelWidth = 100;
        tileBaseInfo.pixelHeight = 100;
        tileBaseInfo.pixelType = PixelType::GRAY8;

        // and now we add a coordinate which has dimension 'T'
        TileCoordinate tc({ { 'T', t } });
        CDataObjCustom dataCustom(4, 1);

        dbWrite->AddTile(&tc, &posInfo, &tileBaseInfo, DataTypes::CUSTOM, &dataCustom);
    }

    dbWrite->CommitTransaction();
    dbWrite->CreateIndexOnCoordinate('T');
    dbWrite.reset();

    OpenOptions openOpts(DatabaseFilename, false);
    auto db2 = IDbFactory::OpenExisting(openOpts);
    auto dbRead = db2->GetReader();
    auto dbWrite2 = db2->GetWriter();
    EXPECT_TRUE(dbRead) << "Expecting to have a valid data-base here.";

    bool isIndexed = dbRead->IsDimensionIndexIndexed('T');
    EXPECT_TRUE(isIndexed) << "Expected to find an index for dimension 'T'.";

    dbWrite2->DropIndexOnCoordinate('T');

    isIndexed = dbRead->IsDimensionIndexIndexed('T');
    EXPECT_FALSE(isIndexed) << "Not Expecting to find an index for dimension 'T'.";
}