file(GLOB_RECURSE EXTENDED_OBOE_SOURCES CONFIGURE_DEPENDS *.cpp)

add_library(extended_oboe STATIC ${EXTENDED_OBOE_SOURCES})
target_include_directories(extended_oboe PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(extended_oboe oboe::oboe)