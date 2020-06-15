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
    cstr     ResultName;
    cstr     Message;
};

struct device_info
{
    VkPhysicalDeviceProperties          Properties;
    VkPhysicalDeviceFeatures            Features;
    VkPhysicalDeviceMemoryProperties    MemoryProperties;
    VkSurfaceCapabilitiesKHR            SurfaceCapabilities;
    ctk::array<VkExtensionProperties>   Extensions;
    ctk::array<VkSurfaceFormatKHR>      SurfaceFormats;
    ctk::array<VkPresentModeKHR>        SurfacePresentModes;
    ctk::array<VkQueueFamilyProperties> QueueFamilies;
    u32                                 GraphicsIndex = VTK_UNSET_INDEX;
    u32                                 PresentIndex = VTK_UNSET_INDEX;
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

static void
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
LayerName(VkLayerProperties *Properties)
{
    return Properties->layerName;
}

static cstr
ExtensionName(VkExtensionProperties *Properties)
{
    return Properties->extensionName;
}

template<typename properties, typename name_selector, u32 size>
static b32
AddOnsSupported(ctk::static_array<cstr, size> *AddOnNames, ctk::array<properties> *SupportedAddOns, name_selector NameSelector)
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
            ctk::Error("add-on \"%s\" is not supported", AddOnName);
        }
    }
    return AllSupported;
}

template<typename properties, typename name_selector, typename loader, u32 size, typename ...args>
static b32
AddOnsSupported(ctk::static_array<cstr, size> *AddOnNames, name_selector NameSelector, loader Loader, args... Args)
{
    auto SupportedAddOns = LoadVkObjects<properties>(Loader, Args...);
    b32 AllSupported = AddOnsSupported<properties>(AddOnNames, &SupportedAddOns, NameSelector);
    ctk::Free(&SupportedAddOns);
    return AllSupported;
}

template<u32 size>
static VkInstance
CreateVkInstance(cstr AppName, ctk::static_array<cstr, size> *Extensions, ctk::static_array<cstr, size> *Layers,
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
    InstanceCreateInfo.flags                   = 0;
    InstanceCreateInfo.pApplicationInfo        = &AppInfo;
    InstanceCreateInfo.enabledLayerCount       = Layers->Count;
    InstanceCreateInfo.ppEnabledLayerNames     = Layers->Data;
    InstanceCreateInfo.enabledExtensionCount   = Extensions->Count;
    InstanceCreateInfo.ppEnabledExtensionNames = Extensions->Data;

    VkInstance Instance = {};
    VkResult Result = vkCreateInstance(&InstanceCreateInfo, NULL, &Instance);
    ValidateVkResult(Result, "vkCreateInstance", "failed to create Vulkan instance");
    return Instance;
}

static void
DeviceInfoFree(device_info *DeviceInfo)
{
    Free(&DeviceInfo->Extensions);
    Free(&DeviceInfo->SurfaceFormats);
    Free(&DeviceInfo->SurfacePresentModes);
    Free(&DeviceInfo->QueueFamilies);
    *DeviceInfo = {};
}

static VkDeviceQueueCreateInfo
CreateQueueCreateInfo(u32 QueueFamilyIndex)
{
    static const f32 QUEUE_PRIORITIES[] = { 1.0f };

    VkDeviceQueueCreateInfo QueueCreateInfo = {};
    QueueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    QueueCreateInfo.flags            = 0;
    QueueCreateInfo.queueFamilyIndex = QueueFamilyIndex;
    QueueCreateInfo.queueCount       = CTK_ARRAY_COUNT(QUEUE_PRIORITIES);
    QueueCreateInfo.pQueuePriorities = QUEUE_PRIORITIES;

    return QueueCreateInfo;
}

static VkImageView
CreateImageView(VkDevice LogicalDevice, VkImage Image, VkFormat Format, VkImageAspectFlags AspectFlags)
{
    VkImageViewCreateInfo ImageViewCreateInfo = {};
    ImageViewCreateInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ImageViewCreateInfo.image                           = Image;
    ImageViewCreateInfo.flags                           = 0;
    ImageViewCreateInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    ImageViewCreateInfo.format                          = Format;
    ImageViewCreateInfo.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    ImageViewCreateInfo.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    ImageViewCreateInfo.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    ImageViewCreateInfo.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    ImageViewCreateInfo.subresourceRange.aspectMask     = AspectFlags;
    ImageViewCreateInfo.subresourceRange.baseMipLevel   = 0;
    ImageViewCreateInfo.subresourceRange.levelCount     = 1;
    ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    ImageViewCreateInfo.subresourceRange.layerCount     = 1;

    VkImageView ImageView = {};
    VkResult Result = vkCreateImageView(LogicalDevice, &ImageViewCreateInfo, NULL, &ImageView);
    ValidateVkResult(Result, "vkCreateImageView", "failed to create image view");
    return ImageView;
}

static u32
FindMemoryTypeIndex(VkPhysicalDeviceMemoryProperties * MemoryProperties, u32 MemoryTypeBits, VkMemoryPropertyFlags MemoryPropertyFlags)
{
    for(u32 MemoryTypeIndex = 0; MemoryTypeIndex < MemoryProperties->memoryTypeCount; ++MemoryTypeIndex)
    {
        // Check if memory at index has correct type and properties.
        if((MemoryTypeBits & (1 << MemoryTypeIndex)) &&
           (MemoryProperties->memoryTypes[MemoryTypeIndex].propertyFlags & MemoryPropertyFlags) == MemoryPropertyFlags)
        {
            return MemoryTypeIndex;
        }
    }
    CTK_FATAL("failed to find memory type")
}

////////////////////////////////////////////////////////////
/// Interface
////////////////////////////////////////////////////////////
instance
CreateInstance(instance_config *Config)
{
    instance Instance = {};
    auto *Layers = &Config->Layers;
    auto *Extensions = &Config->Extensions;
    if(!AddOnsSupported<VkLayerProperties>(Layers, LayerName, vkEnumerateInstanceLayerProperties))
    {
        CTK_FATAL("not all requested layers supported")
    }

    if(!AddOnsSupported<VkExtensionProperties>(Extensions, ExtensionName, vkEnumerateInstanceExtensionProperties, (cstr)NULL))
    {
        CTK_FATAL("not all requested extensions supported")
    }

    if(Config->Debug)
    {
        // Ensure add-on arrays can fit debug add-ons.
        CTK_ASSERT(Layers->Count < Layers->Size)
        CTK_ASSERT(Extensions->Count < Extensions->Size)

        // Add debug extensions and layers.
        ctk::Push(Extensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        ctk::Push(Layers, "VK_LAYER_LUNARG_standard_validation");

        VkDebugUtilsMessengerCreateInfoEXT DebugUtilsMessengerCreateInfo = {};
        DebugUtilsMessengerCreateInfo.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        DebugUtilsMessengerCreateInfo.pNext           = NULL;
        DebugUtilsMessengerCreateInfo.flags           = 0;
        DebugUtilsMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                                        // VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                                                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        DebugUtilsMessengerCreateInfo.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                                        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                                        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        DebugUtilsMessengerCreateInfo.pfnUserCallback = DebugCallback;
        DebugUtilsMessengerCreateInfo.pUserData       = NULL;

        Instance.Instance = CreateVkInstance(Config->AppName, Extensions, Layers, &DebugUtilsMessengerCreateInfo);

        LOAD_INSTANCE_EXTENSION_FUNCTION(Instance.Instance, vkCreateDebugUtilsMessengerEXT)
        VkResult Result = vkCreateDebugUtilsMessengerEXT(Instance.Instance, &DebugUtilsMessengerCreateInfo, NULL,
                                                         &Instance.DebugUtilsMessenger);
        ValidateVkResult(Result, "vkCreateDebugUtilsMessengerEXT", "failed to create debug messenger");
    }
    else
    {
        Instance.Instance = CreateVkInstance(Config->AppName, Extensions, Layers, NULL);
    }
    return Instance;
}

device
CreateDevice(VkInstance Instance, VkSurfaceKHR PlatformSurface, device_config *Config)
{
    device Device = {};
    auto *Extensions = &Config->Extensions;

    ////////////////////////////////////////////////////////////
    /// Physical Device
    ////////////////////////////////////////////////////////////
    auto PhysicalDevices = LoadVkObjects<VkPhysicalDevice>(vkEnumeratePhysicalDevices, Instance);
    b32 FoundSuitableDevice = false;
    for(u32 PhysicalDeviceIndex = 0; PhysicalDeviceIndex < PhysicalDevices.Count && !FoundSuitableDevice; ++PhysicalDeviceIndex)
    {
        device_info SelectedDeviceInfo = {};
        VkPhysicalDevice PhysicalDevice = PhysicalDevices[PhysicalDeviceIndex];

        ////////////////////////////////////////////////////////////
        /// Device Info Loading
        ////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////
        /// Potential future physical device information.
        ////////////////////////////////////////////////////////////
        // vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR
        // vkGetDisplayModePropertiesKHR
        // vkGetDisplayPlaneCapabilitiesKHR
        // vkGetDisplayPlaneSupportedDisplaysKHR
        // vkGetPhysicalDeviceDisplayPlanePropertiesKHR
        // vkGetPhysicalDeviceDisplayPropertiesKHR
        // vkGetPhysicalDeviceExternalBufferProperties
        // vkGetPhysicalDeviceExternalBufferPropertiesKHR
        // vkGetPhysicalDeviceExternalFenceProperties
        // vkGetPhysicalDeviceExternalFencePropertiesKHR
        // vkGetPhysicalDeviceExternalSemaphoreProperties
        // vkGetPhysicalDeviceExternalSemaphorePropertiesKHR
        // vkGetPhysicalDeviceImageFormatProperties
        // vkGetPhysicalDevicePresentRectanglesKHR
        // vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR
        // vkGetPhysicalDeviceSparseImageFormatProperties
        // vkGetPhysicalDeviceFormatProperties

        // Collect requirements data.
        vkGetPhysicalDeviceProperties(PhysicalDevice, &SelectedDeviceInfo.Properties);
        vkGetPhysicalDeviceFeatures(PhysicalDevice, &SelectedDeviceInfo.Features);
        vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &SelectedDeviceInfo.MemoryProperties);
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, PlatformSurface, &SelectedDeviceInfo.SurfaceCapabilities);
        SelectedDeviceInfo.Extensions =
            LoadVkObjects<VkExtensionProperties>(vkEnumerateDeviceExtensionProperties, PhysicalDevice, (cstr)NULL);
        SelectedDeviceInfo.SurfaceFormats =
            LoadVkObjects<VkSurfaceFormatKHR>(vkGetPhysicalDeviceSurfaceFormatsKHR, PhysicalDevice, PlatformSurface);
        SelectedDeviceInfo.SurfacePresentModes =
            LoadVkObjects<VkPresentModeKHR>(vkGetPhysicalDeviceSurfacePresentModesKHR, PhysicalDevice, PlatformSurface);
        SelectedDeviceInfo.QueueFamilies =
            LoadVkObjects<VkQueueFamilyProperties>(vkGetPhysicalDeviceQueueFamilyProperties, PhysicalDevice);

        // Find queue family indexes.
        for(u32 QueueFamilyIndex = 0; QueueFamilyIndex < SelectedDeviceInfo.QueueFamilies.Count; ++QueueFamilyIndex)
        {
            VkQueueFamilyProperties *QueueFamily = SelectedDeviceInfo.QueueFamilies + QueueFamilyIndex;
            if(QueueFamily->queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                SelectedDeviceInfo.GraphicsIndex = QueueFamilyIndex;
            }
            VkBool32 PresentationSupported = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, QueueFamilyIndex, PlatformSurface, &PresentationSupported);
            if(PresentationSupported == VK_TRUE)
            {
                SelectedDeviceInfo.PresentIndex = QueueFamilyIndex;
            }
        }

        ////////////////////////////////////////////////////////////
        /// Device Info Validation
        ////////////////////////////////////////////////////////////

        // Check if device has requested features.
        #define _VTK_DEVICE_FEATURE_CHECK(FEATURE) \
            if(Config->Features.FEATURE && !SelectedDeviceInfo.Features.FEATURE) \
            { \
                DeviceFeaturesSupported = false; \
                ctk::Error("requested device feature \"" #FEATURE "\" not supported"); \
            }

        b32 DeviceFeaturesSupported = true;
        _VTK_DEVICE_FEATURE_CHECK(robustBufferAccess)
        _VTK_DEVICE_FEATURE_CHECK(fullDrawIndexUint32)
        _VTK_DEVICE_FEATURE_CHECK(imageCubeArray)
        _VTK_DEVICE_FEATURE_CHECK(independentBlend)
        _VTK_DEVICE_FEATURE_CHECK(geometryShader)
        _VTK_DEVICE_FEATURE_CHECK(tessellationShader)
        _VTK_DEVICE_FEATURE_CHECK(sampleRateShading)
        _VTK_DEVICE_FEATURE_CHECK(dualSrcBlend)
        _VTK_DEVICE_FEATURE_CHECK(logicOp)
        _VTK_DEVICE_FEATURE_CHECK(multiDrawIndirect)
        _VTK_DEVICE_FEATURE_CHECK(drawIndirectFirstInstance)
        _VTK_DEVICE_FEATURE_CHECK(depthClamp)
        _VTK_DEVICE_FEATURE_CHECK(depthBiasClamp)
        _VTK_DEVICE_FEATURE_CHECK(fillModeNonSolid)
        _VTK_DEVICE_FEATURE_CHECK(depthBounds)
        _VTK_DEVICE_FEATURE_CHECK(wideLines)
        _VTK_DEVICE_FEATURE_CHECK(largePoints)
        _VTK_DEVICE_FEATURE_CHECK(alphaToOne)
        _VTK_DEVICE_FEATURE_CHECK(multiViewport)
        _VTK_DEVICE_FEATURE_CHECK(samplerAnisotropy)
        _VTK_DEVICE_FEATURE_CHECK(textureCompressionETC2)
        _VTK_DEVICE_FEATURE_CHECK(textureCompressionASTC_LDR)
        _VTK_DEVICE_FEATURE_CHECK(textureCompressionBC)
        _VTK_DEVICE_FEATURE_CHECK(occlusionQueryPrecise)
        _VTK_DEVICE_FEATURE_CHECK(pipelineStatisticsQuery)
        _VTK_DEVICE_FEATURE_CHECK(vertexPipelineStoresAndAtomics)
        _VTK_DEVICE_FEATURE_CHECK(fragmentStoresAndAtomics)
        _VTK_DEVICE_FEATURE_CHECK(shaderTessellationAndGeometryPointSize)
        _VTK_DEVICE_FEATURE_CHECK(shaderImageGatherExtended)
        _VTK_DEVICE_FEATURE_CHECK(shaderStorageImageExtendedFormats)
        _VTK_DEVICE_FEATURE_CHECK(shaderStorageImageMultisample)
        _VTK_DEVICE_FEATURE_CHECK(shaderStorageImageReadWithoutFormat)
        _VTK_DEVICE_FEATURE_CHECK(shaderStorageImageWriteWithoutFormat)
        _VTK_DEVICE_FEATURE_CHECK(shaderUniformBufferArrayDynamicIndexing)
        _VTK_DEVICE_FEATURE_CHECK(shaderSampledImageArrayDynamicIndexing)
        _VTK_DEVICE_FEATURE_CHECK(shaderStorageBufferArrayDynamicIndexing)
        _VTK_DEVICE_FEATURE_CHECK(shaderStorageImageArrayDynamicIndexing)
        _VTK_DEVICE_FEATURE_CHECK(shaderClipDistance)
        _VTK_DEVICE_FEATURE_CHECK(shaderCullDistance)
        _VTK_DEVICE_FEATURE_CHECK(shaderFloat64)
        _VTK_DEVICE_FEATURE_CHECK(shaderInt64)
        _VTK_DEVICE_FEATURE_CHECK(shaderInt16)
        _VTK_DEVICE_FEATURE_CHECK(shaderResourceResidency)
        _VTK_DEVICE_FEATURE_CHECK(shaderResourceMinLod)
        _VTK_DEVICE_FEATURE_CHECK(sparseBinding)
        _VTK_DEVICE_FEATURE_CHECK(sparseResidencyBuffer)
        _VTK_DEVICE_FEATURE_CHECK(sparseResidencyImage2D)
        _VTK_DEVICE_FEATURE_CHECK(sparseResidencyImage3D)
        _VTK_DEVICE_FEATURE_CHECK(sparseResidency2Samples)
        _VTK_DEVICE_FEATURE_CHECK(sparseResidency4Samples)
        _VTK_DEVICE_FEATURE_CHECK(sparseResidency8Samples)
        _VTK_DEVICE_FEATURE_CHECK(sparseResidency16Samples)
        _VTK_DEVICE_FEATURE_CHECK(sparseResidencyAliased)
        _VTK_DEVICE_FEATURE_CHECK(variableMultisampleRate)
        _VTK_DEVICE_FEATURE_CHECK(inheritedQueries)

        // Check if physical device meets other requirements.
        b32 SwapchainsSupported = SelectedDeviceInfo.SurfaceFormats.Count != 0 &&
                                  SelectedDeviceInfo.SurfacePresentModes.Count != 0;
        if(SelectedDeviceInfo.Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
           SelectedDeviceInfo.GraphicsIndex != VTK_UNSET_INDEX &&
           SelectedDeviceInfo.PresentIndex != VTK_UNSET_INDEX &&
           AddOnsSupported<VkExtensionProperties>(Extensions, &SelectedDeviceInfo.Extensions, ExtensionName);
           DeviceFeaturesSupported &&
           SwapchainsSupported)
        {
            ctk::Info("physical device \"%s\" satisfies all requirements", SelectedDeviceInfo.Properties.deviceName);
            FoundSuitableDevice = true;

            // Initialize physical device with selected device info.
            Device.Physical            = PhysicalDevice;
            Device.MemoryProperties    = SelectedDeviceInfo.MemoryProperties;
            Device.GraphicsIndex       = SelectedDeviceInfo.GraphicsIndex;
            Device.PresentIndex        = SelectedDeviceInfo.PresentIndex;
            Device.SurfaceCapabilities = SelectedDeviceInfo.SurfaceCapabilities;
            Device.SurfaceFormats      = ctk::CreateArray(&SelectedDeviceInfo.SurfaceFormats);
            Device.SurfacePresentModes = ctk::CreateArray(&SelectedDeviceInfo.SurfacePresentModes);
        }
        else
        {
            ctk::Error("physical device \"%s\" does not satisfy all requirements", SelectedDeviceInfo.Properties.deviceName);
        }
        DeviceInfoFree(&SelectedDeviceInfo);
    }
    if(!FoundSuitableDevice)
    {
        CTK_FATAL("failed to find suitable device")
    }

    ////////////////////////////////////////////////////////////
    /// Logical Device
    ////////////////////////////////////////////////////////////
    ctk::static_array<VkDeviceQueueCreateInfo, 2> QueueCreateInfos = {};
    Push(&QueueCreateInfos, CreateQueueCreateInfo(Device.GraphicsIndex));
    if(Device.PresentIndex != Device.GraphicsIndex)
    {
        Push(&QueueCreateInfos, CreateQueueCreateInfo(Device.PresentIndex));
    }

    VkDeviceCreateInfo LogicalDeviceCreateInfo = {};
    LogicalDeviceCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    LogicalDeviceCreateInfo.flags                   = 0;
    LogicalDeviceCreateInfo.queueCreateInfoCount    = QueueCreateInfos.Count;
    LogicalDeviceCreateInfo.pQueueCreateInfos       = QueueCreateInfos.Data;
    LogicalDeviceCreateInfo.enabledLayerCount       = 0;
    LogicalDeviceCreateInfo.ppEnabledLayerNames     = NULL;
    LogicalDeviceCreateInfo.enabledExtensionCount   = Extensions->Count;
    LogicalDeviceCreateInfo.ppEnabledExtensionNames = Extensions->Data;
    LogicalDeviceCreateInfo.pEnabledFeatures        = &Config->Features;

    VkResult Result = vkCreateDevice(Device.Physical, &LogicalDeviceCreateInfo, NULL, &Device.Logical);
    ValidateVkResult(Result, "vkCreateDevice", "failed to create logical device");

    // Get logical device queues.
    static const u32 QUEUE_INDEX = 0; // Currently only supporting 1 queue per family.
    vkGetDeviceQueue(Device.Logical, Device.GraphicsIndex, QUEUE_INDEX, &Device.GraphicsQueue);
    vkGetDeviceQueue(Device.Logical, Device.PresentIndex, QUEUE_INDEX, &Device.PresentQueue);

    // Cleanup
    Free(&PhysicalDevices);

    return Device;
}

swapchain
CreateSwapchain(VkSurfaceKHR PlatformSurface, device *Device)
{
    swapchain Swapchain = {};

    ////////////////////////////////////////////////////////////
    /// Configuration
    ////////////////////////////////////////////////////////////

    // Default to first surface format.
    VkSurfaceFormatKHR SelectedSurfaceFormat = Device->SurfaceFormats[0];
    for(u32 SurfaceFormatIndex = 0; SurfaceFormatIndex < Device->SurfaceFormats.Count; ++SurfaceFormatIndex)
    {
        VkSurfaceFormatKHR SurfaceFormat = Device->SurfaceFormats[SurfaceFormatIndex];

        // Prefer 4-component 8-bit BGRA unsigned normalized format and sRGB color space.
        if(SurfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM && SurfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            SelectedSurfaceFormat = SurfaceFormat;
            break;
        }
    }

    // Default to FIFO (only present mode with guarenteed availability).
    VkPresentModeKHR SelectedSurfacePresentMode = VK_PRESENT_MODE_FIFO_KHR;
    for(u32 SurfacePresentModeIndex = 0; SurfacePresentModeIndex < Device->SurfacePresentModes.Count; ++SurfacePresentModeIndex)
    {
        VkPresentModeKHR SurfacePresentMode = Device->SurfacePresentModes[SurfacePresentModeIndex];

        // Mailbox is the preferred present mode if available.
        if(SurfacePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            SelectedSurfacePresentMode = SurfacePresentMode;
            break;
        }
    }

    // Set image count to min image count + 1 or max image count (whichever is smaller).
    u32 SelectedImageCount = Device->SurfaceCapabilities.minImageCount + 1;
    if(Device->SurfaceCapabilities.maxImageCount > 0 && SelectedImageCount > Device->SurfaceCapabilities.maxImageCount)
    {
        SelectedImageCount = Device->SurfaceCapabilities.maxImageCount;
    }

    // Verify current extent has been set for surface.
    if(Device->SurfaceCapabilities.currentExtent.width == UINT32_MAX)
    {
        CTK_FATAL("current extent not set for surface")
    }

    ////////////////////////////////////////////////////////////
    /// Swapchain Creation
    ////////////////////////////////////////////////////////////
    u32 GraphicsIndex = Device->GraphicsIndex;
    u32 PresentIndex = Device->PresentIndex;
    u32 QueueFamilyIndexes[] = { GraphicsIndex, PresentIndex };

    VkSwapchainCreateInfoKHR SwapchainCreateInfo = {};
    SwapchainCreateInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    SwapchainCreateInfo.surface          = PlatformSurface;
    SwapchainCreateInfo.flags            = 0;
    SwapchainCreateInfo.minImageCount    = SelectedImageCount;
    SwapchainCreateInfo.imageFormat      = SelectedSurfaceFormat.format;
    SwapchainCreateInfo.imageColorSpace  = SelectedSurfaceFormat.colorSpace;
    SwapchainCreateInfo.imageExtent      = Device->SurfaceCapabilities.currentExtent;
    SwapchainCreateInfo.imageArrayLayers = 1; // Always 1 for standard images.
    SwapchainCreateInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    SwapchainCreateInfo.preTransform     = Device->SurfaceCapabilities.currentTransform;
    SwapchainCreateInfo.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    SwapchainCreateInfo.presentMode      = SelectedSurfacePresentMode;
    SwapchainCreateInfo.clipped          = VK_TRUE;
    SwapchainCreateInfo.oldSwapchain     = VK_NULL_HANDLE;
    if(GraphicsIndex != PresentIndex)
    {
        SwapchainCreateInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        SwapchainCreateInfo.queueFamilyIndexCount = CTK_ARRAY_COUNT(QueueFamilyIndexes);
        SwapchainCreateInfo.pQueueFamilyIndices   = QueueFamilyIndexes;
    }
    else
    {
        SwapchainCreateInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
        SwapchainCreateInfo.queueFamilyIndexCount = 0;
        SwapchainCreateInfo.pQueueFamilyIndices   = NULL;
    }

    VkResult Result = vkCreateSwapchainKHR(Device->Logical, &SwapchainCreateInfo, NULL, &Swapchain.Swapchain);
    ValidateVkResult(Result, "vkCreateSwapchainKHR", "failed to create swapchain");

    ////////////////////////////////////////////////////////////
    /// Swapchain Image Creation
    ////////////////////////////////////////////////////////////
    auto Images = LoadVkObjects<VkImage>(vkGetSwapchainImagesKHR, Device->Logical, Swapchain.Swapchain);
    for(u32 ImageIndex = 0; ImageIndex < Images.Count; ++ImageIndex)
    {
        swapchain_image *SwapchainImage = ctk::Push(&Swapchain.Images);
        SwapchainImage->Image = Images[ImageIndex];
        SwapchainImage->View = CreateImageView(Device->Logical, SwapchainImage->Image, SelectedSurfaceFormat.format,
                                               VK_IMAGE_ASPECT_COLOR_BIT);
    }

    // Cleanup
    Free(&Images);

    return Swapchain;
}

VkCommandPool
CreateCommandPool(VkDevice LogicalDevice, u32 QueueFamilyIndex)
{
    VkCommandPoolCreateInfo CommandPoolCreateInfo = {};
    CommandPoolCreateInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    CommandPoolCreateInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    CommandPoolCreateInfo.queueFamilyIndex = QueueFamilyIndex;

    VkCommandPool CommandPool = {};
    VkResult Result = vkCreateCommandPool(LogicalDevice, &CommandPoolCreateInfo, NULL, &CommandPool);
    ValidateVkResult(Result, "vkCreateCommandPool", "failed to create command pool");
    return CommandPool;
}

buffer
CreateBuffer(device *Device, u32 Size, VkBufferUsageFlags UsageFlags, VkMemoryPropertyFlags MemoryPropertyFlags)
{
    buffer Buffer = {};

    // Buffer Creation
    VkBufferCreateInfo BufferCreateInfo = {};
    BufferCreateInfo.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    BufferCreateInfo.size                  = Size;
    BufferCreateInfo.usage                 = UsageFlags;
    BufferCreateInfo.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
    BufferCreateInfo.queueFamilyIndexCount = 0;
    BufferCreateInfo.pQueueFamilyIndices   = NULL; // Ignored if sharingMode is not VK_SHARING_MODE_CONCURRENT.
    {
        VkResult Result = vkCreateBuffer(Device->Logical, &BufferCreateInfo, NULL, &Buffer.Buffer);
        ValidateVkResult(Result, "vkCreateBuffer", "failed to create buffer");
    }

    // Memory Allocation
    VkMemoryRequirements MemoryRequirements = {};
    vkGetBufferMemoryRequirements(Device->Logical, Buffer.Buffer, &MemoryRequirements);

    VkMemoryAllocateInfo MemoryAllocateInfo = {};
    MemoryAllocateInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    MemoryAllocateInfo.allocationSize  = MemoryRequirements.size;
    MemoryAllocateInfo.memoryTypeIndex = FindMemoryTypeIndex(&Device->MemoryProperties, MemoryRequirements.memoryTypeBits,
                                                             MemoryPropertyFlags);

    // SOURCE: https://vulkan-tutorial.com/Vertex_buffers/Staging_buffer
    // It should be noted that in a real world application, you're not supposed to actually call vkAllocateMemory for
    // every individual buffer. The maximum number of simultaneous memory allocations is limited by the
    // maxMemoryAllocationCount physical device limit, which may be as low as 4096 even on high end hardware like an
    // NVIDIA GTX 1080. The right way to allocate memory for a large number of objects at the same time is to create a
    // custom allocator that splits up a single allocation among many different objects by using the offset parameters
    // that we've seen in many functions.
    VkResult allocate_memory_result = vkAllocateMemory(Device->Logical, &MemoryAllocateInfo, NULL, &Buffer.Memory);
    ValidateVkResult(allocate_memory_result, "vkAllocateMemory", "failed to allocate memory for buffer");

    // Bind device memory to buffer object.
    {
        VkResult Result = vkBindBufferMemory(Device->Logical, Buffer.Buffer, Buffer.Memory, 0);
        ValidateVkResult(Result, "vkBindBufferMemory", "failed to bind buffer memory");
    }

    return Buffer;
}

shader_stages
CreateShaderStages(VkDevice LogicalDevice, shader_stage_config *Configs, u32 ConfigCount)
{
    shader_stages ShaderStages = {};
    ShaderStages.Modules = ctk::CreateArray<VkShaderModule>(ConfigCount);
    ShaderStages.CreateInfos = ctk::CreateArray<VkPipelineShaderStageCreateInfo>(ConfigCount);
    for(u32 ConfigIndex = 0; ConfigIndex < ConfigCount; ++ConfigIndex)
    {
        shader_stage_config             *Config     = Configs + ConfigIndex;
        VkShaderModule                  *Module     = ShaderStages.Modules + ConfigIndex;
        VkPipelineShaderStageCreateInfo *CreateInfo = ShaderStages.CreateInfos + ConfigIndex;

        ctk::array<u8> ShaderByteCode = ctk::ReadFile<u8>(Config->Path);

        // Create Module from shader bytecode.
        VkShaderModuleCreateInfo ShaderModuleCreateInfo = {};
        ShaderModuleCreateInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        ShaderModuleCreateInfo.flags    = 0;
        ShaderModuleCreateInfo.codeSize = ByteSize(&ShaderByteCode);
        ShaderModuleCreateInfo.pCode    = (const u32 *)ShaderByteCode.Data;

        VkResult Result = vkCreateShaderModule(LogicalDevice, &ShaderModuleCreateInfo, NULL, Module);
        ValidateVkResult(Result, "vkCreateShaderModule", "failed to create shader module");

        // Create CreateInfo with Module.
        CreateInfo->sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        CreateInfo->stage               = Config->StageBit;
        CreateInfo->module              = *Module;
        CreateInfo->pName               = "main";
        CreateInfo->pSpecializationInfo = NULL;

        // Cleanup
        Free(&ShaderByteCode);
    }
    return ShaderStages;
}

void
WriteToHostCoherentBuffer(VkDevice LogicalDevice, buffer *Buffer, void *Data, VkDeviceSize Size, VkDeviceSize Offset)
{
    void *MappedMemory = NULL;
    VkResult Result = vkMapMemory(LogicalDevice, Buffer->Memory, Offset, Size, 0, &MappedMemory);
    ValidateVkResult(Result, "vkMapMemory", "failed to map host coherent buffer to host memory");
    memcpy(MappedMemory, Data, Size);
    vkUnmapMemory(LogicalDevice, Buffer->Memory);
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

} // vtk
