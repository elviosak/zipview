cmake_minimum_required(VERSION 3.5)

project(zipview VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(QuaZip-Qt5 REQUIRED)


set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
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
    Qt5::Widgets
    QuaZip::QuaZip
)

#set_target_properties(zipview PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
#    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
#    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
#)

#if(QT_VERSION_MAJOR EQUAL 6)
#    qt_finalize_executable(zipview)
#endif()
