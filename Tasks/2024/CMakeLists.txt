# Use at minimum 3.16 for PCHs and Ubuntu 20.04
cmake_minimum_required(VERSION 3.16..3.30)

project(
	AoC-2024
	VERSION 0.1
	DESCRIPTION "RedCookieLP's solutions to the Advent-of-Code (AoC) 2024 puzzles"
	LANGUAGES C CXX
)

# Define a set of configuration-types
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

# Add the common library
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Lib)

# Add all the subdirectories as targets
list(APPEND DAYS
	Day1  Day2  Day3  Day4  Day5  Day6
	Day7  Day8  Day9  Day10 Day11 Day12
	Day13 Day14 Day15 Day16 Day17 Day18
	Day19 Day20 Day21 Day22 Day23 Day24
)
foreach(day IN LISTS DAYS)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${day})
endforeach()
