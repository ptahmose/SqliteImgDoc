# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.7)

#set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")

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

find_package(GTest QUIET)
if ("${GTest_FOUND}")
    message(STATUS "Found GTest, using it.")
else()
    message(STATUS "Did not find package GTest, will attempt to fetch it locally.")

    include(FetchContent)

    FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG        release-1.10.0
    )
    FetchContent_GetProperties(googletest)
    if(NOT googletest_POPULATED)
        FetchContent_Populate(googletest)
		add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
		set(GTEST_INCLUDE_DIRS ${googletest_SOURCE_DIR}/googletest/include)
		set(GTEST_BOTH_LIBRARIES ${CMAKE_BINARY_DIR}/lib/libgtest.a ${CMAKE_BINARY_DIR}/lib/libgtest_main.a)    
    endif()

endif()

# https://stackoverflow.com/questions/13513905/how-to-set-up-googletest-as-a-shared-library-on-linux
# sudo apt-get install libgtest-dev
# cd /usr/src/gtest
# sudo cmake CMakeLists.txt
# sudo make
# sudo make install
# copy or symlink libgtest.a and libgtest_main.a to your /usr/lib folder
# sudo cp *.a /usr/lib

#[[
include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.10.0
)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
  message(STATUS ${googletest_SOURCE_DIR})
  message(STATUS ${googletest_BINARY_DIR})
  message(STATUS ${googletest_INCLUDE_DIR})
  include_directories(${googletest_SOURCE_DIR}/googletest/include)
  ##set(GTEST_BOTH_LIBRARIES ${googletest_BINARY_DIR}/googlemock/gtest/libgtest.a ${googletest_BINARY_DIR}/googlemock/gtest/libgtest_main.a)
  set(GTEST_BOTH_LIBRARIES ${CMAKE_BINARY_DIR}/lib/libgtest.a ${CMAKE_BINARY_DIR}/lib/libgtest_main.a)
endif()
]]

##target_link_libraries(main PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

message(STATUS "XYZ")
message(STATUS ${GTEST_INCLUDE_DIRS} )

#include_directories(${GTEST_INCLUDE_DIRS})

add_subdirectory ("SqliteImgDocTests")

add_subdirectory("CmdSqliteImgDoc")
