/*
 * Copyright (c) 2021, Eberty Alves
 */

#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

#include <QtWidgets>

#include "ui_main_window.h"

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

 private:
  Ui::MainWindowLayout *ui_;
};

#endif  // MAIN_WINDOW_H_
