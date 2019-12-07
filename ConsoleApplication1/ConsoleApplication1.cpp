// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "../SqliteImgDoc/Interface.h"

using namespace SlImgDoc;
using namespace std;

class CSimpleCoord : public ITileCoordinate
{
public:
    int c, z, t, m;

public:
    virtual bool TryGetCoordinate(char dim, int* coordVal) const
    {
        int v;
        switch (tolower(dim))
        {
        case 'c':v = this->c; break;
        case 'z':v = this->z; break;
        case 't':v = this->t; break;
        case 'm':v = this->m; break;
        default:return false;
        }

        if (coordVal != nullptr)
        {
            *coordVal = v;
        }

        return true;
    }

    virtual void EnumCoordinates(std::function<bool(TileDim, int)> f) const
    {
        char d[4] = { 'C','Z','T','M' };
        for (int i = 0; i < sizeof(d) / sizeof(d[0]); ++i)
        {
            int v;
            this->TryGetCoordinate(d[i], &v);
            if (f(d[i], v) == false)
            {
                break;
            }
        }
    }
};

class CSimpleDataObjUncmp : public IDataObjUncompressedBitmap
{
private:
    BinHdrUncompressedBitmap hdr;
    uint8_t* data;
    size_t size;
public:
    CSimpleDataObjUncmp(int w, int h)
    {
        this->size = w * h;
        this->data = (uint8_t*)malloc(this->size);
        this->hdr.width = w;
        this->hdr.height = h;
        this->hdr.pixeltype = PixelType::RGB24;
        this->hdr.stride = w;

        for (int i = 0; i < this->size; ++i)
        {
            this->data[i] = (uint8_t)i;
        }
    }
    virtual void GetData(const void** p, size_t* s) const
    {
        if (p != nullptr) { *p = this->data; }
        if (s != nullptr) { *s = this->size; }
    }
    virtual const BinHdrUncompressedBitmap& GetBinHdr() const
    {
        return hdr;
    }
};

static void WriteMosaic(IDbWrite* dbw, int rows, int columns, int sizeX, int sizeY)
{
    CSimpleCoord simpleCoord = {};
    LogicalPositionInfo posInfo;
    posInfo.posX = posInfo.posY = 0;
    posInfo.width = sizeX;
    posInfo.height = sizeY;
    posInfo.pyrLvl = 0;
    CSimpleDataObjUncmp dataObj(sizeX, sizeY);

    dbw->BeginTransaction();
    for (int i = 0; i < 10; ++i)
    {
        simpleCoord.t = i;
        simpleCoord.m = 0;
        for (int r = 0; r < rows; ++r)
        {
            for (int c = 0; c < columns; ++c)
            {
                posInfo.posX = c * sizeX;
                posInfo.posY = r * sizeY;

                dbw->AddSubBlock(&simpleCoord, &posInfo, &dataObj);
                simpleCoord.m++;
            }
        }
    }

    dbw->CommitTransaction();
}

void TestRead()
{
    OpenOptions opts;
    opts.dbFilename = "D:\\test.db";
    auto read = IDbFactory::OpenExisting(opts);

    TileCoordinate tileCoord;
    LogicalPositionInfo posInfo;
    read->ReadTileInfo(15, &tileCoord, &posInfo);

    TilePixelInfo tilePixelInfo;
    BlobOnHeap data;
    read->ReadTileData(15, &tilePixelInfo, &data);
}

int main()
{
    TestRead();


    CreateOptions opts;
    opts.dbFilename = "D:\\test.db";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('Z');
    opts.dimensions.emplace('T');
    opts.dimensions.emplace('M');
    //opts.dbFilename = "C:\\temp\\test.db";

    auto dbw = IDbFactory::CreateNew(opts);
    WriteMosaic(dbw, 50, 50, 256, 256);

    /*
    CSimpleCoord simpleCoord = {};
    LogicalPositionInfo posInfo;
    posInfo.posX = posInfo.posY = 0;
    posInfo.width = posInfo.height = 100;
    posInfo.pyrLvl = 0;
    CSimpleDataObjUncmp dataObj(1000, 1000);

    dbw->BeginTransaction();
    for (int i = 0; i < 1000; ++i)
    {
        simpleCoord.t = i;
        dbw->AddSubBlock(&simpleCoord, &posInfo, &dataObj);
    }

    dbw->CommitTransaction();
    */
    std::cout << "Hello World!\n";

    delete dbw;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
