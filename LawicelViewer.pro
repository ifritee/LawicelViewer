QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/common/MessageT.cpp \
    src/gui/DSettings.cpp \
    src/gui/WReceive.cpp \
    src/gui/WTransmit.cpp \
    src/interaction/URS232Abonent.cpp \
    src/interaction/URS232Controller.cpp \
    src/main.cpp \
    src/gui/MainWindow.cpp \
    src/system/CLogRegistration.cpp

HEADERS += \
    src/common/MessageT.h \
    src/gui/DSettings.h \
    src/gui/MainWindow.h \
    src/gui/WReceive.h \
    src/gui/WTransmit.h \
    src/interaction/URS232Abonent.h \
    src/common/CConstants.h \
    src/interaction/URS232Controller.h \
    src/system/CLogRegistration.h

FORMS += \
    src/gui/DSettings.ui \
    src/gui/MainWindow.ui \
    src/gui/WReceive.ui \
    src/gui/WTransmit.ui

INCLUDEPATH += \
    src \
    src/gui \
    src/interaction \
    src/common \
    src/system

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res/LawicelViewer.qrc
