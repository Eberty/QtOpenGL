/*
 * Copyright (c) 2021, Eberty Alves
 */

#include "qt_opengl.h"

#include <assimp/postprocess.h>

#include <assimp/Importer.hpp>
#include <limits>

QtOpenGL::QtOpenGL(QWidget* parent) : QOpenGLWidget(parent) {
  setFocusPolicy(Qt::WheelFocus);
  createCustomContextMenu();

  connect(
      this, &QtOpenGL::loadTextureSignal, this,
      [this]() {
        is_texture_loaded_ = loadTexture(texture_filename_);
      },
      Qt::QueuedConnection);
}

QtOpenGL::~QtOpenGL() {
  makeCurrent();
  if (texture_) {
    delete texture_;
  }
  doneCurrent();
}

void QtOpenGL::setClearColor(const QColor& color) {
  if (color.isValid()) {
    clear_color_ = color;
    update();
  }
}

void QtOpenGL::setUseMaterial(const bool use_material) { use_material_ = use_material; }

bool QtOpenGL::loadMesh(const QString& filename) {
  QFileInfo file(filename);
  if (!file.exists(filename) || !file.completeSuffix().endsWith("obj")) {
    qWarning() << filename << "is not a valid mesh file.";
    return false;
  }

  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(filename.toStdString(), aiProcessPreset_TargetRealtime_MaxQuality);

  if (!scene) {
    qWarning() << "Scene import failed.";
    return false;
  }

  resetView();
  clearAllVBOs();

  float max = std::numeric_limits<float>::max();
  float min = std::numeric_limits<float>::min();

  scene_min_ = QVector3D(max, max, max);
  scene_max_ = QVector3D(min, min, min);

  traverseScene(scene, scene->mRootNode);
  moveObjectToOrigin();

  max = qMax(scene_max_.x(), qMax(scene_max_.y(), scene_max_.z()));
  light_pos_ = QVector3D(max, max, max) * 3.0;

  mesh_filename_ = filename;
  emit loadTextureSignal();

  return true;
}

bool QtOpenGL::loadTexture(const QString& filename) {
  if (filename.isEmpty()) {
    texture_ = NULL;
    return false;
  }

  makeCurrent();
  QString texture_filepath = QFileInfo(mesh_filename_).absolutePath() + QString(QDir::separator()) + filename;

  if (texture_) {
    delete texture_;
  }

  QImage image = QImage(texture_filepath);
  texture_ = new QOpenGLTexture(image.mirrored());
  texture_->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
  texture_->setMagnificationFilter(QOpenGLTexture::Linear);
  texture_->setWrapMode(QOpenGLTexture::ClampToEdge);

  if (texture_->textureId() == 0) {
    texture_ = NULL;
    qWarning() << "Texture import failed.";
    return false;
  }

  texture_->bind();
  return true;
}

void QtOpenGL::paintGL(void) {
  glClearColor(clear_color_.redF(), clear_color_.greenF(), clear_color_.blueF(), clear_color_.alphaF());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  QMatrix4x4 projection;
  float near = scene_max_.distanceToPoint(scene_min_) / 500.0;
  projection.perspective(45.0f, (width() / static_cast<float>(height() ? height() : 1)), near, 100000.0);
  camera_pos_ = QVector3D(0, 0, (scene_max_.z() * 5.0 * camera_pos_z_mult_));

  QMatrix4x4 view;
  view.lookAt(camera_pos_, QVector3D(0, 0, 0), QVector3D(0, 1, 0));

  shader_program_.bind();

  QMatrix4x4 MVP = projection * view * rotation_matrix_;
  setUniformValues(MVP);

  if (texture_) {
    glBindTexture(GL_TEXTURE_2D, texture_->textureId());
  }

  drawMesh();

  shader_program_.release();
}

void QtOpenGL::resizeGL(int width, int height) { glViewport(0, 0, width, height); }

void QtOpenGL::initializeGL() {
  initializeOpenGLFunctions();

  initShaders();
  enableGlCapabilities();

  shader_program_.link();

  getAttributeLocations();
}

void QtOpenGL::keyPressEvent(QKeyEvent* event) {
  switch (event->key()) {
    case Qt::Key_Escape:
      QApplication::quit();
      break;
    case Qt::Key_Space:
      resetView();
      break;
    case Qt::Key_Period:
      makeCurrent();
      glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
      break;
    case Qt::Key_Minus:
      makeCurrent();
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      break;
    case Qt::Key_F:
      makeCurrent();
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      break;
    default:
      break;
  }
}

void QtOpenGL::mouseMoveEvent(QMouseEvent* event) {
  if (event->buttons() & Qt::LeftButton) {
    QVector3D v = getArcBallVector(last_pos_.x(), last_pos_.y());
    QVector3D u = getArcBallVector(event->x(), event->y());

    float angle = qAcos(qMin(1.0f, QVector3D::dotProduct(u, v)));

    QVector3D rot_axis = QVector3D::crossProduct(v, u);
    QVector3D rot_vector = rotation_matrix_.inverted() * rot_axis;

    last_pos_ = event->pos();

    rotation_matrix_.rotate(qRadiansToDegrees(angle), rot_vector);
  }
}

void QtOpenGL::mousePressEvent(QMouseEvent* event) { last_pos_ = event->pos(); }

void QtOpenGL::wheelEvent(QWheelEvent* event) {
  camera_pos_z_mult_ *=
      (event->delta() > 0) ? (camera_pos_z_mult_ < 10 ? 1.25 : 1.0) : (camera_pos_z_mult_ > 0.1 ? 0.8 : 1.0);
}

bool QtOpenGL::event(QEvent* event) {
  bool handled = QOpenGLWidget::event(event);
  switch (event->type()) {
    case QEvent::Show:
    case QEvent::KeyPress:
    case QEvent::MouseButtonPress:
    case QEvent::MouseMove:
    case QEvent::Wheel:
      this->update();
      break;
    default:
      break;
  }
  return handled;
}

void QtOpenGL::createCustomContextMenu() {
  setContextMenuPolicy(Qt::CustomContextMenu);

  QMenu* menu = new QMenu(this);

  QAction* shading_action = new QAction("Enable shading", this);
  shading_action->setCheckable(true);
  shading_action->setChecked(use_material_);
  connect(shading_action, &QAction::toggled, this, &QtOpenGL::setUseMaterial);
  menu->addAction(shading_action);

  menu->addSeparator();

  QAction* color_action = new QAction("Change background color", this);
  connect(color_action, &QAction::triggered, this, [this]() {
    QColor color = QColorDialog::getColor(clear_color_, this);
    setClearColor(color);
  });
  menu->addAction(color_action);

  connect(this, &QLabel::customContextMenuRequested, [this, menu](QPoint pos) { menu->popup(this->mapToGlobal(pos)); });
}

bool QtOpenGL::initShaders() {
  bool success = true;
  success &= shader_program_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":phong.vert");
  success &= shader_program_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":phong.frag");

  if (!success) {
    qWarning() << "Shader import failed.";
  }

  return success;
}

void QtOpenGL::enableGlCapabilities() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);
  glEnable(GL_TEXTURE_2D);

  glDepthFunc(GL_LESS);
}

void QtOpenGL::getAttributeLocations() {
  vertex_location_ = shader_program_.attributeLocation("aPosition");
  vertex_color_location_ = shader_program_.attributeLocation("aColor");
  vertex_normal_location_ = shader_program_.attributeLocation("aNormal");
  vertex_uv_coords_location_ = shader_program_.attributeLocation("aCoords");
}

void QtOpenGL::resetView() {
  makeCurrent();

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  use_material_ = true;
  camera_pos_z_mult_ = 1.0;

  rotation_matrix_.setToIdentity();
}

void QtOpenGL::clearAllVBOs() {
  vbo_vertices_.clear();
  vbo_colors_.clear();
  vbo_normals_.clear();
  vbo_texture_coords_.clear();
}

int QtOpenGL::traverseScene(const aiScene* sc, const aiNode* nd) {
  int tot_vertices = 0;
  for (unsigned int n = 0; n < nd->mNumMeshes; n++) {
    const aiMesh* mesh = sc->mMeshes[nd->mMeshes[n]];
    updateMaterial(sc->mMaterials[mesh->mMaterialIndex], n);

    for (unsigned int t = 0; t < mesh->mNumFaces; t++) {
      const aiFace* face = &mesh->mFaces[t];
      for (unsigned int i = 0; i < face->mNumIndices; i++) {
        int index = face->mIndices[i];

        vbo_colors_.push_back(ambient_material_.x());
        vbo_colors_.push_back(ambient_material_.y());
        vbo_colors_.push_back(ambient_material_.z());
        vbo_colors_.push_back(ambient_material_.w());

        if (mesh->HasTextureCoords(0)) {
          vbo_texture_coords_.push_back(mesh->mTextureCoords[0][index].x);
          vbo_texture_coords_.push_back(mesh->mTextureCoords[0][index].y);
        }

        if (mesh->mNormals != NULL) {
          vbo_normals_.push_back(mesh->mNormals[index].x);
          vbo_normals_.push_back(mesh->mNormals[index].y);
          vbo_normals_.push_back(mesh->mNormals[index].z);
        }

        aiVector3D vertex = mesh->mVertices[index];
        vbo_vertices_.push_back(vertex.x);
        vbo_vertices_.push_back(vertex.y);
        vbo_vertices_.push_back(vertex.z);
        updateSceneBoundingBox(vertex);

        tot_vertices++;
      }
    }
  }

  for (unsigned int n = 0; n < nd->mNumChildren; n++) {
    tot_vertices += traverseScene(sc, nd->mChildren[n]);
  }

  return tot_vertices;
}

void QtOpenGL::updateSceneBoundingBox(const aiVector3D& vertex) {
  scene_min_.setX(qMin(scene_min_.x(), vertex.x));
  scene_min_.setY(qMin(scene_min_.y(), vertex.y));
  scene_min_.setZ(qMin(scene_min_.z(), vertex.z));

  scene_max_.setX(qMax(scene_max_.x(), vertex.x));
  scene_max_.setY(qMax(scene_max_.y(), vertex.y));
  scene_max_.setZ(qMax(scene_max_.z(), vertex.z));
}

void QtOpenGL::moveObjectToOrigin() {
  QVector3D scene_center = QVector3D(scene_min_ + scene_max_) / 2.0;
  for (size_t i = 0; i < vbo_vertices_.size(); i += 3) {
    vbo_vertices_[i] -= scene_center.x();
    vbo_vertices_[i + 1] -= scene_center.y();
    vbo_vertices_[i + 2] -= scene_center.z();
  }

  scene_min_ -= scene_center;
  scene_max_ -= scene_center;
}

void QtOpenGL::updateMaterial(const aiMaterial* const material, const int mesh_index) {
  aiColor4D c;
  aiString s;

  if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, c)) {
    ambient_material_ = QVector4D(c.r, c.g, c.b, c.a);
  }
  if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, c)) {
    diffuse_material_ = QVector4D(c.r, c.g, c.b, c.a);
  }
  if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, c)) {
    specular_material_ = QVector4D(c.r, c.g, c.b, c.a);
  }

  if (AI_SUCCESS == material->Get(AI_MATKEY_TEXTURE_DIFFUSE(mesh_index), s)) {
    texture_filename_ = s.C_Str();
  } else {
    texture_filename_.clear();
  }
}

bool QtOpenGL::isValidTexture() {
  return (is_texture_loaded_ && !vbo_texture_coords_.empty() && !texture_filename_.isEmpty());
}

void QtOpenGL::setUniformValues(const QMatrix4x4& MVP) {
  QMatrix4x4 normal_matrix = rotation_matrix_.inverted().transposed();

  shader_program_.setUniformValue("uLPos", light_pos_);
  shader_program_.setUniformValue("uCamPos", camera_pos_);

  shader_program_.setUniformValue("matAmb", ambient_material_);
  shader_program_.setUniformValue("matDif", diffuse_material_);
  shader_program_.setUniformValue("matSpec", specular_material_);

  shader_program_.setUniformValue("uMVP", MVP);
  shader_program_.setUniformValue("uN", normal_matrix);
  shader_program_.setUniformValue("uM", rotation_matrix_);

  shader_program_.setUniformValue("uTexLoad", isValidTexture());
  shader_program_.setUniformValue("uMaterial", use_material_);
}

void QtOpenGL::drawMesh() {
  if (vbo_vertices_.empty() || vbo_colors_.empty() || vbo_normals_.empty()) {
    return;
  }

  shader_program_.setAttributeArray(vertex_location_, &vbo_vertices_[0], 3);
  shader_program_.enableAttributeArray(vertex_location_);

  shader_program_.setAttributeArray(vertex_color_location_, &vbo_colors_[0], 4);
  shader_program_.enableAttributeArray(vertex_color_location_);

  shader_program_.setAttributeArray(vertex_normal_location_, &vbo_normals_[0], 3);
  shader_program_.enableAttributeArray(vertex_normal_location_);

  if (isValidTexture()) {
    shader_program_.setAttributeArray(vertex_uv_coords_location_, &vbo_texture_coords_[0], 2);
    shader_program_.enableAttributeArray(vertex_uv_coords_location_);
  }

  glDrawArrays(GL_TRIANGLES, 0, (vbo_vertices_.size() / 3));

  shader_program_.disableAttributeArray(vertex_location_);
  shader_program_.disableAttributeArray(vertex_color_location_);
  shader_program_.disableAttributeArray(vertex_normal_location_);
  if (isValidTexture()) {
    shader_program_.disableAttributeArray(vertex_uv_coords_location_);
  }
}

QVector3D QtOpenGL::getArcBallVector(int x, int y) {
  float w = width() ? width() : 1.0;
  float h = height() ? height() : 1.0;

  QVector3D pt = QVector3D((2.0 * x / w - 1.0), (2.0 * y / h - 1.0), 0.0);
  pt.setY(pt.y() * -1);

  float xy_squared = (pt.x() * pt.x()) + (pt.y() * pt.y());

  if (xy_squared <= 1.0) {
    pt.setZ(qSqrt(1.0 - xy_squared));
  } else {
    pt.normalize();
  }

  return pt;
}
