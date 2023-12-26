# CMakeLists.txt in libraries directory

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS *.cpp *.c)

add_library(libraries OBJECT ${SOURCES})

# general includes
target_include_directories(libraries PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../include")
# for libraries to include other libraries
target_include_directories(libraries PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")

target_include_directories(libraries PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/libsndfile/include") # needed by AudioFile
target_include_directories(libraries PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/AudioFile")
target_include_directories(libraries PUBLIC ../dependencies/seasocks/src/main/c) # needed by Gui
target_include_directories(libraries PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Gui")
target_include_directories(libraries PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/GuiController")
target_include_directories(libraries PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/JSON")
target_include_directories(libraries PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Oscillator")
