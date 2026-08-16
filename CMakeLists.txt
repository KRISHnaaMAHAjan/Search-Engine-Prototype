cmake_minimum_required(VERSION 3.20)
project(SearchEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optimization flags
if(MSVC)
    add_compile_options(/W4 /WX /Ot /Oi /Gy /Zi)
else()
    add_compile_options(-Wall -Wextra -O3 -march=native -pthread)
endif()

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
)

add_executable(ags ${SOURCES})

# Target-specific properties
set_target_properties(ags PROPERTIES
    OUTPUT_NAME "searchtool"
)

# Installation (optional, but good practice)
install(TARGETS ags DESTINATION bin)
