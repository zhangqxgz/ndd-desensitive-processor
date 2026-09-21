QT += core gui widgets
CONFIG += console c++17
CONFIG -= app_bundle
TEMPLATE = app
TARGET = load_test
DESTDIR = $$PWD/out

INCLUDEPATH += $$PWD/../../sdk/include

SOURCES += main.cpp

msvc {
    QMAKE_CXXFLAGS += /utf-8
}
