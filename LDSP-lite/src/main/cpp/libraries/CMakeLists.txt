# CMakeLists.txt in libraries directory

# Find all source files
file(GLOB_RECURSE ALL_SOURCES CONFIGURE_DEPENDS *.cpp *.c)

add_library(libraries OBJECT ${ALL_SOURCES})

# --------------- Include directories ---------------

# include - general
target_include_directories(libraries PUBLIC ../include)
target_include_directories(libraries PUBLIC ..)
target_include_directories(libraries PUBLIC ../dependencies/seasocks/src/main/c) # needed by core/WSServer and Gui


# include - libraries and dependencies needed by libraries
target_include_directories(libraries PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/libsndfile/include") # needed by AudioFile
target_include_directories(libraries PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/AudioFile")
target_include_directories(libraries PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Gui")
target_include_directories(libraries PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/GuiController")
target_include_directories(libraries PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/JSON")
target_include_directories(libraries PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Oscillator")
target_include_directories(libraries PUBLIC "../dependencies/onnxruntime/include/headers") # needed by OrtModel
target_include_directories(libraries PUBLIC OrtModel)

# rtneural does not need to explicitly add includes with
# target_include_directories()
# because its CMakelists.txt file propagates them to any target linking against the lib