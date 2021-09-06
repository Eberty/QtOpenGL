/*
 * Copyright (c) 2021, Eberty Alves
 */

#include "QtOpenGL.h"

#include <assimp/postprocess.h>

#include <QApplication>
#include <QOpenGLTexture>
#include <QtMath>
#include <assimp/Importer.hpp>
#include <iostream>

QtOpenGL::QtOpenGL(QWidget* parent) : QOpenGLWidget(parent) {
  loadScene("./bunny.obj");
  cameraPos = QVector3D(0, 0, 500);
}

QtOpenGL::~QtOpenGL() {
  makeCurrent();
  delete mTexture;
}

void QtOpenGL::loadScene(QString filename) {
  if (!vboVertices.empty()) {
    vboVertices.clear();
  }

  if (!vboTxCoords.empty()) {
    vboTxCoords.clear();
  }

  if (!vboNormals.empty()) {
    vboNormals.clear();
  }

  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(filename.toStdString(), aiProcessPreset_TargetRealtime_MaxQuality);

  if (!scene) {
    std::cout << "Scene import failed." << std::endl;
    return;
  }

  nFaces = 0;
  traverseScene(scene, scene->mRootNode);
}

void QtOpenGL::initializeGL() {
  initializeOpenGLFunctions();
  glClearColor(0.0, 0.0, 0.0, 0.0);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);
  glEnable(GL_CULL_FACE);

  bool success = true;
  success &= shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/phong.vert");
  success &= shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/phong.frag");

  if (!success) {
    std::cout << "Shader import failed." << std::endl;
  }

  mModelMatrix.setToIdentity();
  mRotate.setToIdentity();

  shaderProgram.link();

  vertexLocation = shaderProgram.attributeLocation("aPosition");
  vertexUVLocation = shaderProgram.attributeLocation("aCoords");
  vertexNormalLocation = shaderProgram.attributeLocation("aNormal");

  matrixLocation = shaderProgram.uniformLocation("uMVP");
  modelMatrixLocation = shaderProgram.uniformLocation("uM");
  normalLocation = shaderProgram.uniformLocation("uN");

  QImage image = QImage("./bunny.jpg");
  mTexture = new QOpenGLTexture(image.mirrored());
  mTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
  mTexture->setMagnificationFilter(QOpenGLTexture::Linear);
  mTexture->setWrapMode(QOpenGLTexture::ClampToEdge);

  glEnable(GL_TEXTURE_2D);
  mTexture->bind();
}

int QtOpenGL::traverseScene(const aiScene* sc, const aiNode* nd) {
  int totVertices = 0;
  for (unsigned int n = 0; n < nd->mNumMeshes; ++n) {
    const aiMesh* mesh = sc->mMeshes[nd->mMeshes[n]];
    // const aiMaterial* const mat = sc->mMaterials[mesh->mMaterialIndex];
    nFaces += mesh->mNumFaces;
    for (unsigned int t = 0; t < mesh->mNumFaces; ++t) {
      const aiFace* face = &mesh->mFaces[t];
      for (unsigned int i = 0; i < face->mNumIndices; i++) {
        int index = face->mIndices[i];
        if (mesh->HasTextureCoords(0)) {
          vboTxCoords.push_back(mesh->mTextureCoords[0][index][0]);
          vboTxCoords.push_back(mesh->mTextureCoords[0][index][1]);
        }
        if (mesh->mNormals != NULL) {
          vboNormals.push_back(mesh->mNormals[index].x);
          vboNormals.push_back(mesh->mNormals[index].y);
          vboNormals.push_back(mesh->mNormals[index].z);
        }
        vboVertices.push_back(mesh->mVertices[index].x);
        vboVertices.push_back(mesh->mVertices[index].y);
        vboVertices.push_back(mesh->mVertices[index].z);
        totVertices++;
      }
    }
  }

  for (unsigned int n = 0; n < nd->mNumChildren; ++n) {
    totVertices += traverseScene(sc, nd->mChildren[n]);
  }

  return totVertices;
}

void QtOpenGL::paintGL(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (vboVertices.empty() || vboTxCoords.empty() || vboNormals.empty()) {
    return;
  }

  QMatrix4x4 projection;
  projection.perspective(45.0f, 4.0f / 3.0f, 0.01, 10000.0);

  QMatrix4x4 view;
  view.lookAt(cameraPos, QVector3D(0, 0, 0), QVector3D(0, 1, 0));

  QMatrix4x4 MVP = projection * view * mModelMatrix * mRotate;
  QMatrix4x4 normalMat = QMatrix4x4(mModelMatrix * mRotate).inverted().transposed();

  shaderProgram.bind();

  glBindTexture(GL_TEXTURE_2D, 1);

  QVector3D lightPos(1000, 1000, 1000);

  shaderProgram.setUniformValue(matrixLocation, MVP);
  shaderProgram.setUniformValue(normalLocation, normalMat);
  shaderProgram.setUniformValue(modelMatrixLocation, QMatrix4x4(mModelMatrix * mRotate));

  shaderProgram.setUniformValue("uTexLoad", 1);
  shaderProgram.setUniformValue("uCamPos", cameraPos);
  shaderProgram.setUniformValue("uLPos", lightPos);

  QVector4D matAmb(0.117647, 0.117647, 0.117647, 1.0);
  QVector4D matDif(0.752941, 0.752941, 0.752941, 1.0);
  QVector4D matSpec(0.752941, 0.752941, 0.752941, 1.0);
  shaderProgram.setUniformValue("matAmb", matAmb);
  shaderProgram.setUniformValue("matDif", matDif);
  shaderProgram.setUniformValue("matSpec", matSpec);

  shaderProgram.enableAttributeArray(vertexLocation);
  shaderProgram.enableAttributeArray(vertexUVLocation);
  shaderProgram.enableAttributeArray(vertexNormalLocation);

  shaderProgram.setAttributeArray(vertexLocation, &vboVertices[0], 3);
  shaderProgram.setAttributeArray(vertexUVLocation, &vboTxCoords[0], 2);
  shaderProgram.setAttributeArray(vertexNormalLocation, &vboNormals[0], 3);

  glDrawArrays(GL_TRIANGLES, 0, nFaces * 3);

  shaderProgram.disableAttributeArray(vertexLocation);
  shaderProgram.disableAttributeArray(vertexUVLocation);
  shaderProgram.disableAttributeArray(vertexNormalLocation);
  shaderProgram.release();
}

void QtOpenGL::resizeGL(int w, int h) { glViewport(0, 0, w, h); }

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

void QtOpenGL::timerEvent(QTimerEvent*) {}

void QtOpenGL::wheelEvent(QWheelEvent* event) {}

void QtOpenGL::mousePressEvent(QMouseEvent* e) {
  rotate = false;
  if (e->button() == Qt::LeftButton) {
    oldX = newX = e->x();
    oldY = newY = e->y();

    rotate = true;
  }
}

void QtOpenGL::mouseMoveEvent(QMouseEvent* e) {
  if (e->buttons() & Qt::LeftButton) {
    if (rotate) {
      newX = e->x();
      newY = e->y();
      updateMouse();
    }
    oldX = e->x();
    oldY = e->y();
  }
}

void QtOpenGL::updateMouse() {
  QVector3D v = getArcBallVector(oldX, oldY);
  QVector3D u = getArcBallVector(newX, newY);

  float angle = std::acos(std::min(1.0f, QVector3D::dotProduct(u, v)));

  QVector3D rotAxis = QVector3D::crossProduct(v, u);
  QMatrix4x4 eye2ObjSpaceMat = mRotate.inverted();
  QVector3D objSpaceRotAxis = eye2ObjSpaceMat * rotAxis;

  oldX = newX;
  oldY = newY;

  mRotate.rotate(qRadiansToDegrees(angle), objSpaceRotAxis);
  update();
}

QVector3D QtOpenGL::getArcBallVector(int x, int y) {
  QVector3D pt = QVector3D(2.0 * x / this->width() - 1.0, 2.0 * y / this->height() - 1.0, 0);
  pt.setY(pt.y() * -1);

  float xySquared = pt.x() * pt.x() + pt.y() * pt.y();

  if (xySquared <= 1.0) {
    pt.setZ(std::sqrt(1.0 - xySquared));
  } else {
    pt.normalize();
  }

  return pt;
}
