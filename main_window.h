/*
 * Copyright (c) 2021, Eberty Alves
 */

#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

#include <QtWidgets>

#include "ui_main_window.h"

/**
 * TODO.
 */
class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  /**
   * Class constructor.
   *
   * @param filename: TODO.
   */
  explicit MainWindow(const QString& filename = "");

  /**
   * Destructor of the class.
   */
  ~MainWindow();

 private:
  /**
   * TODO.
   */
  void selectFile();

  /**
   * TODO.
   *
   * @param filename: TODO.
   *
   * @return TODO.
   */
  bool loadObjFile(const QString &filename);

  Ui::MainWindowLayout *ui_; /**< User interface layout */
};

#endif  // MAIN_WINDOW_H_
