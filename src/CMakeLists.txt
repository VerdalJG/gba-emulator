add_executable(GBA-Emulator main.cpp)

# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

target_include_directories(GBA-Emulator PUBLIC ../include)

target_compile_features(GBA-Emulator PUBLIC cxx_std_11)

target_link_libraries(GBA-Emulator ${SDL3_LIBRARIES})