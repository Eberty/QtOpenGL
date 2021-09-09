/*
 * Copyright (c) 2021, Eberty Alves
 */

#ifndef QTOPENGL_H_
#define QTOPENGL_H_

#include <assimp/scene.h>

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QtWidgets>
#include <vector>

/**
 * TODO.
 */
class QtOpenGL : public QOpenGLWidget, protected QOpenGLFunctions {
 public:
  /**
   * Class constructor.
   *
   * @param parent: defines a parent widget for this class.
   */
  explicit QtOpenGL(QWidget *parent = 0);

  /**
   * Destructor of the class.
   */
  ~QtOpenGL();

  /**
   * TODO.
   *
   * @param color: TODO.
   */
  void setClearColor(const QColor &color);

  /**
   * TODO.
   *
   * @param use_material: TODO.
   */
  void setUseMaterial(const bool use_material);

  /**
   * TODO.
   *
   * @param filename: TODO.
   *
   * @return TODO.
   */
  bool loadMesh(const QString &filename);

  /**
   * TODO.
   *
   * @param filename: TODO.
   *
   * @return TODO.
   */
  bool loadTexture(const QString &filename);

 protected:
  /**
   * Overload method to render the OpenGL scene whenever the scene is updated.
   */
  void paintGL() override;

  /**
   * Overload method to setup the OpenGL viewport, projection, etc.
   *
   * @param width: width of the widget.
   * @param height: height of the widget.
   */
  void resizeGL(int width, int height) override;

  /**
   * Overload method to setup OpenGL resources and state.
   */
  void initializeGL() override;

  /*
   * When an event occurs is called events function. Theses virtual functions will be responsible for reacting
   * appropriately.
   */
  void keyPressEvent(QKeyEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;
  bool event(QEvent *event) override;

 private:
  /**
   * TODO.
   */
  void createCustomContextMenu();

  /**
   * TODO.
   *
   * @return TODO.
   */
  bool initShaders();

  /**
   * TODO.
   */
  void enableGlCapabilities();

  /**
   * TODO.
   */
  void getAttributeLocations();

  /**
   * TODO.
   */
  void resetView();

  /**
   * TODO.
   */
  void clearAllVBOs();

  /**
   * TODO.
   *
   * @param sc: TODO.
   * @param nd: TODO.
   *
   * @return TODO.
   */
  int traverseScene(const aiScene *sc, const aiNode *nd);

  /**
   * TODO.
   *
   * @param vertex: TODO.
   */
  void updateSceneBoundingBox(const aiVector3D &vertex);

  /**
   * TODO.
   *
   * @param material: TODO.
   * @param mesh_index: TODO.
   */
  void updateMaterial(const aiMaterial *const material, const int mesh_index);

  /**
   * TODO.
   *
   * @return TODO.
   */
  bool isValidTexture();

  /**
   * TODO.
   *
   * @param MVP: TODO.
   */
  void setUniformValues(const QMatrix4x4 &MVP);

  /**
   * TODO.
   */
  void drawMesh();

  /**
   * TODO.
   *
   * @param x: TODO.
   * @param y: TODO.
   *
   * @return TODO.
   */
  QVector3D getArcBallVector(int x, int y);

  QColor clear_color_ = Qt::white; /**< TODO */

  QOpenGLShaderProgram shader_program_; /**< TODO */
  QOpenGLTexture *texture_ = NULL;      /**< TODO */

  QString texture_filename_; /**< TODO */

  bool is_texture_loaded_ = false; /**< TODO */
  bool use_material_ = true;       /**< TODO */

  float camera_pos_z_mult_ = 1.0; /**< TODO */

  int vertex_location_;           /**< TODO */
  int vertex_color_location_;     /**< TODO */
  int vertex_normal_location_;    /**< TODO */
  int vertex_uv_coords_location_; /**< TODO */

  std::vector<float> vbo_vertices_;       /**< TODO */
  std::vector<float> vbo_normals_;        /**< TODO */
  std::vector<float> vbo_colors_;         /**< TODO */
  std::vector<float> vbo_texture_coords_; /**< TODO */

  QPoint last_pos_; /**< TODO */

  QMatrix4x4 rotation_matrix_; /**< TODO */

  QVector3D light_pos_;  /**< TODO */
  QVector3D camera_pos_; /**< TODO */

  QVector3D scene_min_;    /**< TODO */
  QVector3D scene_max_;    /**< TODO */
  QVector3D scene_center_; /**< TODO */

  QVector4D ambient_material_ = QVector4D(0.6, 0.6, 0.6, 1.0);  /**< TODO */
  QVector4D diffuse_material_ = QVector4D(0.5, 0.0, 0.0, 1.0);  /**< TODO */
  QVector4D specular_material_ = QVector4D(1.0, 1.0, 1.0, 1.0); /**< TODO */
};

#endif  // QTOPENGL_H_
