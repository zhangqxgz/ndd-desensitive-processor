QT += core gui widgets
CONFIG += console c++17
CONFIG -= app_bundle
TEMPLATE = app
TARGET = scintilla_test
DESTDIR = $$PWD/out

INCLUDEPATH += $$PWD/../../sdk/qscint/src
INCLUDEPATH += $$PWD/../../sdk/qscint/src/Qsci

SOURCES += main.cpp

msvc {
    QMAKE_CXXFLAGS += /utf-8
    LIBS += -L$$PWD/../../sdk/lib -lqmyedit_qt5
}
