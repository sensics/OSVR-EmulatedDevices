include("${CMAKE_CURRENT_SOURCE_DIR}/hidapi.cmake")

add_library(HIDAPIpp INTERFACE)
target_include_directories(HIDAPIpp INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/HIDAPIpp")