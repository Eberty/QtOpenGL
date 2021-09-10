#-------------------------------------------------
#
# Copyright 2021 Eberty Alves
#
#-------------------------------------------------

QT += core gui widgets opengl

TARGET = qt_opengl
TEMPLATE = app

LIBS += -lGL -lassimp

SOURCES += main.cpp main_window.cpp qt_opengl.cpp
HEADERS += main_window.h qt_opengl.h
RESOURCES += resource.qrc
FORMS += main_window.ui
