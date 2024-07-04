# CMakeLists.txt in core directory

# Optimization flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Ofast")

# Extract the last directory name from the project path
get_filename_component(LAST_DIR_NAME "${PRJ_DIR}" NAME)
# Remove the trailing slash if it exists
string(REGEX REPLACE "/$" "" LAST_DIR_NAME "${LAST_DIR_NAME}")
# Define PROJECT_NAME
add_definitions(-DPROJECT_NAME="${LAST_DIR_NAME}")



# Collect all .cpp files in the core directory (current)
file(GLOB_RECURSE CORE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# Collect all .cpp files in the directory of the current LDSP project and subdirectories
file(GLOB_RECURSE PRJ_SRC "${CMAKE_SOURCE_DIR}/${PRJ_DIR}/*.cpp")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
add_library( # Sets the name of the library.
        ldsplite

        # Sets the library as a shared library.
        SHARED

        ${CORE_SRC} # add all .cpp files from core
        ${PRJ_SRC} # add all .cpp files from current LDSP project
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

#VIC in Android studio, all pre-built libs must be added here instead of in dedicated CMakeLists.txt
# otherwise they are not picked up by Android Studio and put in the apk...
add_library(onnxruntime SHARED IMPORTED)
set(ONNX_LIB_PATH "${CMAKE_SOURCE_DIR}/dependencies/onnxruntime/${CMAKE_ANDROID_ARCH_ABI}/${ONNX_VERSION}/libonnxruntime.so")
set_target_properties ( onnxruntime
                        PROPERTIES
                        IMPORTED_LOCATION ${ONNX_LIB_PATH})

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.
target_link_libraries( # Specifies the target library.
        ldsplite

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        libraries
        extended_oboe
        sndfile
        seasocks
        onnxruntime
        RTNeural
        android
        )


# handle assets

# Define the assets directory
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/../assets")
# Define the source resources directory
set(SOURCE_RESOURCES_DIR "${CMAKE_SOURCE_DIR}/../resources")
# Check if the resources folder exists in the assets directory
if(NOT EXISTS "${ASSETS_DIR}/resources")
    # Copy the resources folder from the source directory
    file(COPY "${SOURCE_RESOURCES_DIR}" DESTINATION "${ASSETS_DIR}")
endif()
# Get all files and directories in the assets directory
file(GLOB ASSETS_CONTENT "${ASSETS_DIR}/*")
# Remove all files and directories in assets except 'resources'
foreach(ITEM ${ASSETS_CONTENT})
    if(NOT ITEM MATCHES "${ASSETS_DIR}/resources")
        file(REMOVE_RECURSE ${ITEM})
    endif()
endforeach()

# Prepare PRJ_DIR by removing leading ./ or / if present
if(PRJ_DIR MATCHES "^\\./")
    string(REPLACE "./" "" PRJ_DIR ${PRJ_DIR})
elseif(PRJ_DIR MATCHES "^/")
    string(SUBSTRING ${PRJ_DIR} 1 -1 PRJ_DIR)
endif()
# Then remove trailing / if present
if(PRJ_DIR MATCHES ".*/$")
    string(SUBSTRING ${PRJ_DIR} 0 -1 PRJ_DIR)
endif()
# Prepend CMAKE_SOURCE_DIR
set(PRJ_DIR "${CMAKE_SOURCE_DIR}/${PRJ_DIR}")

#message("--------------Assets directory: ${ASSETS_DIR}")
#message("--------------Project directory: ${PRJ_DIR}")

# Collect all files in the PRJ_DIR
file(GLOB ALL_FILES
        "${PRJ_DIR}/*"
        "${PRJ_DIR}/*.*"
        )
# Loop through each file and copy it to ./assets if it is not a .cpp, .c, or .h file
foreach(FILE_PATH ${ALL_FILES})
    get_filename_component(FILE_NAME ${FILE_PATH} NAME)
    if (NOT FILE_NAME MATCHES "\\.(cpp|c|h)$")
        file(COPY ${FILE_PATH} DESTINATION "${ASSETS_DIR}")
    endif()
endforeach()