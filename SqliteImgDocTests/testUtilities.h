#pragma once

#include "../SqliteImgDoc/external/Interface.h"

class CTestUtilities
{
public:
    static std::shared_ptr<SlImgDoc::IDb> CreateMosaicTestDatabaseWithSpatialIndex(int rows, int columns, int sizeX, int sizeY);
    static std::shared_ptr<SlImgDoc::IDb> CreateMosaicTestDatabaseWithoutSpatialIndex(int rows, int columns, int sizeX, int sizeY);
    static bool CheckIfSetsAreEqual(std::vector<SlImgDoc::LogicalPositionInfo>& a, std::vector<SlImgDoc::LogicalPositionInfo>& b);
private:
    static std::shared_ptr<SlImgDoc::IDb> CreateMosaicTestDatabase(bool withSpatialIndex, int rows, int columns, int sizeX, int sizeY);
};