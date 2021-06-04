cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable(SqliteImgDocBenchmark 
                  "commandlineoptions.h"
                  "commandlineoptions.cpp"  
                  "testCase1.h" 
                  "testCase1.cpp" 
                  "main.cpp" 
                  "inc_sqliteimgdoc.h" 
                  "utils.cpp" 
                  "utils.h" 
                  "testCase2.h" 
                  "testCase2.cpp" "testCase3.h" "testCase3.cpp")

# Link test executable against gtest & gtest_main
target_link_libraries(SqliteImgDocBenchmark SqliteImgDoc sqlite3 SQLiteCpp)

target_include_directories(SqliteImgDocBenchmark PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../extlibs/cxxopts/>)

if(UNIX AND NOT APPLE)
   # Linux
   target_link_libraries(SqliteImgDocBenchmark SQLiteCpp pthread dl)
elseif(UNIX AND APPLE)
   # Mac OS
   target_link_libraries(SqliteImgDocBenchmark SQLiteCpp pthread)
endif()

set_target_properties(SqliteImgDocBenchmark  PROPERTIES
           CXX_STANDARD 11
           CXX_EXTENSIONS OFF)