cmake_minimum_required(VERSION 3.1)
project(VFC)

# Build options
# Use if since BUILD_SHARED_LIBS defaults to unset.
if (BUILD_SHARED_LIBS)
	set(VFC_SHARED_DEFAULT ON)
else()
	set(VFC_SHARED_DEFAULT OFF)
endif()
set(VFC_SHARED ${VFC_SHARED_DEFAULT} CACHE BOOL "Build VFC using shared libraries.")

# Options for disabling portions of the build.
set(VFC_BUILD_TESTS ON CACHE BOOL "Build unit tests.")
set(VFC_BUILD_DOCS ON CACHE BOOL "Build documentation.")
set(VFC_BUILD_TOOL ON CACHE BOOL "Build the tool.")

# Misc options.
set(VFC_OUTPUT_DIR ${CMAKE_BINARY_DIR}/output CACHE PATH
	"Folder for placing the build output.")
set(VFC_EXPORTS_DIR ${CMAKE_BINARY_DIR}/cmake CACHE PATH
	"Folder for placing the cmake exports while building. Useful when embedding in other projects.")
set(VFC_ROOT_FOLDER VFC CACHE STRING
	"Root folder for the VFC projects. Usefull when embedding in other projects.")
set(VFC_INSTALL ON CACHE BOOL "Allow installation for VFC components.")
set(VFC_INSTALL_SET_RPATH ON CACHE BOOL "Set rpath for library and tool on installation.")

if (NOT CMAKE_OSX_DEPLOYMENT_TARGET)
	set(CMAKE_OSX_DEPLOYMENT_TARGET 10.11 CACHE STRING "Minimum macOS deployment version." FORCE)
endif()

if (VFC_BUILD_TESTS)
	find_package(GTest QUIET)
	if (NOT GTEST_FOUND)
		message("GTest not installed. Skipping tests.")
	endif()
endif()

if (VFC_BUILD_DOCS)
	find_package(Doxygen QUIET)
	if (NOT DOXYGEN_FOUND)
		message("Doxygon not installed. Skipping documentation.")
	endif()
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (VFC_SHARED)
	set(VFC_LIB SHARED)
else()
	set(VFC_LIB STATIC)
endif()

if (VFC_OUTPUT_DIR)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${VFC_OUTPUT_DIR})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${VFC_OUTPUT_DIR})
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${VFC_OUTPUT_DIR})
endif()

set(VFC_MAJOR_VERSION 2)
set(VFC_MINOR_VERSION 0)
set(VFC_PATCH_VERSION 4)
set(VFC_VERSION ${VFC_MAJOR_VERSION}.${VFC_MINOR_VERSION}.${VFC_PATCH_VERSION})

set(VFC_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(cmake/config.cmake)
include(cmake/install.cmake)

add_subdirectory(lib)
if (VFC_BUILD_TOOL)
	add_subdirectory(tool)
endif()

# Documentation. (populated by built libraries)
add_subdirectory(doc)
