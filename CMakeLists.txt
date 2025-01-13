cmake_minimum_required(VERSION 3.10)

project(zipview VERSION 0.2 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(Qt6 COMPONENTS Widgets REQUIRED)
# find_package(QuaZip-Qt6 REQUIRED)
find_package(KF6Archive)

set(PROJECT_SOURCES
        main.cpp
#        mainwindow.cpp mainwindow.h
        delegate.h
        delegate.cpp
        view.h view.cpp
        loader.h loader.cpp
        window.h window.cpp
        zipview.qrc
)
# if(6 GREATER_EQUAL 6)
    qt_add_executable(zipview
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# else()
#     add_executable(zipview
#             ${PROJECT_SOURCES}
#     )
# endif()

set(ICON_FILE "resources/zipview.svg")
set(DESKTOP_FILE "resources/zipview.desktop")
set(ICON_DIR ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/scalable/apps)
set(DESKTOP_DIR ${CMAKE_INSTALL_PREFIX}/share/applications)

install(FILES ${ICON_FILE} DESTINATION ${ICON_DIR})
install(FILES ${DESKTOP_FILE} DESTINATION ${DESKTOP_DIR})
install(TARGETS zipview RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

target_link_libraries(zipview PRIVATE
    Threads::Threads
    Qt6::Widgets
    KF6::Archive
    # QuaZip::QuaZip
)

# if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(zipview)
# endif()
