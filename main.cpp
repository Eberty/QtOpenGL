/*
 * Copyright (c) 2021, Eberty Alves
 */

#include <QApplication>

#include "main_window.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  app.setWindowIcon(QIcon(":qt_opengl.png"));

  QString filename = (argc > 1) ? argv[1] : "bunny.obj";
  MainWindow viewer(filename);
  viewer.show();

  return app.exec();
}
