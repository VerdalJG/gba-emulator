# Find all hpp cpp and ui files
file(GLOB UI_HEADERS *.hpp)
file(GLOB UI_HEADERS *.cpp)
file(GLOB UI_HEADERS *.ui)


# For Qt auto tools - moc and uic 
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# Creat the library with name GBAQtUI
add_library(GBAQtUI ${UI_SOURCES} ${UI_HEADERS} ${UI_FORMS})

# Link to Qt
target_link_libraries(GBAQtUI PRIVATE Qt6::Widgets)
target_include_directories(GBAQtUI PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})