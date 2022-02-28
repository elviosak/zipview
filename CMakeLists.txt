cmake_minimum_required(VERSION 3.5)

project(zipview VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(QuaZip-Qt5 REQUIRED)


set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        delegate.h
        delegate.cpp
        zipview.qrc
)

add_executable(zipview
        ${PROJECT_SOURCES}
)
set(ICON_FILE "resources/zipview.svg")
set(DESKTOP_FILE "resources/zipview.desktop")
set(ICON_DIR /usr/share/icons/hicolor/scalable/apps)
set(DESKTOP_DIR /usr/share/applications)

install(FILES ${ICON_FILE} DESTINATION ${ICON_DIR})
install(FILES ${DESKTOP_FILE} DESTINATION ${DESKTOP_DIR})
install(TARGETS zipview RUNTIME DESTINATION "/usr/bin")

target_link_libraries(zipview PRIVATE
    Threads::Threads
    Qt5::Widgets
    QuaZip::QuaZip
)
