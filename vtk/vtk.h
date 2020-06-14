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

struct device
{
    VkPhysicalDevice                 Physical;
    VkPhysicalDeviceMemoryProperties MemoryProperties;
    u32                              GraphicsIndex = VTK_UNSET_INDEX;
    u32                              PresentIndex = VTK_UNSET_INDEX;
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
void
ValidateVkResult(VkResult Result, cstr FunctionName, cstr FailureMessage);

} // vtk
