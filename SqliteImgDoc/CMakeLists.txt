# CMakeList.txt : CMake project for CMakeProject1, include source and define
# project specific logic here.
#
#cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_library (SqliteImgDoc STATIC 
						src/BlobOnHeap.cpp 
						src/CCustomQueries.cpp 
						src/CCustomQueries.h 
						src/Db.cpp 
						src/Db.h 
						src/DbBase.cpp 
						src/DbBase.h 
						src/DbCreation.cpp 
						src/DbCreation.h 
						src/DbDiscover.cpp 
						src/DbDiscover.h 
						src/DbGlobalInfo.h 
						src/DbGlobalInfo.cpp 
						src/DbRead.cpp 
						src/DbRead.h 
						src/DbWrite.cpp 
						src/DbWrite.h 
						src/dllmain.cpp 
						src/framework.h 
						src/pch.cpp 
						src/pch.h 
						src/miscutilities.h
						src/miscutilities.cpp
						external/DataBinHdr.h 
						external/IDataObj.h 
						external/BlobOnHeap.h 
						external/DataObjImpl.h 
						external/DataTypes.h 
						external/DimCoordinateQueryClause.h 
						external/IBlob.h 
						external/IDimCoordinateQueryClause.h 
						external/ImportExport.h 
						external/Interface.h 
						external/ITileCoordinate.h 
						external/LogicalPositionInfo.h 
						external/TileCoordinate.h 
						external/Types.h 
						external/utilities.h)

target_link_libraries(SqliteImgDoc PRIVATE SQLiteCpp)
#set_target_properties(SqliteImgDoc PROPERTIES CXX_STANDARD 17 CXX_EXTENSIONS OFF)
set_target_properties(SqliteImgDoc PROPERTIES CXX_STANDARD 11 CXX_EXTENSIONS OFF)

##target_compile_features(SqliteImgDoc PRIVATE cxx_std_17)
#target_compile_features(SqliteImgDoc PRIVATE CXX_STANDARD 11 CXX_EXTENSIONS OFF)


# TODO: Add tests and install targets if needed.
