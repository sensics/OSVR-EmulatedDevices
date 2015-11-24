#Emulated Devices project
cmake_minimum_required(VERSION 2.8.12)
project(emulatedDevices)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# GCC compilers on 64-bit machines require -fPIC for shared libraries or libs
# linked into shared libraries.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_SHARED_LIBRARY_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_SHARED_LIBRARY_CXX_FLAGS}")

#
# Third-party libraries
#
find_package(osvr REQUIRED)
add_subdirectory(vendor)

# Generate our JSON header file
osvr_convert_json(com_osvr_emulated_devices_json
    com_osvr_emulated_devices.json
    "${CMAKE_CURRENT_BINARY_DIR}/com_osvr_emulated_devices.h")

if(NOT TARGET hidapi)
    message(FATAL_ERROR "HIDAPI is required to build this project!")
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Build and install the plugin
osvr_add_plugin(com_osvr_emulated_devices
    com_osvr_emulated_devices.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/com_osvr_emulated_devices.h")
    
# Link against PluginKit
target_link_libraries(com_osvr_emulated_devices
    osvr::osvrPluginKitCpp
    hidapi)
target_compile_features(com_osvr_emulated_devices PRIVATE cxx_range_for)

add_executable(OutputDeviceData OutputDeviceData.cpp)
target_link_libraries(OutputDeviceData PRIVATE osvr::osvrClientKitCpp osvr::osvrClient)
set_property(TARGET OutputDeviceData PROPERTY CXX_STANDARD 11)
    
# Install docs, license, sample config
install(FILES
    osvr_server_config.emulatedDevices.sample.json
    DESTINATION .)