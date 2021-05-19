cmake_minimum_required(VERSION 3.4.1)

project(libcli)

message("MINGW=${MINGW}")
message("CYGWIN=${CYGWIN}")
message("WIN32=${WIN32}")

MESSAGE(STATUS "operation system is ${CMAKE_SYSTEM}")
 
if (CMAKE_SYSTEM_NAME MATCHES "Linux")
	message(STATUS "current platform: Linux ")
	add_definitions("-D_LINUX_")
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
	message(STATUS "current platform: Windows")
	add_definitions("-DWIN32")
	if (MINGW)
		add_definitions("-DMINGW")
	endif()
	if (CYGWIN)
		add_definitions("-DCYGWIN")
	endif()
elseif (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
	message(STATUS "current platform: FreeBSD")
else ()
	message(STATUS "other platform: ${CMAKE_SYSTEM_NAME}")
endif (CMAKE_SYSTEM_NAME MATCHES "Linux")

add_definitions("-w -c -std=gnu++0x -DGTEST_ENABLE")

message("CMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}")
message("CMAKE_C_FLAGS_RELEASE=${CMAKE_C_FLAGS_RELEASE}")
message("CMAKE_SYSTEM=${CMAKE_SYSTEM}")

add_subdirectory(src)
