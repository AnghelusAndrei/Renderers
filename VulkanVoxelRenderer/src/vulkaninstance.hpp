#pragma once

#include <atomic>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <optional>
#include <set>
#include <array>
#include <map>
#include <stack>
#include <thread>
#include <mutex>

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <GLFW/glfw3.h>

#include "voxelengine.hpp"

#define MULTITHREADED

/**
 * @brief Represents all rendering infrastructure
 *
 */
struct CameraUBO
{
    glm::vec4 position;
    glm::vec4 direction;
    glm::vec4 cameraPlanVector;
    glm::vec4 cameraPlanSurfaceRightVector;
    glm::vec4 cameraPlanSurfaceUpVector;
};
struct UBO
{
    CameraUBO camera;
};
class VoxelEngine;
class Octree;
class Camera;
class VulkanInstance
{
public:
    VulkanInstance(VoxelEngine *engine);
    void run();
    void render();
    void setCamera(Camera *camera) { camera_ = camera; }
    ~VulkanInstance();
    void update();

    struct VmaBuffer
    {
        vk::Buffer buffer;
        VmaAllocation allocation;
        VmaAllocationInfo allocationInfo;
    };
    VmaBuffer stagingBuffer_;

private:
    Camera *camera_;
    struct QueueSupportDetails
    {
        std::optional<uint32_t> computeFamily;
        std::optional<uint32_t> presentFamily;
        bool hasValues()
        {
            return computeFamily.has_value() && presentFamily.has_value();
        }
    };
    struct SwapChainSupportDetails
    {
        vk::SurfaceCapabilitiesKHR capabilities;
        vk::SurfaceFormatKHR format = vk::Format::eUndefined;
        vk::PresentModeKHR presentMode = vk::PresentModeKHR::eFifo;
        vk::Extent2D extent;
    };

    struct RaycastSpecialization
    {
        uint32_t window_width, window_height, octreeDepth;
    };

    struct LightingSpecialization
    {
    };

    struct RenderSpecialization
    {
    };
    struct SpecializationConstants
    {
        RaycastSpecialization raycast;
        LightingSpecialization lighting;
        RenderSpecialization render;
    };

    VoxelEngine *engine_;
    vk::Instance instance_;
    vk::DispatchLoaderDynamic dispatch_;
    vk::DebugUtilsMessengerEXT debugMessenger_;
    vk::PhysicalDevice physicalDevice_;
    vk::SurfaceKHR surface_;
    vk::Device device_;
    vk::Queue presentQueue_, computeQueue_;
    VmaAllocator allocator_;
    std::mutex uploadMutex_;
    bool upload_;
    VmaBuffer octreeBuffer_, lightingBuffer_;
    std::vector<VmaBuffer> uniformBuffers_;
    size_t currentFrame_ = 0;
    vk::CommandPool commandPool_;
    vk::DescriptorPool computeDescriptorSetPool, graphicsDescriptorSetPool;
    vk::SwapchainKHR swapChain_;
    std::vector<vk::Image> images_;
    std::vector<vk::ImageView> imageViews_;
    vk::Sampler imageSampler_;
    vk::DescriptorSetLayout descriptorSetLayout;
    std::vector<vk::DescriptorSet> descriptorSets;
    vk::PipelineLayout raycastPipelineLayout_, lightingPipelineLayout_, renderPipelineLayout_;
    vk::Pipeline raycastPipeline_, lightingPipeline_, renderPipeline_;
    std::vector<vk::CommandBuffer> commandBuffers_;

    std::vector<vk::Semaphore> imageAvailableSemaphores_, renderFinishedSemaphores_;
    std::vector<vk::Fence> inFlightFences_, imagesInFlightFences_;

    void createInstance();
    void selectPhysicalDevice();
    void createPermanentObjects();
    void createSwapchainObjects();
    void setupFrameObjects();

    QueueSupportDetails utils_getQueueSupportDetails();
    vk::DescriptorPool utils_createDescriptorPool(std::vector<vk::DescriptorType> descriptorTypes,int count);
    VmaBuffer utils_createBuffer(vk::DeviceSize size, vk::BufferUsageFlags bufferUsage, VmaMemoryUsage memoryUsage, VmaAllocationCreateFlags flags);
    SwapChainSupportDetails utils_getSwapChainSupportDetails();
    vk::DescriptorSetLayout utils_createDescriptorSetLayout(std::vector<vk::DescriptorSetLayoutBinding> bindings);
    std::vector<vk::DescriptorSet> utils_allocateDescriptorSets(vk::DescriptorPool pool, vk::DescriptorSetLayout layout);
    vk::ShaderModule utils_createShaderModule(std::string path);
    void utils_destroyBuffer(VmaBuffer buffer);
    const std::vector<const char *> utils_validationLayers = {
        "VK_LAYER_KHRONOS_validation"};
    const std::vector<const char *> utils_deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    friend class VoxelEngine;
    friend class Octree;
};