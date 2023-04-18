QT       += core gui widgets svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    core/about.cpp \
    core/mainwindow.cpp \
    core/options.cpp \
    core/processdialog.cpp \
    core/processscanner.cpp 

HEADERS += \
    core/about.h \
    core/mainwindow.h \
    core/options.h \
    core/parser.h \
    core/processdialog.h \
    core/processscanner.h \
    subclass/myitemdelegate.h \
    subclass/mysortfilterproxymodel.h \
    subclass/mystandarditem.h \
    subclass/mystandarditemmodel.h 

FORMS += \
    core/about.ui \
    core/mainwindow.ui \
    core/options.ui \
    core/processdialog.ui 

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Set executable name
TARGET = simple-process-tracker

RESOURCES += \
    resources.qrc

RC_ICONS = ../Assets/Icons/windows_icon/windows-app-icon.ico
