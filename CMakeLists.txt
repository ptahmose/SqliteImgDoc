# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.7)

project ("SqliteImgDoc")

message(STATUS "HELLO WORLD")

# Include sub-projects.
#find_package(sqlite3 CONFIG REQUIRED)
#find_package(SQLiteCpp REQUIRED)
add_subdirectory(extlibs/SQLite3.30.1)



message(STATUS ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/SQLite3.30.1)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extlibs/SQLite3.30.1)





# add SQLite3 C++ wrapper arround sqlite3 library (and sqlite3 itself provided for ease of use)
# NOTE: here you can set CMake variables to avoid building Example, as well as cpplint, cppcheck...
# NOTE: or set them in the cmake commande line (see for instance provided build.bat/build.sh scripts)
set(SQLITECPP_RUN_CPPCHECK OFF CACHE BOOL "" FORCE)
set(SQLITECPP_RUN_CPPLINT OFF CACHE BOOL "" FORCE)
set(SQLITECPP_INTERNAL_SQLITE OFF CACHE BOOL "" FORCE)
add_subdirectory(extlibs/SQLiteCpp)




message(STATUS ${SQLITECPP_INCLUDE_DIRS} )
#include_directories(${SQLITECPP_INCLUDE_DIR})


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
