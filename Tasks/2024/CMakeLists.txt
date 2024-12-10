# Use at minimum 3.16 for PCHs and Ubuntu 20.04
cmake_minimum_required(VERSION 3.16..3.30)

# Define a set of configuration-types
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

project(
	AoC-2024
	VERSION 0.1
	DESCRIPTION "RedCookieLP's solutions to the Advent-of-Code (AoC) 2024 puzzles"
	LANGUAGES C CXX
)

# Define a common interface library
add_library(puzzleBase INTERFACE)
target_compile_features(puzzleBase INTERFACE cxx_std_17)
target_include_directories(puzzleBase INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(
	puzzleBase
	INTERFACE
		entry-point.cpp
		AoC-Module.h
)
target_compile_definitions(
	puzzleBase
	INTERFACE
		$<$<BOOL:${PART_1}>:PART_1>
		$<$<CONFIG:Debug>:DEBUG>
		$<$<CONFIG:Release>:NDEBUG>
)
target_compile_options(puzzleBase INTERFACE -Wall -Wextra)

# Add all the subdirectories as targets
list(APPEND DAYS
	Day1  Day2  Day3  Day4  Day5  Day6
	Day7  Day8  Day9  Day10 
)
foreach(day IN LISTS DAYS)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${day})
endforeach()
