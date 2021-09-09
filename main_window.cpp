/*
 * Copyright (c) 2021, Eberty Alves
 */

#include "main_window.h"

MainWindow::MainWindow(const QString& filename) : ui_(new Ui::MainWindowLayout) {
  ui_->setupUi(this);
  resize(600, 500);
  loadMesh(filename);

  connect(ui_->open_button_, &QPushButton::clicked, this, &MainWindow::selectFile);
}

MainWindow::~MainWindow() { delete ui_; }

void MainWindow::selectFile() {
  QString dir = QFile::exists(ui_->file_line_edit_->text()) ? ui_->file_line_edit_->text() : QString();
  QString filename = QFileDialog::getOpenFileName(this, "Load OBJ file", dir, "OBJ file (*.obj)");
  loadMesh(filename);
}

bool MainWindow::loadMesh(const QString& filename) {
  if (!filename.isEmpty() && ui_->opengl_widget_->loadMesh(filename)) {
    ui_->file_line_edit_->setText(QFileInfo(filename).canonicalFilePath());
    return true;
  }
  return false;
}
