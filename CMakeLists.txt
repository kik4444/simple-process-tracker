cmake_minimum_required(VERSION 3.16)

project(simple-process-tracker VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS Core REQUIRED)

set(source_files

    mainwindow.ui
    mainwindow.h
    mainwindow.cpp
    main.cpp
    resources.qrc

    custom/platform_specifics.h

    ui/trackentry/trackentry.h
    ui/trackentry/trackentry.cpp
    ui/trackentry/trackentry.ui

    ui/processDialog/processdialog.h
    ui/processDialog/processdialog.cpp
    ui/processDialog/processdialog.ui

    ui/options/options.h
    ui/options/options.cpp
    ui/options/options.ui

    ui/about/about.h
    ui/about/about.cpp
    ui/about/about.ui
)

set(additional_arguments "")

# Set Windows icon
if (WIN32)
    set(additional_arguments WIN32 "${CMAKE_CURRENT_SOURCE_DIR}/Assets/Icons/windows_icon/windows-app-icon.rc")
endif()

add_executable(${PROJECT_NAME} ${additional_arguments} ${source_files})

target_link_libraries(simple-process-tracker PRIVATE
    Qt6::Widgets
    Qt6::Core
)
