QT       += core gui
QT += network
LIBS += -lShell32

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    appglobals.cpp \
    downloader.cpp \
    downloadwindow.cpp \
    finishwindow.cpp \
    main.cpp \
    qdman.cpp \
    urldialog.cpp

HEADERS += \
    appglobals.h \
    downloader.h \
    downloadwindow.h \
    finishwindow.h \
    qdman.h \
    urldialog.h

FORMS += \
    downloadwindow.ui \
    finishwindow.ui \
    qdman.ui \
    urldialog.ui

TRANSLATIONS += \
    Quantum_en_US.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
