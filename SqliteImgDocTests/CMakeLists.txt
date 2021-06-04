# CMakeList.txt : CMake project for SqliteImgDocTests, include source and define
# project specific logic here.
#
#cmake_minimum_required (VERSION 3.8)


find_package(GTest QUIET)
find_package(GMock QUIET)

if ("${GTest_FOUND}" AND "${GMock_FOUND}")
#if ("${GTest_FOUND}")
    message(STATUS "Found GTest, using it.")
else()
    message(STATUS "Did not find package GTest, will attempt to fetch it locally.")

include(FetchContent)

 FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      #GIT_TAG        release-1.10.0
      GIT_TAG        4c8e6a9fe1c8dbd96f244f076c369f44414d8d32 # this is known to work (and contains the fix below)
    )
    FetchContent_GetProperties(googletest)
    if(NOT googletest_POPULATED)
        # Prevent overriding the parent project's compiler/linker
        # settings on Windows -> https://github.com/google/googletest/blob/master/googletest/README.md
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_Populate(googletest)

#[[
        # we need to patch this file for solving a compilation-error on OpenBSD -> https://github.com/google/googletest/pull/2810
        file(COPY patch_gtest/gtest-port.cc DESTINATION ${googletest_SOURCE_DIR}/googletest/src)
]]
	  	  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
		    set(GTEST_INCLUDE_DIRS ${googletest_SOURCE_DIR}/googletest/include)
		    set(GTEST_BOTH_LIBRARIES ${CMAKE_BINARY_DIR}/lib/libgtest.a ${CMAKE_BINARY_DIR}/lib/libgtest_main.a)    
    endif()
endif()


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
                    "QueryBuildUtilsTests.cpp"
                    pch.cpp
                    pch.h 
                    "IndexTests.cpp"
                    "SpatialQueryNoSpatialIndex.cpp"
                    testUtilities.h
                    testUtilities.cpp
                    ReadSpatialAndOtherClausesTests.cpp)

target_include_directories(SqliteImgDocTests PRIVATE ${GTEST_INCLUDE_DIRS})

# Link test executable against gtest & gtest_main

if(UNIX AND NOT APPLE)
   # Linux
   message(STATUS "****** UNIX AND NOT APPLE ******")
   target_link_libraries(SqliteImgDocTests PRIVATE gtest gmock gtest_main SqliteImgDoc sqlite3 SQLiteCpp  ${CMAKE_THREAD_LIBS_INIT}  ${CMAKE_DL_LIBS})
elseif(UNIX AND APPLE)
   # Mac OS
   target_link_libraries(SqliteImgDocTests PRIVATE gtest gmock gtest_main SqliteImgDoc sqlite3 SQLiteCpp  pthread dl)
else()
   target_link_libraries(SqliteImgDocTests PRIVATE gtest gmock gtest_main SqliteImgDoc sqlite3 SQLiteCpp)
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


add_test( SqliteImgDocTests  SqliteImgDocTests )
