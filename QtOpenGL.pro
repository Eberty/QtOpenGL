#-------------------------------------------------
#
# Copyright 2021 Eberty Alves
#
#-------------------------------------------------

QT += core gui widgets opengl

TARGET = QtOpenGL

LIBS += -lassimp
QMAKE_CXXFLAGS += -Wall -std=c++11

SOURCES += main.cpp main_window.cpp QtOpenGL.cpp
HEADERS += main_window.h QtOpenGL.h
FORMS += main_window.ui
RESOURCES += resource.qrc
