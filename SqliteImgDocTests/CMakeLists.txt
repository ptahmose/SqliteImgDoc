# CMakeList.txt : CMake project for CMakeProject1, include source and define
# project specific logic here.
#
#cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable( SqliteImgDocTests 
                    TileCoordinateTests.cpp 
                    ReadTests.cpp 
                    SpatialReadTests.cpp
                    utilitiesTests.cpp)

# Link test executable against gtest & gtest_main
target_link_libraries(SqliteImgDocTests  ${GTEST_BOTH_LIBRARIES} SqliteImgDoc SQLiteCpp sqlite3)

if(UNIX AND NOT APPLE)
   # Linux
   target_link_libraries(SqliteImgDocTests SQLiteCpp pthread dl)
elseif(UNIX AND APPLE)
   # Mac OS
   target_link_libraries(SqliteImgDocTests SQLiteCpp pthread)
endif()



##target_link_libraries(SqliteImgDocTests PRIVATE SQLiteCpp)

set_target_properties(SqliteImgDocTests PROPERTIES
           CXX_STANDARD 11
           CXX_EXTENSIONS OFF)


add_test( SqliteImgDocTests  SqliteImgDocTests  )