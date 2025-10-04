cmake_minimum_required(VERSION 3.10)
project(unit_test)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Compiler options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3 -g3")

# Linker options
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lm")

# Source files
set(SOURCES
        PieceTable.cpp
        tests/test.cpp
)

# Fetch Catch2 automatically
include(FetchContent)
FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.11.0
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(catch2)

# Build test executable
add_executable(unit_test ${SOURCES})
target_link_libraries(unit_test PRIVATE Catch2::Catch2WithMain)

include(Catch)
catch_discover_tests(unit_test)

# Clean target
add_custom_target(my_clean
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/unit_test
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/CMakeFiles
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/cmake_install.cmake
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/CMakeCache.txt
)
