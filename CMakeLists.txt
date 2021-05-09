# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.7)

project ("SqliteImgDoc")

#message(STATUS "HELLO WORLD")

# Include sub-projects.
#find_package(sqlite3 CONFIG REQUIRED)
#find_package(SQLiteCpp REQUIRED)

#add_subdirectory(extlibs/SQLite3.30.1)



#message(STATUS ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/SQLite3.30.1)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/SQLite3.30.1)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/SQLiteCpp/sqlite3)


# add SQLite3 C++ wrapper arround sqlite3 library (and sqlite3 itself provided for ease of use)
set(SQLITECPP_RUN_CPPCHECK OFF CACHE BOOL "" FORCE)
set(SQLITECPP_RUN_CPPLINT OFF CACHE BOOL "" FORCE)
set(SQLITECPP_INTERNAL_SQLITE ON CACHE BOOL "" FORCE) # here we simply instruct it to use the "bundled" SQLite-version
set(SQLITECPP_INCLUDE_SCRIPT OFF CACHE BOOL "" FORCE)
set(SQLITECPP_RUN_CPPLINT OFF CACHE BOOL "" FORCE)
set(SQLITECPP_RUN_CPPCHECK OFF CACHE BOOL "" FORCE)
set(SQLITECPP_USE_STATIC_RUNTIME OFF CACHE BOOL "" FORCE)
set(SQLITE_ENABLE_RTREE ON CACHE BOOL "" FORCE)
add_subdirectory(extlibs/SQLiteCpp)



set(LIBCZI_BUILD_CZICMD OFF CACHE BOOL "" FORCE)
set(LIBCZI_BUILD_DYNLIB OFF CACHE BOOL "" FORCE)
add_subdirectory(extlibs/libCZI)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/libCZI/Src/libCZI)


message(STATUS ${SQLITECPP_INCLUDE_DIRS} )
#include_directories(${SQLITECPP_INCLUDE_DIR})


link_directories(${SQLITE_LINK_DIR})


add_subdirectory ("SqliteImgDoc")

# GTest
enable_testing()


add_subdirectory ("SqliteImgDocTests")

add_subdirectory("CmdSqliteImgDoc")

add_subdirectory("ConvCZI")

add_subdirectory("SqliteImgDocBenchmark")
