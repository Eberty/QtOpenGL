#-------------------------------------------------
#
# Copyright 2021 Eberty Alves
#
#-------------------------------------------------

QT += core gui widgets opengl

TARGET = QtOpenGL
TEMPLATE = app

LIBS += -lGL -lassimp
QMAKE_CXXFLAGS += -Wall -std=c++11

SOURCES += main.cpp main_window.cpp QtOpenGL.cpp
HEADERS += main_window.h QtOpenGL.h
RESOURCES += resource.qrc
FORMS += main_window.ui
