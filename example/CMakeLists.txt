cmake_minimum_required (VERSION 2.6)

project (lincxx_example)

include_directories (
	../include/
)

# Get all files from the include folder
set ( INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../include )
file ( GLOB INCLUDE_FILES ${INCLUDE_PATH}/*.h ${INCLUDE_PATH}/*.hpp )

add_executable ( example ${INCLUDE_FILES} example.cpp )
