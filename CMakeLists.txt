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

# Source files
set(SOURCES
    src/main.cpp
)

add_executable(searchtool ${SOURCES})

# Target-specific properties and include directories
target_include_directories(searchtool PRIVATE include)

# Installation (optional, but good practice)
install(TARGETS searchtool DESTINATION bin)
