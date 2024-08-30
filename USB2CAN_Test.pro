#-------------------------------------------------
#
# Project created by QtCreator 2018-05-14T16:17:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = USB2CAN_Test
TEMPLATE = app

win32{
    message($$QT_ARCH)
    contains(QT_ARCH, i386) {
        LIBS += -L$$PWD/libs/windows/x86 -lUSB2XXX
    } else {
        LIBS += -L$$PWD/libs/windows/x86_64 -lUSB2XXX
    }
}

unix:!macx{
    message($$QMAKE_HOST.arch)
    unix:contains(QMAKE_HOST.arch, x86_64){
        LIBS += -L$$PWD/libs/linux/x86_64 -lUSB2XXX
    }
    unix:contains(QMAKE_HOST.arch, x86){
        LIBS += -L$$PWD/libs/linux/x86 -lUSB2XXX
    }
    unix:contains(QMAKE_HOST.arch, aarch64){
        LIBS += -L$$PWD/libs/linux/aarch64 -lUSB2XXX
    }
    unix:contains(QMAKE_HOST.arch, armv7){
        LIBS += -L$$PWD/libs/linux/armv7 -lUSB2XXX
    }
    unix:contains(QMAKE_HOST.arch, mips64){
        LIBS += -L$$PWD/libs/linux/mips64 -lUSB2XXX
    }
}
macx{
    LIBS += -L$$PWD/libs/mac_os -lUSB2XXX
}


!win32: DEFINES += OS_UNIX


SOURCES += main.cpp\
        mainwindow.cpp \
    deviceworker.cpp \
    threadmanager.cpp

HEADERS  += \
    mainwindow.h \
    usb_device.h \
    usb2can.h \
    deviceworker.h \
    threadmanager.h

FORMS    += mainwindow.ui
