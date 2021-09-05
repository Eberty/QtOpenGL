/*
 * Copyright (c) 2021, Eberty Alves
 */

#include <QApplication>

#include "main_window.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  app.setWindowIcon(QIcon(":/QtOpenGL.png"));

  MainWindow viewer;
  viewer.show();

  return app.exec();
}
