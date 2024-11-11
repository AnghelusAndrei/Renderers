# Renderers
This repository serves as a collection of my most memorable renderers troughout the years

# VoxelRenderer3.0

This renderer is my current project as of November 2024 and my best one so far in terms of techniques, structure and optimization
For more information you can access its repository [here](https://github.com/AnghelusAndrei/VoxelEngine3.0)
The project is made using OpenGL and GLFW and highly optimised for the GPU.
Succesfully implements:
```
-Sparse Voxel Octrees
-A 4 pass raytracing rendering system:

  -RayPass (fragment):
    -finds the intersection with the closest voxel (DDA on octrees optimised with bitwise operations)
    -calculates the incoming light contribution (custom raytracing)
    -repeats for every light bounce and sample
    -stores a color image and a voxel ID image

  -AccumPass (compute):
    -Accumulates color for pixels sharing the same voxel ID in a spatial and temporal buffer using complex Atomic operations 
    -stores the amount of pixels which hit the same voxel in the buffer for every pixel proccesed currently
    -updates previous data from older frames

  -AvgPass (compute):
    -Averages color for pixels sharing the same voxel ID (using Atomic Operations)

  -RenderPass (vertex + fragment):
    -renders the final image to the scene

-per voxel normals
-simple material system
```

### Raytracing
![image](https://github.com/user-attachments/assets/0b2da2ea-1eb6-4dfd-adfb-e517fd563a5d)

# VulkanVoxelRenderer


