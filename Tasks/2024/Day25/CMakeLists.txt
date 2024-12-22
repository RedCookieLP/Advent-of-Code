# Use at minimum 3.16 for PCHs and Ubuntu 20.04
cmake_minimum_required(VERSION 3.16..3.30)

# Add the new target for this day
add_executable(Day25)
target_sources(Day25 PRIVATE main.cpp)

# Link the puzzle-base interface
target_link_libraries(Day25 PRIVATE puzzleBase)
