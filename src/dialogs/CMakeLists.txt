# src/dialogs/CMakeLists.txt
set(DIALOGS_SOURCES
    entrywizard.cpp
    info.cpp
)

set(DIALOGS_HEADERS
    entrywizard.h
    info.h
)

set(DIALOGS_FORMS
    entrywizard.ui
    info.ui
)

add_library(dialogs_lib STATIC
    ${DIALOGS_SOURCES}
    ${DIALOGS_HEADERS}
    ${DIALOGS_FORMS}
)

target_include_directories(dialogs_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(dialogs_lib PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    core_lib
    gui_lib
)