# QtOpenGL

## **Overview**

Minimalist project using CMake, Qt and OpenGL to load and present a 3D object.

**Author:** Eberty Alves da Silva, <eberty.silva@hotmail.com>

**Keywords:** CMake, Qt and OpenGL.

&nbsp;

## **Install dependencies**

```sh
sudo apt install libgl1-mesa-glx libassimp-dev qtbase5-dev qt5-default
```

## **Building with CMake**

```sh
git clone https://github.com/Eberty/QtOpenGL.git
cd QtOpenGL/
mkdir build && cd build
cmake ..
make -j$(nproc)
cd ..
```

## **Building with Qt (.pro file)**

```sh
git clone https://github.com/Eberty/QtOpenGL.git
cd QtOpenGL/
qmake qt_opengl.pro
make -j$(nproc)
```

## **Run C++ program**

```sh
./qt_opengl
```
