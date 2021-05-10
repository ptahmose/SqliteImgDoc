#pragma once
#include "benchmarkitem.h"
#include <memory>
#include "inc_sqliteimgdoc.h"

class TestCase1
{
private:
    int zCount;
    int tCount;
public:
    TestCase1(int zCount, int tCount);

    BenchmarkItem RunTest1();
    BenchmarkItem RunTest2();
    BenchmarkItem RunTest3();

    BenchmarkItem RunTest4();
    BenchmarkItem RunTest5();
private:
    std::shared_ptr<SlImgDoc::IDb> CreateDb(bool withTransaction,bool createIndices);
    std::vector<SlImgDoc::CDimCoordinateQueryClause> GenerateRandomSingeCoordinateQueryClauses(int count);
};