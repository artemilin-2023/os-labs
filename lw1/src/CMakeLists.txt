cmake_minimum_required(VERSION 3.15)
project(lcma C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(lcma
	main.c
	archiver.c
	unarchiver.c
	fileutils.c
	password.c
)

target_include_directories(lcma PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON)
find_package(unofficial-getopt-win32 CONFIG QUIET)
if (TARGET unofficial::getopt-win32::getopt)
	target_link_libraries(lcma PRIVATE unofficial::getopt-win32::getopt)
else()
	if (NOT VCPKG_TARGET_TRIPLET)
		set(VCPKG_TARGET_TRIPLET "x64-windows")
	endif()
	set(_VCPKG_ROOT "$ENV{VCPKG_ROOT}")
	if (NOT _VCPKG_ROOT AND EXISTS "$ENV{USERPROFILE}/vcpkg")
		set(_VCPKG_ROOT "$ENV{USERPROFILE}/vcpkg")
	endif()
	set(_VCPKG_INSTALLED "${_VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}")
	find_path(GETOPT_INCLUDE_DIR NAMES getopt.h PATHS "${_VCPKG_INSTALLED}/include" NO_DEFAULT_PATH)
	find_library(GETOPT_LIBRARY NAMES getopt PATHS "${_VCPKG_INSTALLED}/lib" "${_VCPKG_INSTALLED}/debug/lib" NO_DEFAULT_PATH)
	if (NOT GETOPT_INCLUDE_DIR OR NOT GETOPT_LIBRARY)
		message(FATAL_ERROR "unofficial-getopt-win32 not found. Ensure vcpkg is integrated and package installed for ${VCPKG_TARGET_TRIPLET}.")
	endif()
	include_directories(${GETOPT_INCLUDE_DIR})
	target_link_libraries(lcma PRIVATE ${GETOPT_LIBRARY})
endif()