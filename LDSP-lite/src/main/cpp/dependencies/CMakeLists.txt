# CMakeLists.txt in dependencies directory

# apply compiler and linker optimizations for dependencies only
# remove unused code
add_compile_options(-ffunction-sections -fdata-sections)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections")
# strip debug symbols
if(NOT CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -s")
endif()


# oboe
# We include the official oboe package and extend it with the full duplex code
# NB: as of now this is not needed anymore, as the full duplex class is part of the lib...
#TODO switch to latest and complete oboe lib
find_package(oboe REQUIRED CONFIG)
add_subdirectory(oboe)

# Configuration for libsndfile's provided CMakeLists.txt
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "Build tests" FORCE)
set(BUILD_PROGRAMS OFF CACHE BOOL "Build programs" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "Build examples" FORCE)
set(ENABLE_CPACK OFF CACHE BOOL "Enable CPack" FORCE)
set(ENABLE_PACKAGE_CONFIG OFF CACHE BOOL "Generate and install package config file" FORCE)
set(INSTALL_PKGCONFIG_MODULE OFF CACHE BOOL "Generate and install pkg-config module" FORCE)
add_subdirectory(libsndfile)

# seasocks
# We are using a custom version of seasocks that builds for Android
# seasocks source: https://github.com/mattgodbolt/seasocks
# Set the options for Seasocks

set(SEASOCKS_SHARED OFF CACHE BOOL "Build seasocks as a shared library" FORCE)
set(UNITTESTS OFF CACHE BOOL "Build unittests." FORCE)
set(COVERAGE OFF CACHE BOOL "Build with code coverage enabled" FORCE)
set(SEASOCKS_EXAMPLE_APP OFF CACHE BOOL "Build the example applications." FORCE)
set(DEFLATE_SUPPORT OFF CACHE BOOL "Include support for deflate (requires zlib)." FORCE)
add_subdirectory(seasocks)


#add_subdirectory(onnxruntime)
add_subdirectory(RTNeural)