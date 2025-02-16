cmake_minimum_required(VERSION 3.25)

project(mycommon LANGUAGES C CXX)
set(CMAKE_CONFIGURATION_TYPES Debug;Release)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

function(mycommon_compile_options target)
	target_compile_options(${target} PRIVATE
		$<$<CXX_COMPILER_ID:MSVC>:/MP /W4 /external:W0 /utf-8>
		$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic -Wtype-limits -Wconversion -Wsign-conversion -Wdouble-promotion -Wundef -g>

		# Enable MSVC debug symbols and edit-and-continue support
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:DEBUG>>:/ZI>
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:RELEASE>>:/Zi>

		# Colorize compiler output
		$<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color>
		$<$<CXX_COMPILER_ID:Clang,AppleClang>:-fcolor-diagnostics>)

	get_target_property(target_type ${target} TYPE)
	if(target_type STREQUAL EXECUTABLE)
		target_link_options(${target} PRIVATE
			$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:RELEASE>>:/DEBUG>)
		set_target_properties(${target} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
	endif()

	get_target_property(target_srcs ${target} SOURCES)
	source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${target_srcs})
endfunction()

add_library(mycommon OBJECT code/my_common.hpp code/my_common.cpp)
mycommon_compile_options(mycommon)
target_include_directories(mycommon PUBLIC code)

enable_testing()

add_library(catch OBJECT tests/catch_amalgamated.hpp tests/catch_amalgamated.cpp)

file(GLOB mycommon_tests_srcs CONFIGURE_DEPENDS tests/*_test.hpp tests/*_test.cpp)
add_executable(mycommon_tests ${mycommon_tests_srcs})
mycommon_compile_options(mycommon_tests)
target_link_libraries(mycommon_tests PRIVATE mycommon catch)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT mycommon_tests)
