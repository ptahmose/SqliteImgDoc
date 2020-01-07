#include "pch.h"
#include "DbBase.h"
#include <sstream>
#include <algorithm>

using namespace std;
using namespace SlImgDoc;

///*static*/const char* CDbBase::TableName_DocumentInfo = "DOCINFO";
///*static*/const char* CDbBase::TableName_TileTable = "TILES";
///*static*/const char* CDbBase::TableName_TileData = "TILEDATA";
///*static*/const char* CDbBase::VTableName_SpatialTable = "TILESPATIAL_index";


///*static*/const char* CDbBase::DocumentInfoColName_Version = "Version";
///*static*/const char* CDbBase::DocumentInfoColName_CoordinateDimensions = "CoordDims";
///*static*/const char* CDbBase::DocumentInfoColName_IndexedDimensions = "IndexedDims";

void CDbBase::CheckSizeOfBinHdrAndThrow(size_t sizeBinHdr, std::uint32_t maxSize)
{
    if (sizeBinHdr > maxSize)
    {
        stringstream ss;
        ss << "Size of 'BinHdr'-field is limited to " << maxSize << " bytes, and " << sizeBinHdr << " bytes was requested.";
        throw SqliteImgDocInvalidArgumentException(ss.str());
    }
}

void CDbBase::CheckIfAllDimensionGivenAndThrow(const std::vector<SlImgDoc::TileDim>& tileDims, const SlImgDoc::ITileCoordinate* coord)
{
    size_t countDims = 0;
    bool unknownDimFound = false;
    coord->EnumDimensions(
        [&](TileDim d)->bool
    {
        const auto f = find(tileDims.cbegin(), tileDims.cend(), d);
        if (f == tileDims.cend())
        {
            unknownDimFound = true;
            return false;
        }

        countDims++;
        return true;
    });

    if (unknownDimFound || countDims != tileDims.size())
    {
        throw SqliteImgDocInvalidArgumentException("insufficient coordinate");
    }
}