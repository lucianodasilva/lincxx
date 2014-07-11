cmake_minimum_required (VERSION 2.6)

project (lincxx_example)

# compilation options
set ( LINCXX_UNIT FALSE CACHE BOOL "Build Unit Tests" )
set ( LINCXX_PATH ${CMAKE_CURRENT_SOURCE_DIR} )

include_directories (
	${LINCXX_PATH}/include
)

# Get all files from the include folder
file ( GLOB INCLUDE_FILES ${LINCXX_PATH}/include/*.h ${LINCXX_PATH}/include/*.hpp )

# enable c++11 on gcc and clang
if (NOT ${CMAKE_CXX_COMPILER_ID} EQUAL "MSVC")
	if (CMAKE_GENERATOR STREQUAL Xcode)
		set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
		set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
	else ()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++ -g -Wall")
	endif ()
endif ()

# if visual studio force static runtime linking
if (MSVC)

	message (STATUS "Forcing visual studio static runtime.")

    set(configuration_types
      CMAKE_C_FLAGS_DEBUG
      CMAKE_C_FLAGS_MINSIZEREL
      CMAKE_C_FLAGS_RELEASE
      CMAKE_C_FLAGS_RELWITHDEBINFO
      CMAKE_CXX_FLAGS_DEBUG
      CMAKE_CXX_FLAGS_MINSIZEREL
      CMAKE_CXX_FLAGS_RELEASE
      CMAKE_CXX_FLAGS_RELWITHDEBINFO
    )

    foreach (config ${configuration_types})
    	if(${config} MATCHES "/MD")
    		string (REGEX REPLACE "/MD" "/MT" ${config} "${${config}}")
    	endif ()
    endforeach ()

endif (MSVC)

add_executable ( example ${INCLUDE_FILES} ${LINCXX_PATH}/example/example.cpp )

# add unit tests if selected
if ( LINCXX_UNIT )
	
	set ( LINCXX_UNIT_PATH ${LINCXX_PATH}/test )
	
	add_subdirectory ( ${LINCXX_UNIT_PATH}/gtest )
	include_directories ( ${LINCXX_UNIT_PATH}/gtest/include )
	
	file ( GLOB UNIT_TESTS_SOURCE ${LINCXX_UNIT_PATH}/*.cpp )
	
	add_executable ( tests ${UNIT_TESTS_SOURCE} )
	target_link_libraries ( tests gtest gtest_main )
	
	
	set_property (GLOBAL PROPERTY USE_FOLDERS ON)
	
	set_target_properties (tests PROPERTIES FOLDER "Tests")
	set_target_properties (gtest PROPERTIES FOLDER "Tests")
	set_target_properties (gtest_main PROPERTIES FOLDER "Tests")
	
endif ()