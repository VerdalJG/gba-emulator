# Find all hpp cpp and ui files
file(GLOB_RECURSE UI_SOURCES *.cpp)
file(GLOB_RECURSE UI_HEADERS *.hpp)
file(GLOB_RECURSE UI_FORMS  *.ui)

# For Qt auto tools - moc and uic 
# set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTOUIC ON)

qt_standard_project_setup()

# Create the library with name GBAQtUI
add_library(GBAQtUI STATIC
    ${UI_SOURCES} 
    ${UI_HEADERS} 
    ${UI_FORMS}
)

# Set Qt features for this specific target
target_sources(GBAQtUI PRIVATE ${UI_FORMS})
set_target_properties(GBAQtUI PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    #AUTORCC ON
)

set(CMAKE_AUTOMOC_VERBOSE OFF)
set(CMAKE_AUTOUIC_VERBOSE OFF)

# Link to Qt and SDL3
target_link_libraries(GBAQtUI PUBLIC 
    EmulatorCore
    SDL3::SDL3
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
)

# Add this directory to includes
target_include_directories(GBAQtUI PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
