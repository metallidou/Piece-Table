cmake_minimum_required(VERSION 3.10)
project(unit_test)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Compiler options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3 -g3")

# Add linker options
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lm")

# Find Catch2
find_package(Catch2 REQUIRED)

# List all source files
set(SOURCES
        PieceTable.cpp
        tests/test.cpp
)

# Add the executable for the tests
add_executable(unit_test ${SOURCES})

# Link Catch2 to your test executable
target_link_libraries(unit_test PRIVATE Catch2::Catch2WithMain)

# Include Catch2's CMake configuration
include(Catch)
catch_discover_tests(unit_test)

# Add a custom clean target (optional)
add_custom_target(my_clean
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/unit_test
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/CMakeFiles
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/cmake_install.cmake
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/CMakeCache.txt
)
