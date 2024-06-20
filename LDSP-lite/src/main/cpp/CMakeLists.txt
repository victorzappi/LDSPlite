
# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.18.1)

# Project name
project("ldsplite")

#TODO
# move ONNX and RTNeural examples to repos
# change name to app
# update examples/MachineLearning projects
# move assets automatically

#---------------------------------------------------------------------------------------------------
# Select current LDSP project directory (do not end with '/')
# the starting location is where the current CMakeLists.txt file is
set(PRJ_DIR "examples/LDSPlite-RTNeural/AutoGuitarAmp_Timing")
#set(PRJ_DIR "examples/Fundamentals/sine")
#set(PRJ_DIR "examples/Audio/sample_player")
#set(PRJ_DIR "examples/Gui/sliders")
#---------------------------------------------------------------------------------------------------


# this is needed by ONNX Runtime, it retrieves the correct version of the lib
# on the basis of the Android version running on the connected phone
# (no problem if no phone nor emulator is connected)

# Determine the script to run based on the operating system
if(UNIX)
    set(GET_ANDROID_VERSION_SCRIPT "${CMAKE_SOURCE_DIR}/get_android_version.sh")
elseif(WIN32)
    set(GET_ANDROID_VERSION_SCRIPT "${CMAKE_SOURCE_DIR}/get_android_version.bat")
else()
    message(FATAL_ERROR "Unsupported platform")
endif()
# Ensure the script is executable (Unix-like systems only)
if(UNIX)
    execute_process(COMMAND chmod +x ${GET_ANDROID_VERSION_SCRIPT})
endif()
# Run the script to get the Android version
execute_process(
        COMMAND ${GET_ANDROID_VERSION_SCRIPT}
        OUTPUT_VARIABLE ANDROID_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
# Handle case in which no phone nor emulator is connected
if(ANDROID_VERSION STREQUAL "")
    set(ANDROID_VERSION_NUMBER 0)  # Set a default value
else()
    # Convert the Android version to a number
    string(REGEX REPLACE "[^0-9]" "" ANDROID_VERSION_NUMBER ${ANDROID_VERSION})
endif()
# Perform the comparison and set the onnx var
if(ANDROID_VERSION_NUMBER GREATER_EQUAL 7)
    set(ONNX_VERSION "aboveOrEqual24")
else()
    set(ONNX_VERSION "below24")
endif()
#file(WRITE "${CMAKE_SOURCE_DIR}/onnx_version.txt" "ONNX_VERSION: ${ONNX_VERSION}\n
#ANDROID_VERSION: ${ANDROID_VERSION}\n
#ANDROID_VERSION_NUMBER: ${ANDROID_VERSION_NUMBER}")

#set(ONNX_VERSION "aboveOrEqual24") #VIC we can force the version for debugging


# Set the C++ standard for the project, necessary to RTNeural and Eigen (problems with C++ 20)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add subdirectories
add_subdirectory(core)
add_subdirectory(dependencies)
add_subdirectory(libraries) # here we have all target include directories too
