cmake_minimum_required(VERSION 3.10)
project(GBA-Emulator VERSION 0.1.0 LANGUAGES C CXX)

# Use C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Find SDL3 package
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)

# Add the subdirectories for source, include, and lib files
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(lib)

# CPack testing
include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)