#pragma once
#include "benchmarkitem.h"
#include <memory>
#include "inc_sqliteimgdoc.h"
#include "commandlineoptions.h"

class TestCase2
{
private:
    int rowCount;
    int columnCount;
    int tileWidth;
    int tileHeight;
public:
    TestCase2(const ParametersTestCase2& params);

    BenchmarkItem RunTest1();
    BenchmarkItem RunTest2();
    BenchmarkItem RunTest3();
    BenchmarkItem RunTest4();
private:
    std::shared_ptr<SlImgDoc::IDb> CreateDb(bool withTransaction, bool createIndices, bool withSpatialIndex);
    //std::vector<SlImgDoc::CDimCoordinateQueryClause> GenerateRandomSingeCoordinateQueryClauses(int count);
    std::vector<SlImgDoc::RectangleD> GenerateRandomQueryRects(int count, double width, double height);
};