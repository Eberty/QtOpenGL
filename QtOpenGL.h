/*
 * Copyright (c) 2021, Eberty Alves
 */

#ifndef QTOPENGL_H_
#define QTOPENGL_H_

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
  void timerEvent(QTimerEvent *);

 private:
  QOpenGLShaderProgram shaderProgram;
  QMatrix4x4 MVP;
  QMatrix4x4 view;
  QMatrix4x4 rot;
  QMatrix4x4 model;
  // GLuint mTexture;

  int vertexLocation;
  int matrixLocation;
  int textureLocation;
  int viewMatrixLocation;
  int modelMatrixLocation;
  int vertexUVLocation;
  int vertexNormalLocation;
  int lightLocation;

  GLuint programID;
  GLuint MatrixID;
  GLuint vertexPosition_modelspaceID;
  GLuint vertexUVID;
  GLuint TextureID;
  GLuint vertexbuffer;
  GLuint uvbuffer;
  QOpenGLTexture *mTexture;

  void loadScene(QString filename);
  unsigned int nFaces;
  unsigned int nVertFace;
  // GLfloat* vertexData;
  // GLfloat* texCoord;
  // GLfloat* normals;
  std::vector<GLfloat> vertexData;
  std::vector<GLfloat> texCoord;
  std::vector<GLfloat> normals;
};

#endif  // QTOPENGL_H_
