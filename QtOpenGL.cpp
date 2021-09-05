/*
 * Copyright (c) 2021, Eberty Alves
 */

#include "QtOpenGL.h"

// Include standard headers
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <QApplication>
#include <QOpenGLTexture>
#include <assimp/Importer.hpp>
#include <iostream>

static int timer_interval = 100;  // timer interval (millisec)

QtOpenGL::QtOpenGL(QWidget* parent) : QOpenGLWidget(parent) {
  startTimer(timer_interval);
  loadScene("./suzanne.obj");
}

QtOpenGL::~QtOpenGL() {
  makeCurrent();

  delete mTexture;
}

void QtOpenGL::loadScene(QString filename) {
  if (!vertexData.empty()) {
    vertexData.clear();
  }

  if (!texCoord.empty()) {
    texCoord.clear();
  }

  if (!normals.empty()) normals.clear();

  Assimp::Importer importer;
  const aiScene* scene =
      importer.ReadFile(filename.toStdString(), aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                                                    aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

  if (!scene) {
    std::cout << "Scene import failed." << std::endl;
    return;
  }

  aiMesh* myMesh = scene->mMeshes[0];
  nFaces = myMesh->mNumFaces;
  nVertFace = myMesh->mFaces[0].mNumIndices;

  for (int i = 0; i < nFaces; i++) {
    aiFace currFace = myMesh->mFaces[i];
    for (int j = 0; j < nVertFace; j++) {
      int n = currFace.mIndices[j];
      vertexData.push_back(myMesh->mVertices[n].x);
      vertexData.push_back(myMesh->mVertices[n].y);
      vertexData.push_back(myMesh->mVertices[n].z);

      normals.push_back(myMesh->mNormals[n].x);
      normals.push_back(myMesh->mNormals[n].y);
      normals.push_back(myMesh->mNormals[n].z);

      texCoord.push_back(myMesh->mTextureCoords[0][n].x);
      texCoord.push_back(myMesh->mTextureCoords[0][n].y);
    }
  }
}

void QtOpenGL::initializeGL() {
  initializeOpenGLFunctions();
  glClearColor(0.0f, 0.0f, 0.3f, 0.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);

  bool success;
  success = shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/phong.vert");
  success = shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/phong.frag");

  shaderProgram.link();

  vertexLocation = shaderProgram.attributeLocation("vertexPosition_modelspace");
  matrixLocation = shaderProgram.uniformLocation("MVP");
  modelMatrixLocation = shaderProgram.uniformLocation("M");
  vertexUVLocation = shaderProgram.attributeLocation("vertexUV");
  vertexNormalLocation = shaderProgram.attributeLocation("vertexNormal_modelspace");
  textureLocation = shaderProgram.uniformLocation("myTextureSampler");
  lightLocation = shaderProgram.uniformLocation("LightPosition_worldspace");

  QImage image = QImage(":/uvmap.DDS");
  mTexture = new QOpenGLTexture(image.mirrored());
  mTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
  mTexture->setMagnificationFilter(QOpenGLTexture::Linear);

  glEnable(GL_TEXTURE_2D);
  mTexture->bind();
}

void QtOpenGL::paintGL(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (vertexData.empty() || texCoord.empty() || normals.empty()) return;

  QMatrix4x4 projection;
  projection.perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);

  QVector3D cameraPos(0, 3, 5);
  QMatrix4x4 view;
  view.lookAt(cameraPos, QVector3D(0, 0, 0), QVector3D(0, 1, 0));

  QMatrix4x4 pmvMatrix = projection * view * rot * model;

  shaderProgram.bind();

  glBindTexture(GL_TEXTURE_2D, 1);

  QVector3D lightPos(4, 4, 4);

  shaderProgram.setUniformValue(matrixLocation, pmvMatrix);
  shaderProgram.setUniformValue("M", model);
  shaderProgram.setUniformValue("V", view);
  shaderProgram.setUniformValue(lightLocation, lightPos);

  shaderProgram.enableAttributeArray(vertexLocation);
  shaderProgram.enableAttributeArray(vertexUVLocation);
  shaderProgram.enableAttributeArray(vertexNormalLocation);

  shaderProgram.setAttributeArray(vertexLocation, &vertexData[0], 3);
  shaderProgram.setAttributeArray(vertexUVLocation, &texCoord[0], 2);
  shaderProgram.setAttributeArray(vertexNormalLocation, &normals[0], 3);

  glDrawArrays(GL_TRIANGLES, 0, nFaces * 3);

  shaderProgram.disableAttributeArray(vertexLocation);
  shaderProgram.disableAttributeArray(vertexUVLocation);
  shaderProgram.disableAttributeArray(vertexNormalLocation);
  shaderProgram.release();
}

void QtOpenGL::timerEvent(QTimerEvent*) {
  static float rotval = 4.0;
  rot.rotate(rotval, 1.0, 0.0, 0.0);
  update();
}

void QtOpenGL::resizeGL(int w, int h) { glViewport(0, 0, (GLsizei)w, (GLsizei)h); }

void QtOpenGL::keyPressEvent(QKeyEvent* e) {
  switch (e->key()) {
    case Qt::Key_Escape:
      exit(0);
      break;
    default:
      break;
  }
  e->accept();
}
