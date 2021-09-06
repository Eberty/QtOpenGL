/*
 * Copyright (c) 2021, Eberty Alves
 */

#ifndef QTOPENGL_H_
#define QTOPENGL_H_

#include <assimp/scene.h>

#include <QKeyEvent>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLWidget>
#include <vector>

class QtOpenGL : public QOpenGLWidget, protected QOpenGLFunctions {
 public:
  explicit QtOpenGL(QWidget *parent = 0);
  ~QtOpenGL();

 protected:
  void initializeGL(void);
  void resizeGL(int w, int h);
  void paintGL();
  void keyPressEvent(QKeyEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mousePressEvent(QMouseEvent *e);
  void wheelEvent(QWheelEvent *e);
  void timerEvent(QTimerEvent *);
  void updateMouse();

 private:
  QOpenGLShaderProgram shaderProgram;
  QMatrix4x4 mRotate;
  QMatrix4x4 mModelMatrix;

  int vertexLocation;
  int vertexUVLocation;
  int vertexNormalLocation;

  int matrixLocation;
  int modelMatrixLocation;
  int normalLocation;

  QOpenGLTexture *mTexture;

  void loadScene(QString filename);
  int traverseScene(const aiScene *sc, const aiNode *nd);
  QVector3D getArcBallVector(int x, int y);
  unsigned int nFaces;

  std::vector<GLfloat> vboVertices;
  std::vector<GLfloat> vboTxCoords;
  std::vector<GLfloat> vboNormals;


  int oldX, oldY;
  int newX, newY;

  bool rotate;

  QVector3D cameraPos;
};

#endif  // QTOPENGL_H_
