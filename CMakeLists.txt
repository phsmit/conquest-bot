cmake_minimum_required(VERSION 2.8)
PROJECT(conquest)

##################GTEST
# Enable ExternalProject CMake module
include(ExternalProject)

# Add gtest
ExternalProject_Add(
    googletest
    SVN_REPOSITORY http://googletest.googlecode.com/svn/trunk/
    SVN_REVISION -r 664
    TIMEOUT 10
    # Force separate output paths for debug and release builds to allow easy
    # identification of correct lib in subsequent TARGET_LINK_LIBRARIES commands
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
               -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:PATH=DebugLibs
               -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=ReleaseLibs
               -Dgtest_force_shared_crt=ON
    # Disable install step
    INSTALL_COMMAND ""
    # Wrap download, configure and build steps in a script to log output
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON)

# Specify include dir
ExternalProject_Get_Property(googletest source_dir)
include_directories(${source_dir}/include)
set(Suffix ".a")
set(Pthread "-pthread")



# Specify MainTest's link libraries
ExternalProject_Get_Property(googletest binary_dir)

##################GTEST


include_directories ( . )
add_executable( bot conquestbot.cc bot.cc util.cc io.cc strategy_bot.cc strategy.cc )

add_subdirectory( helpers )