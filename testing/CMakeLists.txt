cmake_minimum_required(VERSION 3.16)

# Collect all files from src and subdirectories - put into SRC_FILES
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

enable_testing()

# Use C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# MSVC: Ensure __cplusplus macro is properly set
if (MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

include(GoogleTest)



foreach(TEST_SOURCE ${TEST_SOURCES})

  get_filename_component(EXE_NAME ${TEST_SOURCE} NAME_WE)

  add_executable(${EXE_NAME} ${TEST_SOURCE})

  # Add include directory
  target_include_directories(${EXE_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

  # Link libraries
  target_link_libraries(${EXE_NAME} PRIVATE GBAQtUI GTest::gtest_main)

  gtest_discover_tests(${EXE_NAME})
endforeach(TEST_SOURCE ${TEST_SOURCES})

