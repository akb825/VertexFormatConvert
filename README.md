# Introduction

Vertex Format Convert (VFC) is a simple library and utility for converting vertex formats. This is intended to aid in converting from raw model formats, such as from interchange formats (e.g. OBJ, COLLADA), into formats that are suitable for usage at runtime. This may include interleaving vertex data, re-mapping indices (e.g. from separate indices to a single combined index), and converting to packed formats.

Example uses include:

* Library integrated into C++ tools for model conversion to efficient runtime formats.
* Library integrated into C++ applications for on the fly conversion to runtime formats.
* Tool integrated into scripts performing model conversion. Most of the implementation can be in a higher-level language, while the tool performs the processor-intensive work.

[![Build Status](https://dev.azure.com/akb825/DeepSea/_apis/build/status/akb825.VertexFormatConvert?branchName=master)](https://dev.azure.com/akb825/DeepSea/_build/latest?definitionId=6&branchName=master)

# Dependencies

The following software is required to build VFC:

* [CMake](https://cmake.org/) 3.1 or later
* [GLM](https://glm.g-truc.net/) (required, included as a submodule)
* [RapidJSON](https://rapidjson.org/) (required for tool, included as a submodule)
* [doxygen](http://www.stack.nl/~dimitri/doxygen/) (optional)
* [gtest](https://github.com/google/googletest) (optional)

The submodules can be downloaded by running the commands

	VFC$ git submodule init
	VFC$ git submodule update

# Platforms

VFC has been built for and tested on the following platforms:

* Linux (GCC and LLVM clang)
* Windows (requires Visual Studio 2015 or later)
* Mac OS X

# Building and Installing

[CMake](https://cmake.org/) is used as the build system. The way to invoke CMake differs for different platforms.

## Linux/Mac OS X

To create a release build, execute the following commands:

	VFC$ mkdir build
	VFC$ cd build
	VFC/build$ cmake .. -DCMAKE_BUILD_TYPE=Release
	VFC/build$ make

The tests can be run by running the command:

	VFC/build$ ctest

The library and tool may then be installed by running the command:

	VFC/build$ sudo make install

## Windows

Building is generally performed through Visual Studio. This can either be done through the CMake GUI tool or on the command line. To generate Visual Studio 2017 projects from the command line, you can run the commands:

	VFC$ mkdir build
	VFC$ cd build
	VFC\build$ cmake .. -G "Visual Studio 15 2017 Win64"

Once generated, the project may be opened through Visual Studio and built as normal. The `RUN_TESTS` project may be built in order to run the tests.

In order to install the libraries and tool, run Visual Studio as administrator, perform a release build, and run the `INSTALL` project. The default installation location is `C:\Program Files\VFC`. After installation, it's recommended to place the `C:\Program Files\VFC\bin` folder on your `PATH` environment variable to run the `vfc` tool from the command line.

## Options

The following options may be used when running cmake:

### Compile Options:

* `-DCMAKE_BUILD_TYPE=Debug|Release`: Building in `Debug` or `Release`. This should always be specified.
* `-DCMAKE_INSTALL_PREFIX=path`: Sets the path to install to when running `make install`.
* `-DVFC_SHARED=ON|OFF`: Set to `ON` to build with shared libraries, `OFF` to build with static libraries. Default is `ON`.

### Enabled Builds

* `-DVFC_BUILD_TESTS=ON|OFF`: Set to `ON` to build the unit tests. `gtest` must also be found in order to build the unit tests. Defaults to `ON`.
* `-DVFC_BUILD_DOCS=ON|OFF`: Set to `ON` to build the documentation. `doxygen` must also be found in order to build the documentation. Defaults to `ON`.
* `-DVFC_BUILD_TOOL=ON|OFF`: Set to `ON` to build the tool. Defaults to `ON`.

### Miscellaneous Options:

* `-DVFC_OUTPUT_DIR=directory`: The folder to place the output files. This may be explicitly left empty in order to output to the defaults used by cmake, but this may prevent tests and executables from running properly when `VFC_SHARED` is set to `ON`. Defaults to `${CMAKE_BINARY_DIR}/output`.
* `-DVFC_EXPORTS_DIR=directory`: The folder to place the cmake exports when building. This directory can be added to the module path when embedding in other projects to be able to use the `library_find()` cmake function. Defaults to `${CMAKE_BINARY_DIR}/cmake`.
* `-DVFC_ROOT_FOLDER=folder`: The root folder for the projects in IDEs that support them. (e.g. Visual Studio or XCode) This is useful if embedding VFC in another project. Defaults to VFC.
* `-DVFC_INSTALL=ON|OFF`: Allow installation for Cuttlefish components. This can be useful when embedding in other projects to prevent installations from including Cuttlefish. For example, when statically linking into a shared library. Default is `ON`.
* `-DVFC_INSTALL_SET_RPATH=ON|OFF`: Set rpath during install for the library and tool on installation. Set to `OFF` if including in another project that wants to control the rpath. Default is `ON`.
* `-DCMAKE_OSX_DEPLOYMENT_TARGET=version`: Minimum version of macOS to target when building for Mac. Defaults to 10.11.

Once you have built and installed VFC, you can find the library by calling `find_package(VFC CONFIG)` within your CMake files. Libraries and include directories can be accessed through the `VFC_LIBRARIES` and `VFC_INCLUDE_DIRS` CMake variables.

> **Note:** In order for `find_package()` to succeed, on Windows you will need to add the path to `INSTALL_DIR/lib/cmake` to `CMAKE_PREFIX_PATH`. (e.g. `C:/Program Files/VFC/lib/cmake`) On other systems, if you don't install to a standard location, you will need to add the base installation path to `CMAKE_PREFIX_PATH`.

# Further Documentation

* [Library](lib/README.md)
* [Tool](tool/README.md)
