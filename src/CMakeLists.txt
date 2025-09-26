cmake_minimum_required(VERSION 3.16)

project(Finance_Manager VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_subdirectory(core)
add_subdirectory(gui)
add_subdirectory(dialogs)

add_executable(Finance_Manager
    main.cpp
    resources/resources.qrc
)

target_link_libraries(Finance_Manager PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    core_lib
    gui_lib
    dialogs_lib
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Finance_Manager)
endif()