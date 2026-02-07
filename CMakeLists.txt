cmake_minimum_required(VERSION 3.20)

project(SpicaCpp
    VERSION 1.0.0
    LANGUAGES CXX
)

# Specify C++ standard.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Collect source files.
file(GLOB_RECURSE SPICACPP_SOURCES "src/*.cpp")
file(GLOB_RECURSE SPICACPP_HEADERS "include/spicacpp/*.hpp")

# Create the library.
add_library(spicacpp ${SPICACPP_SOURCES})

# Enable exception handling for MSVC.
if(MSVC)
    target_compile_options(spicacpp PRIVATE
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
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Set include directories.
target_include_directories(spicacpp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Set any compile options
target_compile_options(spicacpp PRIVATE -Wall)
