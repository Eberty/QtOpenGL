/*
 * Copyright (c) 2021, Eberty Alves
 */

#include "main_window.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui_(new Ui::MainWindowLayout) {
  ui_->setupUi(this);
}

MainWindow::~MainWindow() {
  delete ui_;
}
