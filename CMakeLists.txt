cmake_minimum_required(VERSION 3.20)

project(SpicaCpp
    VERSION 1.0.0
    LANGUAGES CXX
)

# Specify C++ standard.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Create compile_commands.json for IDEs and tools.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Function to define compile options for the targets based on the compiler.
function(apply_spicacpp_compile_options target)
  if(MSVC)
    target_compile_options(${target} PRIVATE
            /EHsc
            /W3
            /wd4668  # Disable undefined macro warnings.
            /wd4865  # Disable the enum underlying type warning.
            /wd5039  # potentially throwing function in extern "C".
            /wd5045  # Disable Spectre mitigation warnings.
        )
    # Suppress Microsoft's warnings about standard C functions (fopen, strcpy, etc.).
    # TODO: Consider replacing C-isms with C++ alternatives during future refactoring.
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endfunction()

# SpicaCpp Library Configuration
################################

# Collect source files.
file(GLOB_RECURSE SPICACPP_SOURCES "src/*.cpp")
file(GLOB_RECURSE SPICACPP_HEADERS "include/spicacpp/*.hpp")

# Create the library.
add_library(spicacpp ${SPICACPP_SOURCES})

# Set include directories.
target_include_directories(spicacpp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Set any compile options
apply_spicacpp_compile_options(spicacpp)

# SpicaCpp Test Program Configuration
#####################################

# Collect test source files.
file(GLOB_RECURSE SPICACPP_TEST_SOURCES "tests/*.cpp")

# Create the test executable.
add_executable(spicacpp_test ${SPICACPP_TEST_SOURCES})
target_link_libraries(spicacpp_test PRIVATE spicacpp)

# Set include directories for the test program.
target_include_directories(spicacpp_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/tests
)

# Set any compile options for the test
apply_spicacpp_compile_options(spicacpp_test)

# Create a custom target to copy compile_commands.json to the source directory.
# This simplifies access for IDEs and tools (such as language servers).
add_custom_target(copy-compile-commands ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
)
