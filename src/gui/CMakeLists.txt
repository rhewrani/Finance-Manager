# src/gui/CMakeLists.txt
set(GUI_SOURCES
    mainwindow.cpp
    configwindow.cpp
    settingswindow.cpp
)

set(GUI_HEADERS
    mainwindow.h
    configwindow.h
    settingswindow.h
)

set(GUI_FORMS
    mainwindow.ui
    configwindow.ui
    settingswindow.ui
)

add_library(gui_lib STATIC
    ${GUI_SOURCES}
    ${GUI_HEADERS}
    ${GUI_FORMS}
)

target_include_directories(gui_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(gui_lib PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    core_lib
)