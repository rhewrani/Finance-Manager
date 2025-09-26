cmake_minimum_required(VERSION 3.16)

project(Finance_Manager VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp mainwindow.h mainwindow.ui
        manager.h manager.cpp
        fileagent.h fileagent.cpp
        configwindow.h configwindow.cpp configwindow.ui
        entrywizard.h entrywizard.cpp entrywizard.ui
        utils.h utils.cpp
        info.h info.cpp info.ui
        settingswindow.h settingswindow.cpp settingswindow.ui
        log.txt
        lang.h lang.cpp
        resources.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS LinguistTools)

    qt_add_executable(Finance_Manager
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(Finance_Manager SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(Finance_Manager
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Finance_Manager PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(Finance_Manager PRIVATE Qt6::Core)
target_link_libraries(Finance_Manager PRIVATE Qt6::Core)
target_link_libraries(Finance_Manager PRIVATE Qt6::Core)
target_link_libraries(Finance_Manager PRIVATE Qt6::Core)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Finance_Manager)
endif()
set_target_properties(Finance_Manager PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Finance_Manager
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Finance_Manager)
endif()
