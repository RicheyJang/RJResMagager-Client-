QT       += core gui network
QT += sql
QT += xlsx
QT += printsupport

RC_ICONS = logo.ico

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    MainFunc.cpp \
    aboutme.cpp \
    errorwidget.cpp \
    hintcombobox.cpp \
    itemsmanager.cpp \
    login.cpp \
    main.cpp \
    mainwindow.cpp \
    messenger.cpp \
    newOrder.cpp \
    newitemorder.cpp \
    papertable.cpp \
    userdialog.cpp \
    usermanager.cpp

HEADERS += \
    QDBC.h \
    aboutme.h \
    errorwidget.h \
    hintcombobox.h \
    itemsmanager.h \
    login.h \
    maininclude.h \
    mainwindow.h \
    messenger.h \
    newitemorder.h \
    neworder.h \
    papertable.h \
    userdialog.h \
    usermanager.h

FORMS += \
    aboutme.ui \
    itemsmanager.ui \
    login.ui \
    mainwindow.ui \
    newitemorder.ui \
    neworder.ui \
    papertable.ui \
    userdialog.ui \
    usermanager.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    src.qrc

DISTFILES += \
    README.md
