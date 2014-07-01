cmake_minimum_required (VERSION 2.6)

project (lincxx_example)

include_directories (
	../include/
)

# Get all files from the include folder
set ( INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../include )
file ( GLOB INCLUDE_FILES ${INCLUDE_PATH}/*.h ${INCLUDE_PATH}/*.hpp )

# enable c++11
if (NOT ${CMAKE_CXX_COMPILER_ID} EQUAL "MSVC")
	if (CMAKE_GENERATOR STREQUAL Xcode)
		set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
		set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
	else ()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++ -g -Wall")
	endif ()
endif ()

add_executable ( example ${INCLUDE_FILES} example.cpp )
