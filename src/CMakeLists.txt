# Collect all files from src and subdirectories - put into SRC_FILES
file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${PROJECT_SOURCE_DIR}/include/*.tpp"    
)

# Create the library with name EmulatorCore
add_library(EmulatorCore STATIC ${CORE_SOURCES})

# Access to the include directory
target_include_directories(EmulatorCore PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Use C++20 standard
target_compile_features(EmulatorCore PUBLIC cxx_std_20)

