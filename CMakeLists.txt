cmake_minimum_required(VERSION 3.22)
project(your-mod-id)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QPM generated files
include(${CMAKE_CURRENT_LIST_DIR}/extern/includes/qpm_defines.cmake OPTIONAL)
include(${CMAKE_CURRENT_LIST_DIR}/extern/includes/extern.cmake OPTIONAL)

# Collect source files
file(GLOB_RECURSE SOURCE_FILES
    src/*.cpp
    src/*.c
)

# Build the mod shared library
add_library(your-mod-id SHARED ${SOURCE_FILES})

target_include_directories(your-mod-id PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/shared
)

# Link QPM dependencies
if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/extern/includes/extern.cmake)
    target_link_libraries(your-mod-id PRIVATE extern)
endif()

# Android specific settings
target_compile_options(your-mod-id PRIVATE
    -fvisibility=hidden
    -fvisibility-inlines-hidden
)
