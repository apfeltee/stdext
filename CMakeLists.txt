
###!!
# to prepare build-directory for scan-build, do something like this:
#
#   cmake -DCMAKE_CXX_COMPILER="/usr/share/clang-analyzer/scan-build/c++-analyzer" ..
#
# then run
#
#  scan-build make
#
#

## needed so cmake defines WIN32 again
## todo: remove this some time
set(CMAKE_LEGACY_CYGWIN_WIN32 0)

cmake_minimum_required(VERSION 2.6)
project(LibStdExt)

## extend the cmake module path to include
## our own modules
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_SOURCE_DIR}/tools/cmake/modules"
)
include(InstallRequiredSystemLibraries)
include(CheckIncludeFiles)
include(CheckIncludeFileCXX)
include(CheckFunctionExists)
include(CheckSymbolExists)
include(CheckLibraryExists)
include(CheckCXXSourceCompiles)

## check for libffi
#find_package(LibFFI)

## this will be the name for the library
set(LibName "stdext")

# 'Debug' or 'Release' mode
set(CMAKE_BUILD_TYPE Debug)

## preliminary checks
#check_function_exists("usleep" HAVE_POSIX_USLEEP_FUNCTION)
#check_function_exists("mkdir"  HAVE_POSIX_MKDIR_FUNCTION)

## our sourcefiles
file(GLOB SourceFiles      "src/*.cpp")
#file(GLOB SourceFiles_FFI  "src/3rdparty/ffi*.cpp")
#file(GLOB SourceFiles_Arch "src/archive*.cpp")

# check for libarchive by searching for archive_entry.h
# if archive_entry.h exist, then archive.h will (most definitely) too
CHECK_INCLUDE_FILE_CXX("archive_entry.h" HAVE_LIBARCHIVE)
if(HAVE_LIBARCHIVE)
    message(STATUS "libarchive appears to be available!")
    set(SourceFiles ${SourceFiles} ${SourceFiles_Arch})
    add_definitions("-larchive")
else()
    message(STATUS "libarchive could NOT be found!")
endif()

## add ffi wrapper sources, if libffi exists
#if(LIBFFI_FOUND)
#    set(SourceFiles ${SourceFiles} ${SourceFiles_FFI})
#    set(LinkerFlags ${LinkerFlags} ${LIBFFI_LIBRARIES})
#    include_directories(${LIBFFI_INCLUDE_DIRS})
#    add_definitions(${LIBFFI_DEFINITIONS})
#endif()

## add the include directory to the #include path...
include_directories("include")

## add a few important flags...
add_definitions("-DLIBSTDEXT_USING_CMAKE")
add_definitions("-std=c++11")
#add_definitions("-Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic")
add_definitions(
    -Wall -Wextra -Wformat-nonliteral -Wcast-align -Wpointer-arith
    -Wmissing-declarations -Winline -Wundef -Wcast-qual -Wshadow
    -Wwrite-strings -Wno-unused-parameter -Wfloat-equal -pedantic
    -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self
    -Wlogical-op -Wmissing-declarations -Wnoexcept
    -Woverloaded-virtual -Wredundant-decls
    -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5
    -Wswitch-default -Wundef -Wno-unused

    #-Wold-style-cast
    #-Wsign-conversion

    # the following flags are needed because '-ansi' seems to disable
    # __cplusplus for some reason
    #-D__cplusplus=201103L
)

#if(CMAKE_BUILD_TYPE EQUAL Release)
#    add_definitions("-O3")
#endif()

## build and link the library
add_library(${LibName} STATIC ${SourceFiles})
target_link_libraries(${LibName})

## knocketh thyself out, for we are done
