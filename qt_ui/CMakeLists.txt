# Find all hpp cpp and ui files
file(GLOB UI_SOURCES *.cpp)
file(GLOB UI_HEADERS *.hpp)
file(GLOB UI_FORMS  *.ui)

# For Qt auto tools - moc and uic 
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# Create the library with name GBAQtUI
add_library(GBAQtUI STATIC
    ${UI_SOURCES} 
    ${UI_HEADERS} 
    ${UI_FORMS}
)

target_include_directories(GBAQtUI PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link to Qt
target_link_libraries(GBAQtUI PUBLIC 
Qt6::Widgets
Qt6::Gui
Qt6::Core
)
