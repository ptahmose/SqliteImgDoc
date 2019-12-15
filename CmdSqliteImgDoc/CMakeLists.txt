# CMakeList.txt : CMake project for CMakeProject1, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable(CmdSqliteImgDoc CmdSqliteImgDoc.cpp )

# Link test executable against gtest & gtest_main
target_link_libraries(CmdSqliteImgDoc   SqliteImgDoc SQLiteCpp sqlite3)

##target_link_libraries(SqliteImgDocTests PRIVATE SQLiteCpp)

set_target_properties(CmdSqliteImgDoc  PROPERTIES
           CXX_STANDARD 17
           CXX_EXTENSIONS OFF)


