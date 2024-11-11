# SVO Renderer
## 3D voxel raytracing engine on the GPU using OpenCL for parallel pixel computation (raycasting on an SVO)

### TO do:
  - Loading from files
  - Fixing bugs
  - Delta frames
  - SVO compression

### requires:
* Linux
* SDL2
* OpenCL
* SDL_ttf

### Building
```
git clone --recursive https://github.com/MegaSomething/Voxel_Renderer.git
cd Voxel_Renderer
git submodule update --init --recursive

mkdir build
cd build

cmake ..
cmake --build .
```
