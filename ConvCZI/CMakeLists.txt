# CMakeList.txt : CMake project for CMakeProject1, include source and define
# project specific logic here.
#
#cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable(ConvCZI 
                convczi.cpp
                inc_libczi.h
                cmdlineopts.cpp
                cmdlineopts.h
                utils.cpp
                utils.h
                inc_libczi.h)

# Link test executable against gtest & gtest_main
target_link_libraries(ConvCZI  SqliteImgDoc SQLiteCpp libCZIStatic JxrDecodeStatic)

target_include_directories(ConvCZI PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../extlibs/cxxopts/>)

##target_link_libraries(SqliteImgDocTests PRIVATE SQLiteCpp)
if(UNIX AND NOT APPLE)
   # Linux
   target_link_libraries(ConvCZI SQLiteCpp pthread dl)
elseif(UNIX AND APPLE)
   # Mac OS
   target_link_libraries(ConvCZI SQLiteCpp pthread)
endif()

set_target_properties(ConvCZI  PROPERTIES
           CXX_STANDARD 11
           CXX_EXTENSIONS OFF)


