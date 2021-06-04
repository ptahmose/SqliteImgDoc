# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.7)

project ("SqliteImgDoc")

#message(STATUS "HELLO WORLD")

# Include sub-projects.
#find_package(sqlite3 CONFIG REQUIRED)
#find_package(SQLiteCpp REQUIRED)

add_subdirectory(extlibs/sqlite3)



#message(STATUS ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/SQLite3.30.1)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/SQLite3.30.1)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/SQLiteCpp/sqlite3)

# We instruct SQLiteCpp to use "external sqlite", so we have to provide it with the path for
#  see sqlite-headers and the library. SQLiteCpp will use a find_library-call and pick up those
# variables. I couldn't figure out how to get the "destination .lib/.a-library filename in a proper
# way, and using a generator-expression ("$<TARGET_FILE:sqlite3>") did not work for me here, so
# we put together the filename here by hand.
set(SQLite3_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/sqlite3)
get_target_property(SQLITE3BINARYDIR sqlite3 BINARY_DIR)
set(SQLite3_LIBRARY    ${SQLITE3BINARYDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}sqlite3${CMAKE_STATIC_LIBRARY_SUFFIX} )
message(STATUS ***** ${SQLite3_LIBRARY} ********)

#get_target_property(TESTVAR sqlite3 BINARY_DIR)
#message(STATUS XXX***** ${TESTVAR} ********XXX)


# add SQLite3 C++ wrapper arround sqlite3 library (and sqlite3 itself provided for ease of use)
set(SQLITECPP_RUN_CPPCHECK OFF CACHE BOOL "" FORCE)
set(SQLITECPP_RUN_CPPLINT OFF CACHE BOOL "" FORCE)
set(SQLITECPP_INTERNAL_SQLITE OFF CACHE BOOL "" FORCE) # here we instruct it to NOT use the "bundled" SQLite-version
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
