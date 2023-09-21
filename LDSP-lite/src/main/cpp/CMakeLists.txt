
# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

# Declares and names the project.
project("ldsplite")

# Optimization flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")





# Select current LDSP project directory (must end with '/')
set(PRJ_DIR "examples/Fundamentals/sine/")




# Collect all .cpp files in the directory of the current LDSP project and subdirectories
file(GLOB_RECURSE PRJ_SRC "${PRJ_DIR}*.cpp")

# Collect all .cpp files in the dependencies directory and subdirectories
file(GLOB_RECURSE DEPENDENCIES "dependencies/oboe/**/*.cpp")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
            ldsplite

            # Sets the library as a shared library.
            SHARED

            # Provides a relative path to your source file(s).
            ldsplite-native-lib.cpp
            LDSPlite.cpp
            OboeAudioEngine.cpp
            ${PRJ_SRC} # add all .cpp files from current LDSP project

            # dependencies
            ${DEPENDENCIES}
        )

include_directories(
        include
        dependencies/oboe
)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log
        )
find_package(oboe REQUIRED CONFIG)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       ldsplite

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                       oboe::oboe
        )
