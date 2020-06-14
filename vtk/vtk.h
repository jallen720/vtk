#pragma once

#include <vulkan/vulkan.h>
#include "ctk/ctk.h"

#ifdef VTK_EXPORTS
    #define VTK_API __declspec(dllexport)
#else
    #define VTK_API __declspec(dllimport)
#endif

namespace vtk {

////////////////////////////////////////////////////////////
/// Data
////////////////////////////////////////////////////////////
struct vulkan_instance_config
{
    ctk::array<cstr> Layers;
    ctk::array<cstr> Extensions;
    b32 Debug;
    cstr AppName;
};

struct vulkan_instance
{
    VkInstance Instance;
    VkDebugUtilsMessengerEXT DebugUtilsMessenger;
};

////////////////////////////////////////////////////////////
/// Interface
////////////////////////////////////////////////////////////
VTK_API
vulkan_instance
CreateVulkanInstance(vulkan_instance_config *Config);

} // vtk
