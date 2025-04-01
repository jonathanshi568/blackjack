# Set the minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(BlackjackEngine VERSION 1.0 LANGUAGES CXX)

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Define the source files
set(SRCS
    main.cpp
    BlackjackEngine.cpp
    Shuffler.cpp
    Player.cpp
    types.cpp
    util.cpp
)

# Define header files
set(HEADERS
    types.hpp
    BlackjackEngine.hpp
    Shuffler.hpp
    Player.hpp
    types.hpp
    util.hpp
)

# Add the executable
add_executable(blackjack_engine ${SRCS} ${HEADERS})

# Set additional compilation flags
target_compile_options(blackjack_engine PRIVATE -Wall)

# Optionally, you can set debug flags for the debug build
# If you want a specific debug target
set(CMAKE_BUILD_TYPE Debug)

# Enable debugging symbols if in debug mode
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(blackjack_engine PRIVATE -g)
endif()

# Optional: You can define the output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include directories for header files (if necessary)
# target_include_directories(blackjack_engine PRIVATE ${CMAKE_SOURCE_DIR})

# Clean up the build (CMake handles cleaning automatically)
# But if you want a custom command, you can define the following:
# add_custom_target(clean
#   COMMAND rm -rf ${CMAKE_BINARY_DIR}/*)