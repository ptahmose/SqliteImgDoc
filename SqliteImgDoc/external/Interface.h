#pragma once

#include <string>
#include <memory>
#include <unordered_set>
#include "ImportExport.h"
#include "Types.h"
#include "LogicalPositionInfo.h"
#include "IDataObj.h"
#include "ITileCoordinate.h"
#include "TileCoordinate.h"
#include "IBlob.h"
#include "BlobOnHeap.h"
#include "IDimCoordinateQueryClause.h"
#include "DimCoordinateQueryClause.h"
#include "DataTypes.h"
#include "DataObjImpl.h"
#include "SqliteImgDocException.h"
#include "CoordinateData.h"

namespace SlImgDoc
{
    typedef long long dbIndex;

    class IBlobData
    {
    public:
        virtual bool Set(const void* ptrData, size_t size) = 0;
    };

    class CreateOptions
    {
    public:
        const int DefaultSizeOfDataBinHdrField = 32;
    public:
        CreateOptions() : sizeOfDataBinHdrField(DefaultSizeOfDataBinHdrField)
        {
        }

        std::string dbFilename;

        /// The set of "dimensions" (to be used as coordinate of a subblock). Allowed values are characters A-Z.
        /// Upper and lowercase characters are **not** distinguished.
        std::unordered_set<char> dimensions;

        /// Size of the DataBinHdr-field in bytes. This must be greater than zero.
        int  sizeOfDataBinHdrField;

        /// Information describing additional "per tile information" - that is, columns can be defined, allowing to
        /// give custom data for each subblock.
        PerTileDataCreateOptions perTileData;

        void SetDefaultValues()
        {
            this->sizeOfDataBinHdrField = DefaultSizeOfDataBinHdrField;
        }
    };

    class OpenOptions
    {
    public:
        std::string dbFilename;
    };

    struct TileBaseInfo
    {
        int pixelWidth;
        int pixelHeight;
        std::uint8_t pixelType;
    };

    class SQLITEIMGDOC_API IDbWriteTransaction
    {
    public:
        virtual void BeginTransaction() = 0;
        virtual void CommitTransaction() = 0;
        virtual void RollbackTransaction() = 0;
        virtual ~IDbWriteTransaction() = default;
    };

    class SQLITEIMGDOC_API IDbWriteCommon
    {
    public:
        virtual void AddPerTileData(dbIndex index, std::function<bool(int, KeyVariadicValuePair&)> funcGetData) = 0;

        virtual ~IDbWriteCommon() = default;
    };

    /// A sqliteimgdoc api. see https://stackoverflow.com/questions/11205230/virtual-inheritance-and-interfaces
    class SQLITEIMGDOC_API IDbWrite : public IDbWriteCommon, public virtual IDbWriteTransaction
    {
    public:
        virtual dbIndex AddTile(const ITileCoordinate* coord, const LogicalPositionInfo* info, const TileBaseInfo* tileInfo, SlImgDoc::DataTypes datatype, const IDataObjBase* data) = 0;
        //virtual dbIndex AddTile(const ITileCoordinate* coord, const LogicalPositionInfo* info, const IDataObjUncompressedBitmap* data) = 0;
        //virtual dbIndex AddTile(const ITileCoordinate* coord, const LogicalPositionInfo* info, const TileBaseInfo* tileInfo, const IDataObjCustom* data) = 0;

        //virtual void AddPerTileData(dbIndex index, std::function<bool(int, KeyVariadicValuePair&)> funcGetData) = 0;

        virtual ~IDbWrite() = default;
    };

    struct TileBaseInfo3D
    {
        int pixelWidth;
        int pixelHeight;
        int pixelDepth;
        std::uint8_t pixelType;
    };

    class SQLITEIMGDOC_API IDbWrite3D : public IDbWriteCommon, public virtual IDbWriteTransaction
    {
    public:
        virtual dbIndex AddBrick(const ITileCoordinate* coord, const LogicalPositionInfo3D* info, const TileBaseInfo3D* tileInfo, SlImgDoc::DataTypes datatype, const IDataObjBase* data) = 0;
        //virtual dbIndex AddBrick(const ITileCoordinate* coord, const LogicalPositionInfo3D* info, const IDataObjUncompressedBrick* data) = 0;
        //virtual dbIndex AddBrick(const ITileCoordinate* coord, const LogicalPositionInfo3D* info, const TileBaseInfo3D* tileInfo, const IDataObjCustom* data) = 0;

        virtual ~IDbWrite3D() = default;
    };

    struct TilePixelInfo
    {
        /// The width of the tile in pixels.
        int pixelWidth;

        /// The height of the tile in pixels.
        int pixelHeight;

        /// The pixel type.
        std::uint8_t pixelType;

        /// The data-type of the tile. See DataTypes for defined values.
        int dataType;

        /// The binary header - this blob contains (data-type specific) information about the pixel data.
        IBlobData* dataBinHdr;
    };

    class SQLITEIMGDOC_API IDbReadCommon
    {
    public:
        virtual void ReadPerTileData(dbIndex idx, const std::vector<std::string>& columns, std::function<bool(const SlImgDoc::KeyVariadicValuePair&)> func) = 0;
        virtual void Query(const IDimCoordinateQueryClause* clause, std::function<bool(dbIndex)> func) = 0;
        virtual void EnumPerTileColumns(const std::function<bool(const ColumnDescription&)>& func) const = 0;

        virtual ~IDbReadCommon() = default;

        std::vector<dbIndex> Query(const IDimCoordinateQueryClause* clause);
        std::vector<ColumnDescription> GetPerTileColumns() const
        {
            std::vector<ColumnDescription> colDescr;
            this->EnumPerTileColumns([&](const ColumnDescription& cd)->bool {colDescr.emplace_back(cd); return true; });
            return colDescr;
        }

    };

    /// The interface for reading databases containing 2D-subblocks.
    class SQLITEIMGDOC_API IDbRead : public IDbReadCommon
    {
    public:
        virtual void ReadTileInfo(dbIndex idx, SlImgDoc::TileCoordinate* coord, LogicalPositionInfo* info) = 0;
        virtual void ReadTileData(dbIndex idx, TilePixelInfo* pixelInfo, IBlob* data) = 0;

        //virtual void Query(const IDimCoordinateQueryClause* clause, std::function<bool(dbIndex)> func) = 0;

        //virtual void ReadPerTileData(dbIndex idx, const std::vector<std::string>& columns, std::function<bool(const SlImgDoc::KeyVariadicValuePair&)> func) = 0;

        virtual void GetTilesIntersectingRect(const RectangleD& rect, std::function<bool(dbIndex)> func) = 0;
        virtual void GetTilesIntersectingWithLine(const LineThruTwoPointsD& line, std::function<bool(dbIndex)> func) = 0;

        std::vector<dbIndex> GetTilesIntersectingRect(const RectangleD& rect);
        std::vector<dbIndex> GetTilesIntersectingWithLine(const LineThruTwoPointsD& line);
        //std::vector<dbIndex> Query(const IDimCoordinateQueryClause* clause);

        virtual ~IDbRead() {};
    };

    struct TilePixelInfo3D
    {
        int pixelWidth;         ///< Width of the brick tile in pixels.
        int pixelHeight;        ///< Height of the brick tile in pixels.
        int pixelDepth;         ///< Depth of the brick tile in pixels.
        std::uint8_t pixelType; ///< The pixel type of the brick tile.
        int dataType;           ///< The data type of the brick tile.
        IBlobData* dataBinHdr;  ///< If non-null, pointer to an object which will receive the DataBinHdr.
    };

    /// The interface for reading databases containing 3D-subblocks.
    class SQLITEIMGDOC_API IDbRead3D : public IDbReadCommon
    {
    public:
        virtual void ReadTileInfo(dbIndex idx, SlImgDoc::TileCoordinate* coord, LogicalPositionInfo3D* info) = 0;
        virtual void ReadTileData(dbIndex ix, TilePixelInfo3D* pixelInfo, IBlob* data) = 0;

        //virtual void Query(const IDimCoordinateQueryClause* clause, std::function<bool(dbIndex)> func) = 0;

        virtual void GetTilesIntersectingCuboid(const CuboidD& rect, std::function<bool(dbIndex)> func) = 0;
        virtual void GetTilesIntersectingWithPlane(const Plane_NormalAndDistD& plane, std::function<bool(dbIndex)> func) = 0;

        std::vector<dbIndex> GetTilesIntersectingCuboid(const CuboidD& cuboid);
        std::vector<dbIndex> GetTilesIntersectingWithPlane(const Plane_NormalAndDistD& plane);
        std::vector<dbIndex> Query(const IDimCoordinateQueryClause* clause);

        virtual ~IDbRead3D() = default;
    };

    class SQLITEIMGDOC_API IDb
    {
    public:
        virtual std::shared_ptr<IDbWrite> GetWriter() = 0;
        virtual std::shared_ptr<IDbRead> GetReader() = 0;

        virtual std::shared_ptr<IDbWrite3D> GetWriter3D() = 0;
        virtual std::shared_ptr<IDbRead3D> GetReader3D() = 0;

        virtual ~IDb() {}
    };

    class SQLITEIMGDOC_API IDbFactory
    {
    public:
        static std::shared_ptr<IDb> CreateNew(const CreateOptions& opts);
        static std::shared_ptr<IDb> CreateNew3D(const CreateOptions& opts);
        static std::shared_ptr<IDb> OpenExisting(const OpenOptions& opts);

    };
}

#include "BlobData.h"
