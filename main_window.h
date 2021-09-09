/*
 * Copyright (c) 2021, Eberty Alves
 */

#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

#include <QtWidgets>

#include "ui_main_window.h"

/**
 * @brief A main window provides a framework for building an application's user interface. Qt has QMainWindow and its
 * related classes for main window management.
 */
class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  /**
   * Class constructor.
   *
   * @param filename: initial mesh file to be loaded by the qt opengl widget.
   */
  explicit MainWindow(const QString& filename = "");

  /**
   * Destructor of the class.
   */
  ~MainWindow();

 private:
  /**
   * Slot called by button click. It will open a QFileDialog allowing to select a file path and call the method to load
   * the chosen mesh file.
   */
  void selectFile();

  /**
   * Loads a mesh file (.obj) and displays it on the screen. Calls QtOpenGL::loadMesh.
   *
   * @param filename: mesh file to be loaded by the qt opengl widget.
   *
   * @return True if the mesh was loaded successfully.
   */
  bool loadMesh(const QString &filename);

  Ui::MainWindowLayout *ui_; /**< User interface layout */
};

#endif  // MAIN_WINDOW_H_
