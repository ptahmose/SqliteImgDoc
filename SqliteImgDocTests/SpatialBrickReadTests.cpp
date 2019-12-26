#include "pch.h"

using namespace std;
using namespace SlImgDoc;

static shared_ptr<IDb> CreateBrickTestDatabase(int rows, int columns, int stacks, int sizeX, int sizeY, int sizeZ)
{
    // create a database with dimension 'M' only
    // place tiles on in a checkerboard arrangement

    CreateOptions opts;
    opts.dbFilename = ":memory:";
    opts.dbFilename = "E:\\unittest3d.db";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.dimensions.emplace('T');
    opts.dimensions.emplace('M');

    auto db = IDbFactory::CreateNew3D(opts);

    auto dbWrite = db->GetWriter3D();

    dbWrite->BeginTransaction();

    LogicalPositionInfo3D posInfo;
    posInfo.width = sizeX;
    posInfo.height = sizeY;
    posInfo.depth = sizeZ;
    posInfo.pyrLvl = 0;
    TileBaseInfo3D tileBaseInfo;
    tileBaseInfo.pixelWidth = sizeX;
    tileBaseInfo.pixelHeight = sizeY;
    tileBaseInfo.pixelDepth = sizeZ;
    tileBaseInfo.pixelType = PixelType::GRAY8;

    CDataObjCustom dataCustom(16, 1);

    int m = 0;
    TileCoordinate tc({ { 'M',0 } });

    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < columns; ++c)
        {
            for (int z = 0; z < stacks; ++z)
            {
                posInfo.posX = c * sizeX;
                posInfo.posY = r * sizeY;
                posInfo.posZ = z * sizeZ;

                tc.Set('M', m);
                dbWrite->AddBrick(&tc, &posInfo, &tileBaseInfo, &dataCustom);
                m++;
            }
        }
    }

    dbWrite->CommitTransaction();
    return db;
}

TEST(SpatialBrickReadTests, QueryCuboid1)
{
    auto db = CreateBrickTestDatabase(10, 10, 10, 256, 256, 256);
}