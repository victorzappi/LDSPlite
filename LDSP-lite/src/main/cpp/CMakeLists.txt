
# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.18.1)

# Project name
project("ldsplite")

#TODO
# change name to app
# update examples/MachineLearning projects

#---------------------------------------------------------------------------------------------------
# Select current LDSP project directory
# the starting location is where the current CMakeLists.txt file is
#set(PRJ_DIR "examples/MachineLearning/RTNeural")
set(PRJ_DIR "examples/Fundamentals/sine")
#set(PRJ_DIR "examples/Audio/sample_player")
#set(PRJ_DIR "examples/Gui/sliders")
#---------------------------------------------------------------------------------------------------


# this is needed by ONNX Runtime, it retrieves the correct version of the lib
# on the basis of the Android version running on the connected phone
# (no problem if no phone nor emulator is connected)

# Determine the script to run based on the operating system
if(UNIX)
    set(GET_ANDROID_API_SCRIPT "${CMAKE_SOURCE_DIR}/get_android_api.sh")
elseif(WIN32)
    set(GET_ANDROID_API_SCRIPT "${CMAKE_SOURCE_DIR}/get_android_api.bat")
else()
    message(FATAL_ERROR "Unsupported platform")
endif()
# Ensure the script is executable (Unix-like systems only)
if(UNIX)
    execute_process(COMMAND chmod +x ${GET_ANDROID_API_SCRIPT})
endif()
# Run the script to get the Android version
execute_process(
        COMMAND ${GET_ANDROID_API_SCRIPT}
        OUTPUT_VARIABLE ANDROID_API
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
# Handle case in which no phone nor emulator is connected
if(ANDROID_API STREQUAL "")
    set(ANDROID_API_NUMBER 0)  # Set a default value
else()
    # Convert the Android version to a number
    string(REGEX REPLACE "[^0-9]" "" ANDROID_API_NUMBER ${ANDROID_API})
endif()
# Perform the comparison and set the onnx var
if(ANDROID_API_NUMBER GREATER_EQUAL 24)
    set(ONNX_VERSION "aboveOrEqual24")
else()
    set(ONNX_VERSION "below24")
endif()
#file(WRITE "${CMAKE_SOURCE_DIR}/onnx_version.txt" "ONNX_VERSION: ${ONNX_VERSION}\n
#ANDROID_API: ${ANDROID_API}\n
#ANDROID_API_NUMBER: ${ANDROID_API_NUMBER}")

#set(ONNX_VERSION "aboveOrEqual24") #VIC we can force the version for debugging


# Set the C++ standard for the project, necessary to RTNeural and Eigen (problems with C++ 20)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add subdirectories
add_subdirectory(core)
add_subdirectory(dependencies)
add_subdirectory(libraries) # here we have all target include directories too
