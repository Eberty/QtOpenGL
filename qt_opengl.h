/*
 * Copyright (c) 2021, Eberty Alves
 */

#ifndef QT_OPENGL_H_
#define QT_OPENGL_H_

#include <assimp/scene.h>

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QtWidgets>
#include <vector>

/**
 * @brief A QOpenGLWidget based class that allows loading and displaying OpenGL scenes in qt applications. For this
 * widget, we use the Phong's realistic rendering technique
 */
class QtOpenGL : public QOpenGLWidget, protected QOpenGLFunctions {
  Q_OBJECT

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
   * Updates the background color of the viewer.
   *
   * @param color: new background color of the viewer.
   */
  void setClearColor(const QColor &color);

  /**
   * Enable shading method (Phong).
   *
   * @param use_material: True to enable shading.
   */
  void setUseMaterial(const bool use_material);

  /**
   * Loads the mesh and updating the viewer.
   *
   * @param filename: path to the mesh file to be loaded.
   *
   * @return True if the mesh was loaded successfully.
   */
  bool loadMesh(const QString &filename);

  /**
   * Create a new QOpenGLTexture from an image.
   *
   * @param filename: path to the texture file to be loaded.
   *
   * @return True if the texture was loaded successfully.
   */
  bool loadTexture(const QString &filename);

 Q_SIGNALS:  // NOLINT
  /**
   * To allow texture loading outside the main thread, this signal is emitted, so texture loading can be done at runtime
   * even if the context is not the current.
   */
  void loadTextureSignal();

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
   * Create custom context menus to update viewer properties.
   */
  void createCustomContextMenu();

  /**
   * Add veetex and fragment shaders to the shader_program_ using source files.
   *
   * @return True if shaders were loaded successfully.
   */
  bool initShaders();

  /**
   * Enable GL_DEPTH_TEST, GL_NORMALIZE and GL_TEXTURE_2D. Also set glDepthFunc to GL_LESS.
   */
  void enableGlCapabilities();

  /**
   * Get location for vertex positions, colors, normals and UV coordinates from the shader program.
   */
  void getAttributeLocations();

  /**
   * Resets all viewer properties.
   */
  void resetView();

  /**
   * Clear all VBOs vectors.
   */
  void clearAllVBOs();

  /**
   * Recursive function to traverse the entire scene and update the VBOs (vertex buffer object).
   *
   * @param sc: assimp scene to be traversed.
   *
   * @return Number of traversed vertices.
   */
  int traverseScene(const aiScene *sc, const aiNode *nd);

  /**
   * Given a new vertex, updates the scene bounding box (scene min and max).
   *
   * @param vertex: vertex to check if it is a delimiter of the scene.
   */
  void updateSceneBoundingBox(const aiVector3D &vertex);

  /**
   * Given a aiMaterial from a mesh gets its diffuse, specular, ambient materials. Get also the texture if exists.
   *
   * @param material: assimp loaded material.
   * @param mesh_index: mesh index of a scene.
   */
  void updateMaterial(const aiMaterial *const material, const int mesh_index);

  /**
   * Validates the texture: texture file must be valid (and correctly loaded) and object must contain texture
   * coordinates..
   *
   * @return True if the loaded mesh has a is valid texture.
   */
  bool isValidTexture();

  /**
   * Sets the uniform variables in the shader program..
   *
   * @param MVP: model/view/projection matrix.
   */
  void setUniformValues(const QMatrix4x4 &MVP);

  /**
   * Set attribute arrays and call glDrawArrays GL_TRIANGLES..
   */
  void drawMesh();

  /**
   * A method to helps manipulating and rotating a scene with the mouse.
   *
   * @param x: x-axis coordinate.
   * @param y: y-axis coordinate.
   *
   * @see http://courses.cms.caltech.edu/cs171/assignments/hw3/hw3-notes/notes-hw3.html
   *
   * @return The point at the surface of the ball (scene) that matches the mouse click.
   */
  QVector3D getArcBallVector(int x, int y);

  QColor clear_color_ = Qt::white; /**< Background color of the viewer */

  QOpenGLShaderProgram shader_program_; /**< Allows OpenGL shader programs to be linked and used */
  QOpenGLTexture *texture_ = NULL;      /**< Encapsulates an OpenGL texture object */

  QString mesh_filename_;    /**< Path to the loaded mesh */
  QString texture_filename_; /**< Path to the loaded texture */

  bool is_texture_loaded_ = false; /**< True if the texture was loaded successfully */
  bool use_material_ = true;       /**< Enable shading method (Phong) */

  float camera_pos_z_mult_ = 1.0; /**< Responsible for zoom in and zoom out */

  int vertex_location_;           /**< Location of aPosition attribute in shader */
  int vertex_color_location_;     /**< Location of aColor attribute in shader */
  int vertex_normal_location_;    /**< Location of aNormal attribute in shader */
  int vertex_uv_coords_location_; /**< Location of aCoords attribute in shader */

  std::vector<float> vbo_vertices_;       /**< VBO: vertexcies */
  std::vector<float> vbo_normals_;        /**< VBO: normals */
  std::vector<float> vbo_colors_;         /**< VBO: colors */
  std::vector<float> vbo_texture_coords_; /**< VBO: texture coordinates*/

  QPoint last_pos_; /**< Last known mouse position during its manipulation */

  QMatrix4x4 rotation_matrix_; /**< Rotation matrix for the shading technique and visualization */

  QVector3D light_pos_;  /**< Light position */
  QVector3D camera_pos_; /**< Camera position */

  QVector3D scene_min_;    /**< Minimum point of the bound box of the scene */
  QVector3D scene_max_;    /**< Maximum point of the bound box of the scene */
  QVector3D scene_center_; /**< Point indicating the scene center */

  QVector4D ambient_material_ = QVector4D(0.6, 0.6, 0.6, 1.0);  /**< Ambient material for shading */
  QVector4D diffuse_material_ = QVector4D(0.5, 0.0, 0.0, 1.0);  /**< Diffuse material for shading */
  QVector4D specular_material_ = QVector4D(1.0, 1.0, 1.0, 1.0); /**< Specular material for shading */
};

#endif  // QT_OPENGL_H_
