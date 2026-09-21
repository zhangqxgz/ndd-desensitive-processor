QT += core
QT -= gui

CONFIG += console c++17
CONFIG -= app_bundle

TEMPLATE = app
TARGET = engine_test
DESTDIR = $$PWD/out

INCLUDEPATH += $$PWD/../../src

SOURCES += \
    main.cpp \
    ../../src/desensitizer.cpp

msvc {
    QMAKE_CXXFLAGS += /utf-8
}
