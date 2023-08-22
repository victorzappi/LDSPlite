
# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

# Declares and names the project.

project("ldsplite")

# Select current LDSP project directory

set(PRJ_DIR "examples/example2/")

# Collect all .cpp files in the directory of the current LDSP project

file(GLOB PRJ_SRC "${PRJ_DIR}*.cpp")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             ldsplite

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             wavetablesynthesizer-native-lib.cpp
             WavetableSynthesizer.cpp
             WavetableOscillator.cpp
             WavetableFactory.cpp
             OboeAudioPlayer.cpp
             ${PRJ_SRC} # add all .cpp files from current LDSP project
        )

include_directories(
        include
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