# CMakeList.txt : CMake project for SqliteImgDocTests, include source and define
# project specific logic here.
#
#cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable( SqliteImgDocTests 
                    TileCoordinateTests.cpp 
                    ReadTests.cpp 
                    SpatialReadTests.cpp
                    SpatialBrickReadTests.cpp
                    utilitiesTests.cpp
                    DbDiscoverTests.cpp
                    CustomQueriesTests.cpp
                    DbCreateDbTests.cpp
                    PerTilesDataTests.cpp
                    pch.cpp
                    pch.h)

target_include_directories(SqliteImgDocTests PRIVATE ${GTEST_INCLUDE_DIRS})

# Link test executable against gtest & gtest_main
target_link_libraries(SqliteImgDocTests  ${GTEST_BOTH_LIBRARIES} SqliteImgDoc SQLiteCpp sqlite3)

if(UNIX AND NOT APPLE)
   # Linux
   target_link_libraries(SqliteImgDocTests SQLiteCpp pthread dl)
elseif(UNIX AND APPLE)
   # Mac OS
   target_link_libraries(SqliteImgDocTests SQLiteCpp pthread)
endif()

# Cygwin seems to need some special treatment -> https://stackoverflow.com/questions/18784112/googletest-1-6-with-cygwin-1-7-compile-error-fileno-was-not-declared-in-this
 if(NOT CYGWIN)
    set_target_properties(SqliteImgDocTests PROPERTIES
               CXX_STANDARD 11
               CXX_EXTENSIONS OFF)
else()
    set_target_properties(SqliteImgDocTests PROPERTIES
               CXX_STANDARD 11
               CXX_EXTENSIONS ON)
endif()


add_test( SqliteImgDocTests  SqliteImgDocTests  )