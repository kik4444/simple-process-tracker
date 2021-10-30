cmake_minimum_required(VERSION 3.16)

project(simple-process-tracker VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS Core REQUIRED)

add_executable(simple-process-tracker
    mainwindow.ui
    mainwindow.h
    mainwindow.cpp
    main.cpp
    resources.qrc

    ui/trackentry/trackentry.h
    ui/trackentry/trackentry.cpp
    ui/trackentry/trackentry.ui

    ui/processDialog/processdialog.h
    ui/processDialog/processdialog.cpp
    ui/processDialog/processdialog.ui

    custom/platform_specifics.h
)

target_link_libraries(simple-process-tracker PRIVATE
    Qt6::Widgets
    Qt6::Core
)
