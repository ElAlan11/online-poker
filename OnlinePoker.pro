QT       += core gui
QT += network
CONFIG += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    lobby.cpp \
    main.cpp \
    mainserverapp.cpp \
    mainwindow.cpp \
    player.cpp \
    playermin.cpp \
    pokergame.cpp

HEADERS += \
    lobby.h \
    mainserverapp.h \
    mainwindow.h \
    player.h \
    playermin.h \
    pokergame.h

FORMS += \
    lobby.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
