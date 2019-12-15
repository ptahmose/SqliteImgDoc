# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("SqliteImgDoc")

message(STATUS "HELLO WORLD")

# Include sub-projects.
find_package(sqlite3 CONFIG REQUIRED)
find_package(SQLiteCpp REQUIRED)

message(STATUS ${SQLITECPP_INCLUDE_DIRS} )
include_directories(${SQLITECPP_INCLUDE_DIR})

link_directories(${SQLITE_LINK_DIR})


add_subdirectory ("SqliteImgDoc")

# GTest
enable_testing()
find_package(GTest  REQUIRED)
##target_link_libraries(main PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

message(STATUS "XYZ")
message(STATUS ${GTEST_INCLUDE_DIRS} )

include_directories(${GTEST_INCLUDE_DIRS})

add_subdirectory ("SqliteImgDocTests")

add_subdirectory("CmdSqliteImgDoc")
