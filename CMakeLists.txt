cmake_minimum_required(VERSION 3.1.0)
project(qt_opengl)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Widgets OpenGL)

include_directories(include ${OPENGL_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} main.cpp main_window.cpp qt_opengl.cpp resource.qrc)
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${ASSIMP_LIBRARIES} Qt5::Widgets Qt5::OpenGL)
