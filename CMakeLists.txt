cmake_minimum_required(VERSION 3.10)
project(GBA-Emulator VERSION 0.1.0 LANGUAGES C CXX)

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)

# Add the subdirectories for source, include, and lib files
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(lib)
add_subdirectory(include)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)