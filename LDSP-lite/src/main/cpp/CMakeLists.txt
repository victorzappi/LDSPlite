
# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.18.1)

# Project name
project("ldsplite")



# Select current LDSP project directory (do not end with '/')
# the starting location is where the current CMakeLists.txt file is
set(PRJ_DIR "examples/Performance/RTNeural/GuitarLSTM_Timing")
#set(PRJ_DIR "examples/Fundamentals/sine")
#set(PRJ_DIR "examples/Audio/sample_player")
#set(PRJ_DIR "examples/Gui/sliders")





# Set the C++ standard for the project, necessary to RTNeural and Eigen (problems with C++ 20)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add subdirectories
add_subdirectory(core)
add_subdirectory(libraries)
add_subdirectory(dependencies)
