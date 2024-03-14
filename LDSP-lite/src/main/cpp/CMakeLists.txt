
# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.18.1)

# Project name
project("ldsplite")



# Select current LDSP project directory (do not end with '/')
# the starting location is where the current CMakeLists.txt file is
set(PRJ_DIR "examples/ML/BasicLSTM")
#set(PRJ_DIR "examples/Audio/sample_player")
#set(PRJ_DIR "examples/Fundamentals/sine")




# Add subdirectories
add_subdirectory(core)
add_subdirectory(libraries)
add_subdirectory(dependencies)
