# Collect all files from src and subdirectories - put into SRC_FILES
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# Collect all header files and subdirectories - put into HEADER_FILES
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

# Create main executable
add_executable(GBA-Emulator ${SRC_FILES} ${HEADER_FILES})

# Add include directory
target_include_directories(GBA-Emulator PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Use C++20 standard
target_compile_features(GBA-Emulator PUBLIC cxx_std_20)

# Link SDL3 library
target_link_libraries(GBA-Emulator PRIVATE SDL3::SDL3)

if(WIN32)
    add_custom_command(
        TARGET GBA-Emulator POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:GBA-Emulator>
        VERBATIM
    )
endif()