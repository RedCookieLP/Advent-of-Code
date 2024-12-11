# Use at minimum 3.16 for PCHs and Ubuntu 20.04
cmake_minimum_required(VERSION 3.16..3.30)

# Define a common interface library
add_library(puzzleBase STATIC)
target_compile_features(puzzleBase PUBLIC cxx_std_17)
target_include_directories(puzzleBase PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(
	puzzleBase
	PRIVATE
		main.cpp
	PUBLIC
		AoC-Module.h
		HashHelper.hpp
		Vector2.hpp
		Vector3.hpp
)
target_compile_definitions(
	puzzleBase
	PUBLIC
		$<$<BOOL:${PART_1}>:PART_1>
		$<$<CONFIG:Debug>:DEBUG>
		$<$<CONFIG:Release>:NDEBUG>
)
#target_compile_options(puzzleBase PUBLIC -Wall -Wextra)
