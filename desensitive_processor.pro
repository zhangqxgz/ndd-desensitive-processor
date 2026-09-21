TEMPLATE = lib
LANGUAGE = C++
CONFIG += qt warn_on release c++17
QT += core gui widgets

TARGET = desensitive_processor
DESTDIR = $$PWD/build/out

HEADERS += \
    src/desensitizer.h \
    src/mappingstore.h \
    src/mainwindow.h \
    src/instanceobj.h

SOURCES += \
    src/desensitizer.cpp \
    src/mappingstore.cpp \
    src/mainwindow.cpp \
    src/instanceobj.cpp \
    src/plugin_export.cpp

INCLUDEPATH += $$PWD/sdk/include
INCLUDEPATH += $$PWD/sdk/qscint/src
INCLUDEPATH += $$PWD/sdk/qscint/src/Qsci

msvc {
    QMAKE_CXXFLAGS += /utf-8
    LIBS += -L$$PWD/sdk/lib -lqmyedit_qt5
}

unix {
    QMAKE_CXXFLAGS += -fPIC
}
