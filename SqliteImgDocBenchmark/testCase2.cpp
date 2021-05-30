#include "testCase2.h"
#include "inc_sqliteimgdoc.h"
#include <chrono>
#include <sstream>
#include <random>

using namespace std;
using namespace std::chrono;
using namespace SlImgDoc;

TestCase2::TestCase2(const ParametersTestCase2& params)
    : rowCount(params.rowCount),
    columnCount(params.columnCount),
    tileWidth(params.tileWidth),
    tileHeight(params.tileHeight)
{
}

BenchmarkItem TestCase2::RunTest1()
{
    // Get starting timepoint
    auto start = high_resolution_clock::now();

    auto db = this->CreateDb(true, true, true);

    // Get ending timepoint
    auto stop = high_resolution_clock::now();

    BenchmarkItem item;
    stringstream ss;
    ss.imbue(std::locale(""));
    ss << "add " << this->columnCount << "x" << this->rowCount << " tiles in regular grid (w/ transaction, w/ index, w/ spatial index)";
    item.benchmarkName = ss.str();
    ss = stringstream();
    ss.imbue(std::locale(""));
    ss << "Create a database (in memory) with " << this->columnCount <<  "x" << this->rowCount << " = " << this->columnCount * this->rowCount << " tiles. An index for dimension 'M' is created, and a spatial index."<<
        "The information is added within a single transaction.";
    item.explanation = ss.str();
    item.executionTime = (stop - start);
    return item;
}

BenchmarkItem TestCase2::RunTest2()
{
    // Get starting timepoint
    auto start = high_resolution_clock::now();

    auto db = this->CreateDb(true, false, false);

    // Get ending timepoint
    auto stop = high_resolution_clock::now();

    BenchmarkItem item;
    stringstream ss;
    ss.imbue(std::locale(""));
    ss << "add " << this->columnCount << " x " << this->rowCount << " = " << this->columnCount * this->rowCount << " tiles in regular grid (w/ transaction, w/o index, w/o spatial index)";
    item.benchmarkName = ss.str();
    ss = stringstream();
    ss.imbue(std::locale(""));
    ss << "Create a database (in memory) with " << this->columnCount << "x" << this->rowCount << " = " << this->columnCount * this->rowCount << " tiles. No indexes are created. " <<
        "The information is added within a single transaction.";
    item.explanation = ss.str();
    item.executionTime = (stop - start);
    return item;
}

BenchmarkItem TestCase2::RunTest3()
{
    const int NumberOfQueries = 10000;
    auto db = this->CreateDb(true, true, true);
    auto read = db->GetReader();

    auto queryRectangles = this->GenerateRandomQueryRects(NumberOfQueries, this->tileWidth * 1.5, this->tileHeight * 1.5);

    // Get starting timepoint
    auto start = high_resolution_clock::now();

    for (auto& queryRect : queryRectangles)
    {
        auto result = read->GetTilesIntersectingRect(queryRect);
    }

    // Get ending timepoint
    auto stop = high_resolution_clock::now();

    BenchmarkItem item;
    stringstream ss;
    ss.imbue(std::locale(""));
    ss << "query " << this->columnCount << " x " << this->rowCount << " tiles in regular grid (w/ spatial index)";
    item.benchmarkName = ss.str();
    ss = stringstream();
    ss.imbue(std::locale(""));
    ss << "Using a database (in memory) with " << this->columnCount << "x" << this->rowCount << " = " << this->columnCount * this->rowCount << " tiles and a spatial index, " <<
        NumberOfQueries << " random queries for a rect are run.";
    item.explanation = ss.str();
    item.executionTime = (stop - start);
    return item;
}

BenchmarkItem TestCase2::RunTest4()
{
    const int NumberOfQueries = 500;
    auto db = this->CreateDb(true, false, false);
    auto read = db->GetReader();

    auto queryRectangles = this->GenerateRandomQueryRects(NumberOfQueries, this->tileWidth * 1.5, this->tileHeight * 1.5);

    // Get starting timepoint
    auto start = high_resolution_clock::now();

    for (auto& queryRect : queryRectangles)
    {
        auto result = read->GetTilesIntersectingRect(queryRect);
    }

    // Get ending timepoint
    auto stop = high_resolution_clock::now();

    BenchmarkItem item;
    stringstream ss;
    ss.imbue(std::locale(""));
    ss << "query " << this->columnCount << " x " << this->rowCount << " tiles in regular grid (w/o spatial index)";
    item.benchmarkName = ss.str();
    ss = stringstream();
    ss.imbue(std::locale(""));
    ss << "Using a database (in memory) with " << this->columnCount << "x" << this->rowCount << " = " << this->columnCount * this->rowCount << " tiles and without a spatial index, " <<
        NumberOfQueries << " random queries for a rect are run.";
    item.explanation = ss.str();
    item.executionTime = (stop - start);
    return item;
}

std::vector<SlImgDoc::RectangleD> TestCase2::GenerateRandomQueryRects(int count, double width, double height)
{
    default_random_engine generator;

    int widthOfMosaic = this->columnCount * this->tileWidth;
    int heightOfMosaic = this->rowCount * this->tileHeight;
    uniform_real_distribution<double> distPosX(0, widthOfMosaic- 1);
    uniform_real_distribution<double> distPosY(0, heightOfMosaic - 1);

    vector<RectangleD> result;
    result.reserve(count);
    for (int i=0;i<count;++i)
    {
        result.emplace_back(RectangleD{ distPosX(generator),distPosY(generator),width,height });
    }

    return result;
}

std::shared_ptr<SlImgDoc::IDb> TestCase2::CreateDb(bool withTransaction, bool createIndices, bool withSpatialIndex)
{
    CreateOptions opts;
    opts.dbFilename = ":memory:";
    opts.dimensions.emplace('M');
    opts.sizeOfDataBinHdrField = 4;
    opts.createSpatialIndex = withSpatialIndex;
    auto db = IDbFactory::CreateNew(opts);
    auto dbWrite = db->GetWriter();

    if (createIndices)
    {
        dbWrite->CreateIndexOnCoordinate('M');
    }

    if (withTransaction)
    {
        dbWrite->BeginTransaction();
    }

    for (int r = 0; r < this->rowCount; ++r)
    {
        for (int c = 0; c < this->columnCount; ++c)
        {
            LogicalPositionInfo posInfo;
            posInfo.posX = c * this->tileWidth;
            posInfo.posY = r * this->tileHeight;
            posInfo.width = this->tileWidth;
            posInfo.height = this->tileHeight;
            posInfo.pyrLvl = 0;
            TileBaseInfo tileBaseInfo;
            tileBaseInfo.pixelWidth = this->tileWidth;;
            tileBaseInfo.pixelHeight = this->tileWidth;;
            tileBaseInfo.pixelType = PixelType::GRAY8;

            // and now we add a coordinate which has dimension 'M'
            TileCoordinate tc({ { 'M', r * this->columnCount + c } });
            CDataObjCustom dataCustom(4, 1);

            dbWrite->AddTile(&tc, &posInfo, &tileBaseInfo, DataTypes::CUSTOM, &dataCustom);
        }
    }

    if (withTransaction)
    {
        dbWrite->CommitTransaction();
    }

    return db;
}