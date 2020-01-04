#include "pch.h"

using namespace std;
using namespace SlImgDoc;

TEST(PerTilesDataTests, Test1)
{
    CreateOptions opts;
    opts.dbFilename = ":memory:";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.dimensions.emplace('T');
    opts.dimensions.emplace('M');

    opts.perTileData.descriptions.push_back(ColumnDescription{ "AcquisitionTime","FLOAT" });
    opts.perTileData.descriptions.push_back(ColumnDescription{ "FocusPosition","FLOAT" });

    auto db = IDbFactory::CreateNew(opts);
    auto dbw = db->GetWriter();

    TileCoordinate coord({ { 'C',0 },{ 'Z',0 },{'T',0},{'M',0} });
    LogicalPositionInfo posInfo;
    posInfo.posX = posInfo.posY = 0;
    posInfo.width = 10;
    posInfo.height = 10;
    posInfo.pyrLvl = 0;
    TileBaseInfo tileBaseInfo;
    tileBaseInfo.pixelWidth = 10;
    tileBaseInfo.pixelHeight = 10;
    tileBaseInfo.pixelType = PixelType::GRAY8;
    CDataObjCustom dataCustom(1, 1);

    auto idx = dbw->AddTile(&coord, &posInfo, &tileBaseInfo, &dataCustom);

    dbw->AddPerTileData(
        idx,
        [](int no, SlImgDoc::KeyVariadicValuePair& kv)->bool
    {
        switch (no)
        {
        case 0:
            kv.Data.DataType = "FLOAT";
            kv.Name = "AcquisitionTime";
            kv.Data.doubleValue = 42;
            return true;
        case 1:
            kv.Data.DataType = "FLOAT";
            kv.Name = "FocusPosition";
            kv.Data.doubleValue = 43;
            return true;
        default:
            return false;
        }
    });

}