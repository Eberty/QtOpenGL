cmake_minimum_required(VERSION 3.1.0)
project(QtOpenGL)

add_compile_options(-std=c++11)
add_definitions(-Wall)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets OpenGL)

include_directories(include ${Qt5Widgets_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} main.cpp main_window.cpp QtOpenGL.cpp resource.qrc)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::OpenGL assimp)
