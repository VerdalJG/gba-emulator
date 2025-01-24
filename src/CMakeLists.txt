add_executable(GBA-Emulator main.cpp)

target_include_directories(GBA-Emulator PUBLIC ../include)

target_compile_features(GBA-Emulator PUBLIC cxx_std_11)

target_link_libraries(GBA-Emulator PRIVATE SDL3::SDL3)

if(WIN32)
    add_custom_command(
        TARGET GBA-Emulator POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:GBA-Emulator>
        VERBATIM
    )
endif()