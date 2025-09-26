set(CORE_SOURCES
    manager.cpp
    fileagent.cpp
    utils.cpp
    lang.cpp
)

set(CORE_HEADERS
    manager.h
    fileagent.h
    utils.h
    lang.h
)

add_library(core_lib STATIC
    ${CORE_SOURCES}
    ${CORE_HEADERS}
)

target_include_directories(core_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(core_lib PRIVATE Qt${QT_VERSION_MAJOR}::Core)