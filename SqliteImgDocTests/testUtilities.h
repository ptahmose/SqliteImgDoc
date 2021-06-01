#pragma once

#include "../SqliteImgDoc/external/Interface.h"

class CTestUtilities
{
public:
    static std::shared_ptr<SlImgDoc::IDb> CreateMosaicTestDatabaseWithSpatialIndex(int rows, int columns, int sizeX, int sizeY);
    static std::shared_ptr<SlImgDoc::IDb> CreateMosaicTestDatabaseWithoutSpatialIndex(int rows, int columns, int sizeX, int sizeY);

    struct DimAndRange
    {
        SlImgDoc::TileDim dim;
        int startIdx;
        int size;
    };

    struct MosaicTestDataBaseOptions
    {
        bool withSpatialIndex;
        bool withDimensionIndex;
        bool withMindex;
        int rows;
        int columns;
        int sizeTileX;
        int sizeTileY;
        double overlap;
        std::vector<DimAndRange> dimAndRange;
    };

    static std::shared_ptr<SlImgDoc::IDb> CreateMosaicTestDatabase(const MosaicTestDataBaseOptions& options);


    static bool CheckIfSetsAreEqual(std::vector<SlImgDoc::LogicalPositionInfo>& a, std::vector<SlImgDoc::LogicalPositionInfo>& b);
private:
    static std::shared_ptr<SlImgDoc::IDb> CreateMosaicTestDatabase(bool withSpatialIndex, int rows, int columns, int sizeX, int sizeY);


    class EnumCoordinates
    {
    private:
        const std::vector<DimAndRange>& dimAndRange;
        SlImgDoc::TileCoordinate coordinate;
        bool enumerationCompleted;
    public:
        EnumCoordinates(const std::vector<DimAndRange>& dimAndRange);
        bool Next(SlImgDoc::TileCoordinate& coord);
    private:
        void Advance();
    };

};