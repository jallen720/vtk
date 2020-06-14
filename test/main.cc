#include "vtk/vtk.h"
#include "ctk/ctk.h"
#include "ctk/data.h"

s32
main()
{
    ctk::data VTKConfig = ctk::LoadData("assets/vtk_config.ctkd");
    vtk::vulkan_instance_config VulkanInstanceConfig = {};
    VulkanInstanceConfig.Debug = ctk::B32(&VTKConfig, "debug");
    VulkanInstanceConfig.AppName = ctk::CStr(&VTKConfig, "app_name");
    vtk::vulkan_instance VulkanInstance = vtk::CreateVulkanInstance(&VulkanInstanceConfig);
}
