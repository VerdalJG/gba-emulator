cmake_minimum_required(VERSION 3.16)
project(GBA-Emulator VERSION 0.1.0 LANGUAGES C CXX)

# Use C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MSVC: Ensure __cplusplus macro is properly set
if (MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

#Find SDL3 package
find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)

# Set the CMake Path for Qt
set(CMAKE_PREFIX_PATH "D:/WorkUni/Qt6/6.9.1/msvc2022_64")
set(CMAKE_VERBOSE_MAKEFILE ON)

#Find Qt package
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)


# Add the subdirectories for source, include, and lib files
add_subdirectory(qt_ui)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(lib)


# CPack testing
include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)