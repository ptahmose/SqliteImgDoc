#include "testCase1.h"
#include "inc_sqliteimgdoc.h"
#include <chrono>
#include <random>

using namespace std;
using namespace std::chrono;
using namespace SlImgDoc;

TestCase1::TestCase1(int zCount, int tCount)
    : zCount(zCount), tCount(tCount)
{
}

BenchmarkItem TestCase1::RunTest1()
{
    // Get starting timepoint
    auto start = high_resolution_clock::now();

    auto db = this->CreateDb(false, false);

    // Get ending timepoint
    auto stop = high_resolution_clock::now();

    BenchmarkItem item;
    item.benchmarkName = "add 1000T x 1000Z tiles (w/o transaction)";
    item.executionTime = (stop - start);
    return item;
}

BenchmarkItem TestCase1::RunTest2()
{
    // Get starting timepoint
    auto start = high_resolution_clock::now();

    auto db = this->CreateDb(true, false);

    // Get ending timepoint
    auto stop = high_resolution_clock::now();

    BenchmarkItem item;
    item.benchmarkName = "add 1000T x 1000Z tiles (w/ transaction)";
    item.executionTime = (stop - start);
    return item;
}

BenchmarkItem TestCase1::RunTest3()
{
    // Get starting timepoint
    auto start = high_resolution_clock::now();

    auto db = this->CreateDb(true, true);

    // Get ending timepoint
    auto stop = high_resolution_clock::now();

    BenchmarkItem item;
    item.benchmarkName = "add 1000T x 1000Z tiles (w/ transaction & indices)";
    item.executionTime = stop - start;
    return item;
}

BenchmarkItem TestCase1::RunTest4()
{
    // Get starting timepoint
    auto start = high_resolution_clock::now();

    auto db = this->CreateDb(true, false);

    db->GetWriter()->CreateIndexOnCoordinate('Z');
    db->GetWriter()->CreateIndexOnCoordinate('T');

    // Get ending timepoint
    auto stop = high_resolution_clock::now();

    BenchmarkItem item;
    item.benchmarkName = "add 1000T x 1000Z tiles (w/ transaction, indices created after adding)";
    item.executionTime = stop - start;
    return item;
}

BenchmarkItem TestCase1::RunTest5()
{
    auto db = this->CreateDb(true, true);

    auto randomCoordinateQueryClauses = this->GenerateRandomSingeCoordinateQueryClauses(1000);

    // Get starting timepoint
    auto start = high_resolution_clock::now();

    for (const CDimCoordinateQueryClause& clause : randomCoordinateQueryClauses)
    {
        auto r = db->GetReader()->Query(&clause);
        if (r.size() != 1)
        {
            throw logic_error("Expect to fine exactly one tile");
        }
    }

    // Get ending timepoint
    auto stop = high_resolution_clock::now();

    BenchmarkItem item;
    item.benchmarkName = "read constant T/constant Z (w/ indices)";
    item.executionTime = stop - start;
    return item;
}

BenchmarkItem TestCase1::RunTest6()
{
    auto db = this->CreateDb(true, false);

    auto randomCoordinateQueryClauses = this->GenerateRandomSingeCoordinateQueryClauses(1000);

    // Get starting timepoint
    auto start = high_resolution_clock::now();

    for (const CDimCoordinateQueryClause& clause : randomCoordinateQueryClauses)
    {
        auto r = db->GetReader()->Query(&clause);
        if (r.size() != 1)
        {
            throw logic_error("Expect to fine exactly one tile");
        }
    }

    // Get ending timepoint
    auto stop = high_resolution_clock::now();

    BenchmarkItem item;
    item.benchmarkName = "read constant T/constant Z (w/o indices)";
    item.executionTime = stop - start;
    return item;
}

/*private*/std::vector<SlImgDoc::CDimCoordinateQueryClause> TestCase1::GenerateRandomSingeCoordinateQueryClauses(int count)
{
    vector<CDimCoordinateQueryClause> clauses;
    clauses.reserve(count);

    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<int> distT(0, this->tCount-1);
    uniform_int_distribution<int> distZ(0, this->zCount-1);
    for (int i = 0; i < count; ++i)
    {
        CDimCoordinateQueryClause queryClause;
        int z = distZ(mt);
        queryClause.AddRangeClause('Z', IDimCoordinateQueryClause::RangeClause{ z, z });
        int t = distT(mt);
        queryClause.AddRangeClause('T', IDimCoordinateQueryClause::RangeClause{ t, t });
        clauses.emplace_back(queryClause);
    }

    return clauses;
}

/*private*/std::shared_ptr<SlImgDoc::IDb> TestCase1::CreateDb(bool withTransaction, bool createIndices)
{
    CreateOptions opts;
    opts.dbFilename = ":memory:";
    opts.dimensions.emplace('T');
    opts.dimensions.emplace('Z');
    opts.sizeOfDataBinHdrField = 4;
    auto db = IDbFactory::CreateNew(opts);
    auto dbWrite = db->GetWriter();

    if (createIndices)
    {
        dbWrite->CreateIndexOnCoordinate('T');
        dbWrite->CreateIndexOnCoordinate('Z');
    }

    // Get starting timepoint
    auto start = high_resolution_clock::now();

    if (withTransaction)
    {
        dbWrite->BeginTransaction();
    }

    for (int t = 0; t < this->tCount; ++t)
    {
        for (int z = 0; z < this->zCount; ++z)
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
            TileCoordinate tc({ { 'T', t },{ 'Z', z } });
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