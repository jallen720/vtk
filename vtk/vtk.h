#pragma once

#include <vulkan/vulkan.h>
#include "ctk/ctk.h"

////////////////////////////////////////////////////////////
/// Macros
////////////////////////////////////////////////////////////
#ifdef VTK_EXPORTS
    #define VTK_API __declspec(dllexport)
#else
    #define VTK_API __declspec(dllimport)
#endif
#define VTK_UNSET_INDEX CTK_U32_MAX

namespace vtk {

////////////////////////////////////////////////////////////
/// Data
////////////////////////////////////////////////////////////
struct instance_config
{
    ctk::static_array<cstr, 8> Layers;
    ctk::static_array<cstr, 8> Extensions;
    b32                        Debug;
    cstr                       AppName;
};

struct instance
{
    VkInstance               Instance;
    VkDebugUtilsMessengerEXT DebugUtilsMessenger;
};

struct device_config
{
    ctk::static_array<cstr, 8> Extensions;
    VkPhysicalDeviceFeatures   Features;
};

struct queue_family_indexes
{
    u32 Graphics = VTK_UNSET_INDEX;
    u32 Present  = VTK_UNSET_INDEX;
};

struct device
{
    VkPhysicalDevice                 Physical;
    VkPhysicalDeviceMemoryProperties MemoryProperties;
    queue_family_indexes             QueueFamilyIndexes;
    VkSurfaceCapabilitiesKHR         SurfaceCapabilities;
    ctk::array<VkSurfaceFormatKHR>   SurfaceFormats;
    ctk::array<VkPresentModeKHR>     SurfacePresentModes;

    VkDevice                         Logical;
    VkQueue                          GraphicsQueue;
    VkQueue                          PresentQueue;
};

struct swapchain_image
{
    VkImage     Image;
    VkImageView View;
};

struct swapchain
{
    VkSwapchainKHR                        Swapchain;
    ctk::static_array<swapchain_image, 4> Images;
    VkExtent2D                            Extent;
};

struct buffer
{
    VkBuffer       Buffer;
    VkDeviceMemory Memory;
    u32            ElementCount;
    u32            ElementSize;
};

struct shader_module
{
    VkShaderModule        Module;
    VkShaderStageFlagBits StageBit;
};

struct vertex_attribute
{
    VkFormat Format;
    u32      Size;
    u32      Offset;
};

struct vertex_layout
{
    ctk::static_array<vertex_attribute, 4> Attributes;
    u32                                    Size;
};

struct vertex_input
{
    u32 Location;
    u32 Binding;
    u32 AttributeIndex;
};

struct graphics_pipeline_config
{
    ctk::static_array<shader_module *, 4> ShaderModules;
    ctk::static_array<vertex_input, 4>    VertexInputs;
    vertex_layout                         *VertexLayout;
    VkExtent2D                            ViewportExtent;
    VkPrimitiveTopology                   PrimitiveTopology;
    VkBool32                              DepthTesting;
};

struct graphics_pipeline
{
    VkPipeline       Pipeline;
    VkPipelineLayout Layout;
};

////////////////////////////////////////////////////////////
/// Interface
////////////////////////////////////////////////////////////
VTK_API
instance
CreateInstance(instance_config *Config);

VTK_API
device
CreateDevice(VkInstance Instance, VkSurfaceKHR PlatformSurface, device_config *Config);

VTK_API
swapchain
CreateSwapchain(VkSurfaceKHR PlatformSurface, device *Device);

VTK_API
VkCommandPool
CreateCommandPool(VkDevice LogicalDevice, u32 QueueFamilyIndex);

VTK_API
buffer
CreateBuffer(device *Device, u32 Size, VkBufferUsageFlags UsageFlags, VkMemoryPropertyFlags MemoryPropertyFlags);

VTK_API
shader_module
CreateShaderModule(VkDevice LogicalDevice, cstr Path, VkShaderStageFlagBits StageBit);

VTK_API
u32
PushVertexAttribute(vertex_layout *VertexLayout, u32 ElementCount);

VTK_API
graphics_pipeline
CreateGraphicsPipeline(VkDevice LogicalDevice, graphics_pipeline_config *Config);

VTK_API
void
WriteToHostCoherentBuffer(VkDevice LogicalDevice, buffer *Buffer, void *Data, VkDeviceSize Size, VkDeviceSize Offset);

VTK_API
void
ValidateVkResult(VkResult Result, cstr FunctionName, cstr FailureMessage);

} // vtk
