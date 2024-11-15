#pragma once

#include <GLFW/glfw3.h>
#include "config.hpp"
#include <stdlib.h>

#include "vulkaninstance.hpp"

class Octree;
class OctreeModel;

class VulkanInstance;
class VoxelEngine
{
public:
    VoxelEngine(Config config);


    void run();
    
    ~VoxelEngine();
public:

    GLFWwindow *window;
    Octree *octree;
    bool running_;
    
private:
    VulkanInstance *instance_;
    int maxThreads_p;
    Config config_;

    static void framebuffer_resized(GLFWwindow *window_, int width, int height);
    static void window_maximized(GLFWwindow *window_, int maximized);
    
    friend class VulkanInstance;
};