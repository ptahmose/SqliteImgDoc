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
#include "ITileInfoQueryClause.h"
#include "TileInfoQueryClause.h"
#include "DataTypes.h"
#include "DataObjImpl.h"
#include "SqliteImgDocException.h"
#include "CoordinateData.h"

namespace SlImgDoc
{
    typedef long long dbIndex;

    /// This interface is used when a method is returning a blob. It will be called, passing in the data.
    /// The IBlobData-object can then decide how to deal with the data.
    class IBlobData
    {
    public:

        /// Sets the content of the blob-object.
        /// \param ptrData Pointer to the data.
        /// \param size    The size of the data.
        /// \returns True if it succeeds, false if it fails.
        virtual bool Set(const void* ptrData, size_t size) = 0;
    };

    /// The parameters and options to be used when creating a new sqlite-image-document.
    class CreateOptions
    {
    public:
        const int DefaultSizeOfDataBinHdrField = 32;
    public:
        /// Initializes a new instance of the CreateOptions class with default parameters.
        CreateOptions() : sizeOfDataBinHdrField(DefaultSizeOfDataBinHdrField)
        {
        }

        /// The filename of the SQLite-database to create. Note that this parameter is passed directly to SQLite, allowing for 
        /// additional functionality (like in-memory database) - cf. https://sqlite.org/inmemorydb.html, https://sqlite.org/uri.html.
        /// The string is expected in UTF-8 encoding.
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

    /// The options and parameters for opening an existing document.
    class OpenOptions
    {
    public:
        /// The filename of the SQLite-database to open. Note that this parameter is passed directly to SQLite, allowing for 
        /// additional functionality (like in-memory database) - cf. https://sqlite.org/inmemorydb.html, https://sqlite.org/uri.html.
        /// The string is expected in UTF-8 encoding.
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

    class SQLITEIMGDOC_API IDbIndexManagement
    {
    public:
        virtual void DropIndexOnCoordinate(TileDim dim) = 0;
        virtual void CreateIndexOnCoordinate(TileDim dim) = 0;

        virtual ~IDbIndexManagement() = default;
    };

    /// A sqliteimgdoc api. see https://stackoverflow.com/questions/11205230/virtual-inheritance-and-interfaces
    class SQLITEIMGDOC_API IDbWrite : public IDbWriteCommon, public virtual IDbWriteTransaction, public virtual IDbIndexManagement
    {
    public:
        virtual dbIndex AddTile(const ITileCoordinate* coord, const LogicalPositionInfo* info, const TileBaseInfo* tileInfo, SlImgDoc::DataTypes datatype, const IDataObjBase* data) = 0;

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

    class SQLITEIMGDOC_API IDbReadDbInfo
    {
    public:

        /// Query if for the specified dimension there is an index available.
        /// \param dim The dimension to check.
        /// \returns True if dimension is indexed, false if not.
        virtual bool IsDimensionIndexIndexed(TileDim dim) = 0;

        virtual ~IDbReadDbInfo() = default;
    };

    /// The "read interface" (the common part for both 2D and 3D documents).
    class SQLITEIMGDOC_API IDbReadCommon
    {
    public:
        /// Reads "per tile data".
        /// \param idx     The index of the tile to read "per tile data" for.
        /// \param columns The list of column-names for the "per tile data" to be queried.
        /// \param func    Functor which will be called reporting the requested data. The return value determines whether to continue to report data.
        virtual void ReadPerTileData(dbIndex idx, const std::vector<std::string>& columns, std::function<bool(const SlImgDoc::KeyVariadicValuePair&)> func) = 0;

        /// Query the tiles table. The 
        /// \param clause        The query clause (dealing with dimension indexes).
        /// \param tileInfoQuery The query clause (dealing with other "per tile data").
        /// \param func          A functor which we will called, passing in the indexs of tiles matching the query. If the functor returns false, the enumeration is canceled, and no
        ///                      more calls to the functor will occur any more.
        virtual void Query(const IDimCoordinateQueryClause* clause, const ITileInfoQueryClause* tileInfoQuery, std::function<bool(dbIndex)> func) = 0;


        virtual void EnumPerTileColumns(const std::function<bool(const ColumnDescription&)>& func) const = 0;

        virtual TileCoordinateBounds QueryDimensionBounds() = 0;

        virtual ~IDbReadCommon() = default;

        std::vector<dbIndex> Query(const IDimCoordinateQueryClause* clause);
        std::vector<dbIndex> Query(const IDimCoordinateQueryClause* clause, const ITileInfoQueryClause* tileInfoQuery);
        std::vector<ColumnDescription> GetPerTileColumns() const
        {
            std::vector<ColumnDescription> colDescr;
            this->EnumPerTileColumns([&](const ColumnDescription& cd)->bool {colDescr.emplace_back(cd); return true; });
            return colDescr;
        }

        void Query(const IDimCoordinateQueryClause* clause, std::function<bool(dbIndex)> func)
        {
            this->Query(clause, nullptr, func);
        }
    };

    /// The interface for reading databases containing 2D-subblocks.
    class SQLITEIMGDOC_API IDbRead : public IDbReadCommon, public IDbReadDbInfo
    {
    public:
        /// Reads the tile information for the specified index.
        /// \param          idx   The db-index specifying the tile.
        /// \param [in,out] coord If non-null, the coordinate information will be put here.
        /// \param [in,out] info  If non-null, the local tile information will be put here.
        virtual void ReadTileInfo(dbIndex idx, SlImgDoc::TileCoordinate* coord, LogicalPositionInfo* info) = 0;

        virtual void ReadTileData(dbIndex idx, TilePixelInfo* pixelInfo, IBlob* data) = 0;

        virtual void GetTilesIntersectingRect(const RectangleD& rect, std::function<bool(dbIndex)> func) = 0;
        virtual void GetTilesIntersectingWithLine(const LineThruTwoPointsD& line, std::function<bool(dbIndex)> func) = 0;

        std::vector<dbIndex> GetTilesIntersectingRect(const RectangleD& rect);
        std::vector<dbIndex> GetTilesIntersectingWithLine(const LineThruTwoPointsD& line);

        ~IDbRead() override = default;
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

        virtual void GetTilesIntersectingCuboid(const CuboidD& rect, std::function<bool(dbIndex)> func) = 0;
        virtual void GetTilesIntersectingWithPlane(const Plane_NormalAndDistD& plane, std::function<bool(dbIndex)> func) = 0;

        std::vector<dbIndex> GetTilesIntersectingCuboid(const CuboidD& cuboid);
        std::vector<dbIndex> GetTilesIntersectingWithPlane(const Plane_NormalAndDistD& plane);

        ~IDbRead3D() override = default;
    };

    /// A Db-object represents an attached database.
    class SQLITEIMGDOC_API IDb
    {
    public:
        /// Try to get a "write object" for a 2D-document. This method may return an empty shared_ptr
        /// if such an object cannot be constructed.
        /// \returns The writer-object (for 2D-document).
        virtual std::shared_ptr<IDbWrite> GetWriter() = 0;

        /// Try to get a "read object" for a 2D-document. This method may return an empty shared_ptr
        /// if such an object cannot be constructed.
        /// \returns The read-object (for 2D-document).
        virtual std::shared_ptr<IDbRead> GetReader() = 0;

        /// Try to get a "write object" for a 2D-document. This method may return an empty shared_ptr
        /// if such an object cannot be constructed.
        /// \returns The writer-object (for 3D-document).
        virtual std::shared_ptr<IDbWrite3D> GetWriter3D() = 0;

        /// Try to get a "read object" for a 3D-document. This method may return an empty shared_ptr
        /// if such an object cannot be constructed.
        /// \returns The read-object (for 3D-document).
        virtual std::shared_ptr<IDbRead3D> GetReader3D() = 0;

        virtual ~IDb() = default;
    };

    /// Factory functions for creating a database-object are found here.
    class SQLITEIMGDOC_API IDbFactory
    {
    public:
        static std::shared_ptr<IDb> CreateNew(const CreateOptions& opts);
        static std::shared_ptr<IDb> CreateNew3D(const CreateOptions& opts);

        /// Opens an existing sqlite-image-document file (as specified with the OpenOptions).
        /// If the document cannot be opened, an empty shared_ptr is returned.
        /// \param opts Options for controlling the operation.
        /// \returns The newly created db-object if the operation was successful; an empty shared_ptr otherwise.
        static std::shared_ptr<IDb> OpenExisting(const OpenOptions& opts);

    };
}

#include "BlobData.h"
