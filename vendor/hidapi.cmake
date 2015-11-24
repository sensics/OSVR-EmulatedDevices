# Partially based on VRPN build system, BSL1.0 license

set(_source)
set(_libs)
set(_includes)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(NOT HIDAPI_ROOT)
	set(HIDAPI_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/hidapi")
endif()

if(NOT EXISTS "${HIDAPI_ROOT}/hidapi/hidapi.h")
	message(STATUS "HIDAPI submodule missing (git submodule update --init --recursive) and/or didn't pass alternate location as HIDAPI_ROOT")
	return()
endif()
# Local HIDAPI requirements
if(ANDROID)
	find_package(Libusb1)

	# Check to see if our submodule is new enough.
	if(NOT EXISTS "${HIDAPI_ROOT}/android/jni/Android.mk")
		message(STATUS "HIDAPI_ROOT ${HIDAPI_ROOT} is not new enough to contain Android support!")
		return()
	endif()

	if(NOT LIBUSB1_FOUND)
		message(STATUS "HIDAPI: Requires Libusb1.0, not found")
		return()
	endif()

	set(_source "${HIDAPI_ROOT}/libusb/hid.c")
	set(_includes ${LIBUSB1_INCLUDE_DIRS})
	set(_libs ${LIBUSB1_LIBRARIES})
elseif(WIN32)
	# No extra deps needed on Windows - dynamically/delay loading the dlls.
	set(HIDAPI_BACKEND_FOUND YES)
	set(_source ${CMAKE_CURRENT_SOURCE_DIR}/hidapi/windows/hid.c)
	set(_libs setupapi)

elseif(APPLE)
	find_package(MacHID)
	if(NOT MACHID_FOUND)
		message(STATUS "HIDAPI: Requires some Mac HID libraries you should have had, not found.")
		return()
	endif()
	set(_source ${CMAKE_CURRENT_SOURCE_DIR}/hidapi/mac/hid.c)
	set(_libs ${MACHID_LIBRARIES})
else()
	find_package(Libusb1)

	set(HIDAPI_LIBUDEV_FOUND NO)
	if(CMAKE_SYSTEM_NAME MATCHES "Linux")
		find_library(HIDAPI_LIBUDEV udev)
		find_path(HIDAPI_HIDRAW_INCLUDE_DIR linux/hidraw.h)
		find_path(HIDAPI_LIBUDEV_INCLUDE_DIR libudev.h)
		if(HIDAPI_LIBUDEV AND HIDAPI_HIDRAW_INCLUDE_DIR AND HIDAPI_LIBUDEV_INCLUDE_DIR)
			mark_as_advanced(HIDAPI_LIBUDEV
				HIDAPI_HIDRAW_INCLUDE_DIR
				HIDAPI_LIBUDEV_INCLUDE_DIR)
			set(HIDAPI_LIBUDEV_FOUND YES)
		endif()
	endif()
	if(HIDAPI_LIBUDEV_FOUND AND LIBUSB1_FOUND)
		option(HIDAPI_USE_UDEV_HIDRAW "Should we build against the Linux kernel, HID raw, and libudev? (Alternative is libusb1)" OFF)
		if(HIDAPI_USE_UDEV_HIDRAW)
			set(_hidapi_udev ON)
			set(_hidapi_libusb OFF)
		else()
			set(_hidapi_udev OFF)
			set(_hidapi_libusb ON)
		endif()
	elseif(HIDAPI_LIBUDEV_FOUND)
		# Only found libudev, using it.
		set(_hidapi_udev ON)
		set(_hidapi_libusb OFF)
	elseif(LIBUSB1_FOUND)
		# Only found libusb1, using it.
		set(_hidapi_udev OFF)
		set(_hidapi_libusb ON)
	else()
		message(STATUS "HIDAPI: Assuming your platform may be supported by libusb1, but libusb1 not found! (If this is Linux, we didn't find the hidraw kernel header and libudev either!)")
		return()
	endif()

	if(_hidapi_libusb)
		set(_source "${HIDAPI_ROOT}/libusb/hid.c")
		set(_includes ${LIBUSB1_INCLUDE_DIRS})
		set(_libs ${LIBUSB1_LIBRARIES})
	elseif(_hidapi_udev)
		set(_source "${HIDAPI_ROOT}/linux/hid.c")
		set(_includes ${HIDAPI_HIDRAW_INCLUDE_DIR} ${HIDAPI_LIBUDEV_INCLUDE_DIR})
		set(_libs ${HIDAPI_LIBUDEV})
	endif()
	find_library(HIDAPI_LIBRT rt)
	mark_as_advanced(HIDAPI_LIBRT)
	if(HIDAPI_LIBRT)
		list(APPEND _libs ${HIDAPI_LIBRT})
	endif()
endif()

add_library(hidapi STATIC "${HIDAPI_ROOT}/hidapi/hidapi.h" ${_source})
target_include_directories(hidapi PUBLIC "${HIDAPI_ROOT}/hidapi")
if(_libs)
	target_link_libraries(hidapi PRIVATE ${_libs})
endif()
if(_includes)
	target_include_directories(hidapi PRIVATE ${_includes})
endif()