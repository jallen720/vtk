#include "vtk/vtk.h"

////////////////////////////////////////////////////////////
/// Macros
////////////////////////////////////////////////////////////
#define VK_RESULT_NAME(VK_RESULT) VK_RESULT, #VK_RESULT

#define LOAD_INSTANCE_EXTENSION_FUNCTION(INSTANCE, FUNCTION_NAME) \
    auto FUNCTION_NAME = (PFN_ ## FUNCTION_NAME)vkGetInstanceProcAddr(INSTANCE, #FUNCTION_NAME); \
    if(FUNCTION_NAME == NULL) \
    { \
        CTK_FATAL("failed to load instance extension function \"%s\"", #FUNCTION_NAME) \
    }

namespace vtk {

////////////////////////////////////////////////////////////
/// Data
////////////////////////////////////////////////////////////
struct vk_result_debug_info
{
    VkResult Result;
    cstr ResultName;
    cstr Message;
};

////////////////////////////////////////////////////////////
/// Internal
////////////////////////////////////////////////////////////
static VKAPI_ATTR VkBool32 VKAPI_CALL
DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverityFlagBits, VkDebugUtilsMessageTypeFlagsEXT MessageTypeFlags,
              const VkDebugUtilsMessengerCallbackDataEXT *CallbackData, void *UserData)
{
    cstr MessageID = CallbackData->pMessageIdName ? CallbackData->pMessageIdName : "";
    if(VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT & MessageSeverityFlagBits)
    {
        CTK_FATAL("VALIDATION LAYER [%s]: %s\n", MessageID, CallbackData->pMessage)
    }
    else if(VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT & MessageSeverityFlagBits)
    {
        ctk::Warning("VALIDATION LAYER [%s]: %s\n", MessageID, CallbackData->pMessage);
    }
    else
    {
        ctk::Info("VALIDATION LAYER [%s]: %s\n", MessageID, CallbackData->pMessage);
    }
    return VK_FALSE;
}

void
OutputVkResult(VkResult Result, cstr FunctionName)
{
    static vk_result_debug_info VK_RESULT_DEBUG_INFOS[] =
    {
        { VK_RESULT_NAME(VK_SUCCESS), "VULKAN SPEC ERROR MESSAGE: Command successfully completed." },
        { VK_RESULT_NAME(VK_NOT_READY), "VULKAN SPEC ERROR MESSAGE: A fence or query has not yet completed." },
        { VK_RESULT_NAME(VK_TIMEOUT), "VULKAN SPEC ERROR MESSAGE: A wait operation has not completed in the specified time." },
        { VK_RESULT_NAME(VK_EVENT_SET), "VULKAN SPEC ERROR MESSAGE: An event is signaled." },
        { VK_RESULT_NAME(VK_EVENT_RESET), "VULKAN SPEC ERROR MESSAGE: An event is unsignaled." },
        { VK_RESULT_NAME(VK_INCOMPLETE), "VULKAN SPEC ERROR MESSAGE: A return array was too small for the result." },
        { VK_RESULT_NAME(VK_SUBOPTIMAL_KHR), "VULKAN SPEC ERROR MESSAGE: A swapchain no longer matches the surface properties exactly, but can still be used to present to the surface successfully." },
        { VK_RESULT_NAME(VK_ERROR_OUT_OF_HOST_MEMORY), "VULKAN SPEC ERROR MESSAGE: A host memory allocation has failed." },
        { VK_RESULT_NAME(VK_ERROR_OUT_OF_DEVICE_MEMORY), "VULKAN SPEC ERROR MESSAGE: A device memory allocation has failed." },
        { VK_RESULT_NAME(VK_ERROR_INITIALIZATION_FAILED), "VULKAN SPEC ERROR MESSAGE: Initialization of an object could not be completed for implementation-specific reasons." },
        { VK_RESULT_NAME(VK_ERROR_DEVICE_LOST), "VULKAN SPEC ERROR MESSAGE: The logical or physical device has been lost." },
        { VK_RESULT_NAME(VK_ERROR_MEMORY_MAP_FAILED), "VULKAN SPEC ERROR MESSAGE: Mapping of a memory object has failed." },
        { VK_RESULT_NAME(VK_ERROR_LAYER_NOT_PRESENT), "VULKAN SPEC ERROR MESSAGE: A requested layer is not present or could not be loaded." },
        { VK_RESULT_NAME(VK_ERROR_EXTENSION_NOT_PRESENT), "VULKAN SPEC ERROR MESSAGE: A requested extension is not supported." },
        { VK_RESULT_NAME(VK_ERROR_FEATURE_NOT_PRESENT), "VULKAN SPEC ERROR MESSAGE: A requested feature is not supported." },
        { VK_RESULT_NAME(VK_ERROR_INCOMPATIBLE_DRIVER), "VULKAN SPEC ERROR MESSAGE: The requested version of Vulkan is not supported by the driver or is otherwise incompatible for implementation-specific reasons." },
        { VK_RESULT_NAME(VK_ERROR_TOO_MANY_OBJECTS), "VULKAN SPEC ERROR MESSAGE: Too many objects of the type have already been created." },
        { VK_RESULT_NAME(VK_ERROR_FORMAT_NOT_SUPPORTED), "VULKAN SPEC ERROR MESSAGE: A requested format is not supported on this device." },
        { VK_RESULT_NAME(VK_ERROR_FRAGMENTED_POOL), "VULKAN SPEC ERROR MESSAGE: A pool allocation has failed due to fragmentation of the pool’s memory. This must only be returned if no attempt to allocate host or device memory was made to accommodate the new allocation. This should be returned in preference to VK_ERROR_OUT_OF_POOL_MEMORY, but only if the implementation is certain that the pool allocation failure was due to fragmentation." },
        { VK_RESULT_NAME(VK_ERROR_SURFACE_LOST_KHR), "VULKAN SPEC ERROR MESSAGE: A surface is no longer available." },
        { VK_RESULT_NAME(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR), "VULKAN SPEC ERROR MESSAGE: The requested window is already in use by Vulkan or another API in a manner which prevents it from being used again." },
        { VK_RESULT_NAME(VK_ERROR_OUT_OF_DATE_KHR), "VULKAN SPEC ERROR MESSAGE: A surface has changed in such a way that it is no longer compatible with the swapchain, and further presentation requests using the swapchain will fail. Applications must query the new surface properties and recreate their swapchain if they wish to continue presenting to the surface." },
        { VK_RESULT_NAME(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR), "VULKAN SPEC ERROR MESSAGE: The display used by a swapchain does not use the same presentable image layout, or is incompatible in a way that prevents sharing an image." },
        { VK_RESULT_NAME(VK_ERROR_INVALID_SHADER_NV), "VULKAN SPEC ERROR MESSAGE: One or more shaders failed to compile or link. More details are reported back to the application via https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html#VK_EXT_debug_report if enabled." },
        { VK_RESULT_NAME(VK_ERROR_OUT_OF_POOL_MEMORY), "VULKAN SPEC ERROR MESSAGE: A pool memory allocation has failed. This must only be returned if no attempt to allocate host or device memory was made to accommodate the new allocation. If the failure was definitely due to fragmentation of the pool, VK_ERROR_FRAGMENTED_POOL should be returned instead." },
        { VK_RESULT_NAME(VK_ERROR_INVALID_EXTERNAL_HANDLE), "VULKAN SPEC ERROR MESSAGE: An external handle is not a valid handle of the specified type." },
        // { VK_RESULT_NAME(VK_ERROR_FRAGMENTATION), "VULKAN SPEC ERROR MESSAGE: A descriptor pool creation has failed due to fragmentation." },
        { VK_RESULT_NAME(VK_ERROR_INVALID_DEVICE_ADDRESS_EXT), "VULKAN SPEC ERROR MESSAGE: A buffer creation failed because the requested address is not available." },
        // { VK_RESULT_NAME(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS), "VULKAN SPEC ERROR MESSAGE: A buffer creation or memory allocation failed because the requested address is not available." },
        { VK_RESULT_NAME(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT), "VULKAN SPEC ERROR MESSAGE: An operation on a swapchain created with VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT failed as it did not have exlusive full-screen access. This may occur due to implementation-dependent reasons, outside of the application’s control." },
        // { VK_RESULT_NAME(VK_ERROR_UNKNOWN), "VULKAN SPEC ERROR MESSAGE: An unknown error has occurred; either the application has provided invalid input, or an implementation failure has occurred." },
    };
    vk_result_debug_info *DebugInfo = NULL;
    for(u32 VkResultDebugInfoIndex = 0; VkResultDebugInfoIndex < CTK_ARRAY_COUNT(VK_RESULT_DEBUG_INFOS); ++VkResultDebugInfoIndex)
    {
        DebugInfo = VK_RESULT_DEBUG_INFOS + VkResultDebugInfoIndex;
        if(DebugInfo->Result == Result)
        {
            break;
        }
    }
    if(!DebugInfo)
    {
        CTK_FATAL("failed to find debug info for VkResult %d", Result)
    }

    if(DebugInfo->Result == 0)
    {
        ctk::Info("%s() returned %s: %s", FunctionName, DebugInfo->ResultName, DebugInfo->Message);
    }
    else if(DebugInfo->Result > 0)
    {
        ctk::Warning("%s() returned %s: %s", FunctionName, DebugInfo->ResultName, DebugInfo->Message);
    }
    else
    {
        ctk::Error("%s() returned %s: %s", FunctionName, DebugInfo->ResultName, DebugInfo->Message);
    }
}

void
ValidateVkResult(VkResult Result, cstr FunctionName, cstr FailureMessage)
{
    if(Result != VK_SUCCESS)
    {
        OutputVkResult(Result, FunctionName);
        CTK_FATAL(FailureMessage)
    }
}

template<typename vk_object, typename loader, typename ...args>
static ctk::array<vk_object>
LoadVkObjects(loader Loader, args... Args)
{
    u32 Count = 0;
    Loader(Args..., &Count, NULL);
    auto VkObjects = ctk::CreateArray<vk_object>(Count);
    Loader(Args..., &Count, VkObjects.Data);
    return VkObjects;
}

static cstr
LayerName(VkLayerProperties * Properties)
{
    return Properties->layerName;
}

static cstr
ExtensionName(VkExtensionProperties * Properties)
{
    return Properties->extensionName;
}

template<typename properties, typename name_selector>
static b32
AddOnsSupported(ctk::array<cstr> * AddOnNames, ctk::array<properties> * SupportedAddOns, cstr Type, name_selector NameSelector)
{
    b32 AllSupported = true;
    for(u32 AddOnIndex = 0; AddOnIndex < AddOnNames->Count; ++AddOnIndex)
    {
        b32 Supported = false;
        cstr AddOnName = *At(AddOnNames, AddOnIndex);
        for(u32 SupportedAddOnsIndex = 0; SupportedAddOnsIndex < SupportedAddOns->Count; ++SupportedAddOnsIndex)
        {
            cstr SupportedAddOnName = NameSelector(At(SupportedAddOns, SupportedAddOnsIndex));
            if(ctk::StringEqual(AddOnName, SupportedAddOnName))
            {
                Supported = true;
                break;
            }
        }
        if(!Supported)
        {
            AllSupported = false;
            ctk::Error("%s \"%s\" is not supported", Type, AddOnName);
        }
    }
    return AllSupported;
}

template<typename properties, typename name_selector, typename loader, typename ...args>
static b32
AddOnsSupported(ctk::array<cstr> * AddOnNames, cstr Type, name_selector NameSelector, loader Loader, args... Args)
{
    auto SupportedAddOns = LoadVkObjects<properties>(Loader, Args...);
    b32 AllSupported = AddOnsSupported<properties>(AddOnNames, &SupportedAddOns, Type, NameSelector);
    ctk::Free(&SupportedAddOns);
    return AllSupported;
}

static VkInstance
CreateVkInstance(cstr AppName, ctk::array<cstr> *Extensions, ctk::array<cstr> *Layers,
                 VkDebugUtilsMessengerCreateInfoEXT *DebugUtilsMessengerCreateInfo)
{
    VkApplicationInfo AppInfo = {};
    AppInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    AppInfo.pNext              = NULL;
    AppInfo.pApplicationName   = AppName;
    AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    AppInfo.pEngineName        = AppName;
    AppInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
    AppInfo.apiVersion         = VK_API_VERSION_1_0;

    VkInstanceCreateInfo InstanceCreateInfo = {};
    InstanceCreateInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    InstanceCreateInfo.pNext                   = DebugUtilsMessengerCreateInfo;
    InstanceCreateInfo.pApplicationInfo        = &AppInfo;
    InstanceCreateInfo.enabledLayerCount       = Layers->Count;
    InstanceCreateInfo.ppEnabledLayerNames     = Layers->Data;
    InstanceCreateInfo.enabledExtensionCount   = Extensions->Count;
    InstanceCreateInfo.ppEnabledExtensionNames = Extensions->Data;

    VkInstance Instance = VK_NULL_HANDLE;
    VkResult Result = vkCreateInstance(&InstanceCreateInfo, NULL, &Instance);
    ValidateVkResult(Result, "vkCreateInstance", "failed to create Vulkan instance");
    return Instance;
}

////////////////////////////////////////////////////////////
/// Interface
////////////////////////////////////////////////////////////
vulkan_instance
CreateVulkanInstance(vulkan_instance_config *Config)
{
    vulkan_instance VulkanInstance = {};
    auto Extensions = ctk::CreateArrayEmpty<cstr>(16);
    auto Layers = ctk::CreateArrayEmpty<cstr>(16);
    ctk::array<cstr> *RequestedLayers = &Config->Layers;
    ctk::array<cstr> *RequestedExtensions = &Config->Extensions;
    if(RequestedLayers->Count > 0)
    {
        if(!AddOnsSupported<VkLayerProperties>(RequestedLayers, "layer", LayerName, vkEnumerateInstanceLayerProperties))
        {
            CTK_FATAL("not all requested layers supported")
        }
        ctk::Push(&Layers, RequestedLayers->Data, RequestedLayers->Count);
    }

    if(RequestedExtensions->Count > 0)
    {
        if(!AddOnsSupported<VkExtensionProperties>(RequestedExtensions, "extension", ExtensionName, vkEnumerateInstanceExtensionProperties,
                                                   (cstr)NULL))
        {
            CTK_FATAL("not all requested extensions supported")
        }
        ctk::Push(&Extensions, RequestedExtensions->Data, RequestedExtensions->Count);
    }

    if(Config->Debug)
    {
        // Add debug extensions and layers.
        ctk::Push(&Extensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        ctk::Push(&Layers, "VK_LAYER_LUNARG_standard_validation");

        VkDebugUtilsMessengerCreateInfoEXT DebugUtilsMessengerCreateInfo = {};
        DebugUtilsMessengerCreateInfo.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        DebugUtilsMessengerCreateInfo.pNext           = NULL;
        DebugUtilsMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                                        // VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                                                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        DebugUtilsMessengerCreateInfo.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                                        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                                        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        DebugUtilsMessengerCreateInfo.pfnUserCallback = DebugCallback;
        DebugUtilsMessengerCreateInfo.pUserData       = NULL;

        VulkanInstance.Instance = CreateVkInstance(Config->AppName, &Extensions, &Layers, &DebugUtilsMessengerCreateInfo);

        LOAD_INSTANCE_EXTENSION_FUNCTION(VulkanInstance.Instance, vkCreateDebugUtilsMessengerEXT)
        VkResult Result = vkCreateDebugUtilsMessengerEXT(VulkanInstance.Instance, &DebugUtilsMessengerCreateInfo, NULL,
                                                         &VulkanInstance.DebugUtilsMessenger);
        ValidateVkResult(Result, "vkCreateDebugUtilsMessengerEXT", "failed to create debug messenger");
    }
    else
    {
        VulkanInstance.Instance = CreateVkInstance(Config->AppName, &Extensions, &Layers, NULL);
    }

    // Cleanup
    ctk::Free(&Extensions);
    ctk::Free(&Layers);

    return VulkanInstance;
}

} // vtk
