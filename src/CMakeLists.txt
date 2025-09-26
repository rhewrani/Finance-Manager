cmake_minimum_required(VERSION 3.16)

project(Finance_Manager VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Finance_Manager
        MANUAL_FINALIZATION
        main.cpp
        resources/resources.qrc
    )
else()
    if(ANDROID)
        add_library(Finance_Manager SHARED
            main.cpp
            resources/resources.qrc
        )
    else()
        add_executable(Finance_Manager
            main.cpp
            resources/resources.qrc
        )
    endif()
endif()


add_subdirectory(core)
add_subdirectory(gui)
add_subdirectory(dialogs)

target_link_libraries(Finance_Manager PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
)

target_include_directories(Finance_Manager PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

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
