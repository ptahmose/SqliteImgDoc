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

/*static*/std::shared_ptr<SlImgDoc::IDb> CTestUtilities::CreateMosaicTestDatabase(const MosaicTestDataBaseOptions& options)
{
    CreateOptions opts;
    opts.createSpatialIndex = false;    // Note: we create the spatial index (if requested) after we initialized the DB, which is a bit faster
    opts.dbFilename = ":memory:";
    //opts.dbFilename = "L:\\test.db";
    for (const DimAndRange& dr : options.dimAndRange)
    {
        opts.dimensions.emplace(dr.dim);
    }

    if (options.withMindex)
    {
        opts.dimensions.emplace('M');
    }

    auto db = IDbFactory::CreateNew(opts);
    auto dbWrite = db->GetWriter();

    CDataObjCustom dataCustom(16, 1);
    EnumCoordinates coordinateEnumerator(options.dimAndRange);

    dbWrite->BeginTransaction();
    TileCoordinate coord;
    for (;;)
    {
        if (!coordinateEnumerator.Next(coord))
        {
            break;
        }

        for (int r = 0; r < options.rows; ++r)
        {
            for (int c = 0; c < options.columns; ++c)
            {
                LogicalPositionInfo posInfo;
                posInfo.posX = c * (options.sizeTileX * (1 - options.overlap));
                posInfo.posY = r * (options.sizeTileY * (1 - options.overlap));
                posInfo.width = options.sizeTileX;
                posInfo.height = options.sizeTileY;
                posInfo.pyrLvl = 0;
                TileBaseInfo tileBaseInfo;
                tileBaseInfo.pixelWidth = options.sizeTileX;
                tileBaseInfo.pixelHeight = options.sizeTileY;
                tileBaseInfo.pixelType = PixelType::GRAY8;

                if (options.withMindex)
                {
                    coord.Set('M', r * options.rows + c);
                }

                dbWrite->AddTile(&coord, &posInfo, &tileBaseInfo, DataTypes::CUSTOM, &dataCustom);
            }
        }
    }

    dbWrite->CommitTransaction();

    if (options.withDimensionIndex)
    {
        for (auto d : opts.dimensions)
        {
            dbWrite->CreateIndexOnCoordinate(d);
        }
    }

    if (options.withSpatialIndex)
    {
        dbWrite->CreateSpatialIndex();
    }

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

//-----------------------------------------------------------------------------

CTestUtilities::EnumCoordinates::EnumCoordinates(const std::vector<DimAndRange>& dimAndRange)
    : dimAndRange(dimAndRange), enumerationCompleted(false)
{
    for (const DimAndRange& dr : dimAndRange)
    {
        this->coordinate.Set(dr.dim, dr.startIdx);
    }
}

bool CTestUtilities::EnumCoordinates::Next(SlImgDoc::TileCoordinate& coord)
{
    if (!this->enumerationCompleted)
    {
        coord = this->coordinate;
        this->Advance();
        return true;
    }
    else
    {
        return false;
    }
}

void CTestUtilities::EnumCoordinates::Advance()
{
    for (auto iter = this->dimAndRange.cbegin(); iter != this->dimAndRange.cend(); ++iter)
    {
        int coordVal;
        this->coordinate.TryGetCoordinate(iter->dim, &coordVal);
        if (coordVal < iter->startIdx + iter->size - 1)
        {
            this->coordinate.Set(iter->dim, coordVal + 1);
            break;
        }
        else
        {
            // Now we have a 'Carry', i. e. we need to increment the next coordinate.
            // Note that this works as long as we have a carry on the last coordinate,
            // in which case we are done.
            if (iter + 1 != this->dimAndRange.cend())
            {
                this->coordinate.Set(iter->dim, iter->startIdx);
            }
            else
            {
                this->enumerationCompleted = true;
            }
        }
    }
}
