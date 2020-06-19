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
    ctk::sarray<cstr, 8> Layers;
    ctk::sarray<cstr, 8> Extensions;
    b32                  Debug;
    cstr                 AppName;
};

struct instance
{
    VkInstance               Handle;
    VkDebugUtilsMessengerEXT DebugUtilsMessenger;
};

struct device_config
{
    ctk::sarray<cstr, 8>     Extensions;
    VkPhysicalDeviceFeatures Features;
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
    VkImage     Handle;
    VkImageView View;
};

struct swapchain
{
    VkSwapchainKHR                  Handle;
    ctk::sarray<swapchain_image, 4> Images;
    VkFormat                        ImageFormat;
    VkExtent2D                      Extent;
};

struct buffer
{
    VkBuffer       Handle;
    VkDeviceMemory Memory;
};

struct attachment
{
    VkFormat            Format;
    VkAttachmentLoadOp  LoadOp;
    VkAttachmentStoreOp StoreOp;
    VkClearValue        ClearValue;
};

struct subpass
{
    ctk::sarray<VkAttachmentReference, 4> ColorAttachmentReferences;
    ctk::optional<VkAttachmentReference>  DepthAttachmentReference;
};

struct render_pass_config
{
    ctk::sarray<attachment, 4> Attachments;
    ctk::sarray<subpass, 4>    Subpasses;
};

struct render_pass
{
    VkRenderPass                 Handle;
    ctk::sarray<VkClearValue, 4> ClearValues;
};

struct framebuffer_config
{
    ctk::sarray<VkImageView, 4> Attachments;
    VkExtent2D                  Extent;
    u32                         Layers;
};

struct shader_module
{
    VkShaderModule        Handle;
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
    ctk::sarray<vertex_attribute, 4> Attributes;
    u32                              Size;
};

struct vertex_input
{
    u32 Location;
    u32 Binding;
    u32 AttributeIndex;
};

struct graphics_pipeline_config
{
    ctk::sarray<shader_module *, 4> ShaderModules;
    ctk::sarray<vertex_input, 4>    VertexInputs;
    vertex_layout                   *VertexLayout;
    VkExtent2D                      ViewportExtent;
    VkPrimitiveTopology             PrimitiveTopology;
    VkBool32                        DepthTesting;
};

struct graphics_pipeline
{
    VkPipeline       Handle;
    VkPipelineLayout Layout;
};

struct frame
{
    VkSemaphore ImageAquiredSemaphore;
    VkSemaphore RenderFinishedSemaphore;
    VkFence     InFlightFence;
};

struct frame_state
{
    ctk::sarray<frame, 4>   Frames;
    ctk::sarray<VkFence, 4> PreviousFrameInFlightFences;
    u32                     CurrentFrameIndex;
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
CreateSwapchain(device *Device, VkSurfaceKHR PlatformSurface);

VTK_API
VkCommandPool
CreateCommandPool(VkDevice LogicalDevice, u32 QueueFamilyIndex);

VTK_API
buffer
CreateBuffer(device *Device, u32 Size, VkBufferUsageFlags UsageFlags, VkMemoryPropertyFlags MemoryPropertyFlags);

VTK_API
render_pass
CreateRenderPass(VkDevice LogicalDevice, render_pass_config *Config);

VTK_API
VkFramebuffer
CreateFramebuffer(VkDevice LogicalDevice, VkRenderPass RenderPass, framebuffer_config *Config);

VTK_API
void
AllocateCommandBuffers(VkDevice LogicalDevice, VkCommandPool CommandPool, u32 Count, VkCommandBuffer *CommandBuffers);

VTK_API
shader_module
CreateShaderModule(VkDevice LogicalDevice, cstr Path, VkShaderStageFlagBits StageBit);

VTK_API
u32
PushVertexAttribute(vertex_layout *VertexLayout, u32 ElementCount);

VTK_API
graphics_pipeline
CreateGraphicsPipeline(VkDevice LogicalDevice, VkRenderPass RenderPass, graphics_pipeline_config *Config);

VTK_API
frame_state
CreateFrameState(VkDevice LogicalDevice, u32 FrameCount, u32 SwapchainImageCount);

VTK_API
void
WriteToHostCoherentBuffer(VkDevice LogicalDevice, buffer *Buffer, void *Data, VkDeviceSize Size, VkDeviceSize Offset);

VTK_API
void
ValidateVkResult(VkResult Result, cstr FunctionName, cstr FailureMessage);

} // vtk
