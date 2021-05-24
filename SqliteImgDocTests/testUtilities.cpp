#include "pch.h"
#include "testUtilities.h"

using namespace std;
using namespace SlImgDoc;

/*static*/std::shared_ptr<SlImgDoc::IDb> CTestUtilities::CreateMosaicTestDatabaseWithSpatialIndex(int rows, int columns, int sizeX, int sizeY)
{
    return  CTestUtilities::CreateMosaicTestDatabase(true, rows, columns, sizeX, sizeY);
}

/*static*/std::shared_ptr<SlImgDoc::IDb> CTestUtilities::CreateMosaicTestDatabaseWithoutSpatialIndex(int rows, int columns, int sizeX, int sizeY)
{
    return  CTestUtilities::CreateMosaicTestDatabase(false, rows, columns, sizeX, sizeY);
}

/*static*/shared_ptr<IDb> CTestUtilities::CreateMosaicTestDatabase(bool withSpatialIndex, int rows, int columns, int sizeX, int sizeY)
{
    // create a database with dimension 'M' only
    // place tiles on in a checkerboard arrangement

    CreateOptions opts;
    opts.createSpatialIndex = withSpatialIndex;
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
    posInfo.width = sizeX;
    posInfo.height = sizeY;
    posInfo.pyrLvl = 0;
    TileBaseInfo tileBaseInfo;
    tileBaseInfo.pixelWidth = sizeX;
    tileBaseInfo.pixelHeight = sizeY;
    tileBaseInfo.pixelType = PixelType::GRAY8;

    CDataObjCustom dataCustom(16, 1);

    int m = 0;
    TileCoordinate tc({ {'C',0},{'Z',0},{'T',0},{ 'M',0 } });

    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < columns; ++c)
        {
            posInfo.posX = c * sizeX;
            posInfo.posY = r * sizeY;

            tc.Set('M', m);
            dbWrite->AddTile(&tc, &posInfo, &tileBaseInfo, DataTypes::CUSTOM, &dataCustom);
            m++;
        }
    }

    dbWrite->CommitTransaction();
    return db;
}

/*static*/bool CTestUtilities::CheckIfSetsAreEqual(std::vector<LogicalPositionInfo>& a, std::vector<LogicalPositionInfo>& b)
{
    if (a.size() != b.size())
    {
        return false;
    }

    for (size_t i = 0; i < a.size(); ++i)
    {
        auto f = find(a.cbegin(), a.cend(), b[i]);
        if (f == a.cend())
        {
            return false;
        }
    }

    return true;
}
