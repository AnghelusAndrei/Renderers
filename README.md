# Renderers
This repository serves as a collection of my most memorable renderers troughout the years

# [VoxelRenderer3.0](https://github.com/AnghelusAndrei/VoxelEngine3.0)

This renderer is my current project as of November 2024 and my best one so far in terms of techniques, structure and optimization
For more information or build details you can access its repository.
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

# [VulkanVoxelRenderer](https://github.com/AnghelusAndrei/VulkanVoxelRenderer)

This renderer is my second big renderer I made in collaboration with Albert Maftei, student at ETH Zürich, and altough it doesn't excel in neither optimization nor features,
it is still very impressive for me as it was my first first time working with Vulkan and attempting a multiple pass pipeline.
This project could be considered a direct predecessor to VoxelRenderer3.0

# [Voxel_Renderer](https://github.com/AnghelusAndrei/Voxel_Renderer)

Voxel_Renderer is my first big renderer from the voxel raytracing series, I was in 8th grade when I began working on it, which 
is very clear in the messy code structure. Altough it is not nearly as refined and optimized as my last project, I still consider
this project a succes as I managed to implement the highest amount of features out of all the renderers, no matter how many bugs they have.
It is here where I first created my own 3D DDA algorithm, developed my first GPU SVO structure and used hardware acceleration 
to march the rays trough the octree. Successfully implements:
```
-Sparse Voxel Octrees
-per voxel normals
-simple material system
-simple object system(moving and rotating)
-capability to import obj files
-blinn-phong lighting mode, very simple reflections and simple hard shadows
```
