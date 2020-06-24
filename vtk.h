#pragma once

#include <vulkan/vulkan.h>
#include "ctk/ctk.h"

////////////////////////////////////////////////////////////
/// Macros
////////////////////////////////////////////////////////////
#define VTK_UNSET_INDEX CTK_U32_MAX
#define VTK_VK_RESULT_NAME(VK_RESULT) VK_RESULT, #VK_RESULT
#define VTK_LOAD_INSTANCE_EXTENSION_FUNCTION(INSTANCE, FUNCTION_NAME) \
    auto FUNCTION_NAME = (PFN_ ## FUNCTION_NAME)vkGetInstanceProcAddr(INSTANCE, #FUNCTION_NAME); \
    if(FUNCTION_NAME == NULL) \
    { \
        CTK_FATAL("failed to load instance extension function \"%s\"", #FUNCTION_NAME) \
    }

namespace vtk {

////////////////////////////////////////////////////////////
/// Data
////////////////////////////////////////////////////////////
struct instance_config
{
    ctk::sarray<cstr, 8> Layers;
    ctk::sarray<cstr, 8> Extensions;
    b32 Debug;
    cstr AppName;
};

struct instance
{
    VkInstance Handle;
    VkDebugUtilsMessengerEXT DebugUtilsMessenger;
};

struct device_config
{
    ctk::sarray<cstr, 8> Extensions;
    VkPhysicalDeviceFeatures Features;
};

struct queue_family_indexes
{
    u32 Graphics = VTK_UNSET_INDEX;
    u32 Present = VTK_UNSET_INDEX;
};

struct device
{
    VkPhysicalDevice Physical;
    VkPhysicalDeviceMemoryProperties MemoryProperties;
    queue_family_indexes QueueFamilyIndexes;
    VkSurfaceCapabilitiesKHR SurfaceCapabilities;
    ctk::array<VkSurfaceFormatKHR> SurfaceFormats;
    ctk::array<VkPresentModeKHR> SurfacePresentModes;

    VkDevice Logical;
    VkQueue GraphicsQueue;
    VkQueue PresentQueue;
};

struct swapchain_image
{
    VkImage Handle;
    VkImageView View;
};

struct swapchain
{
    VkSwapchainKHR Handle;
    ctk::sarray<swapchain_image, 4> Images;
    VkFormat ImageFormat;
    VkExtent2D Extent;
};

struct buffer_config
{
    VkDeviceSize Size;
    VkBufferUsageFlags UsageFlags;
    VkMemoryPropertyFlags MemoryPropertyFlags;
};

struct buffer
{
    VkBuffer Handle;
    VkDeviceMemory Memory;
    VkDeviceSize Size;
    VkDeviceSize End;
};

struct region
{
    buffer *Buffer;
    VkDeviceSize Size;
    VkDeviceSize Offset;
};

struct image_config
{
    u32 Width;
    u32 Height;
    VkFormat Format;
    VkImageTiling Tiling;
    VkImageUsageFlags UsageFlags;
    VkMemoryPropertyFlags MemoryPropertyFlags;
    VkImageAspectFlags AspectMask;
};

struct image
{
    VkImage Handle;
    VkDeviceMemory Memory;
    VkDeviceSize Width;
    VkDeviceSize Height;
    VkFormat Format;
    VkImageView View;
    VkSampler Sampler;
};

struct attachment
{
    VkAttachmentDescription Description;
    VkClearValue ClearValue;
};

struct subpass
{
    ctk::sarray<VkAttachmentReference, 4> ColorAttachmentReferences;
    ctk::optional<VkAttachmentReference> DepthAttachmentReference;
};

struct render_pass_config
{
    ctk::sarray<attachment, 4> Attachments;
    ctk::sarray<subpass, 4> Subpasses;
};

struct render_pass
{
    VkRenderPass Handle;
    ctk::sarray<VkClearValue, 4> ClearValues;
};

struct framebuffer_config
{
    ctk::sarray<VkImageView, 4> Attachments;
    VkExtent2D Extent;
    u32 Layers;
};

struct shader_module
{
    VkShaderModule Handle;
    VkShaderStageFlagBits StageBit;
};

struct vertex_attribute
{
    VkFormat Format;
    u32 Size;
    u32 Offset;
};

struct vertex_layout
{
    ctk::sarray<vertex_attribute, 4> Attributes;
    u32 Size;
};

struct vertex_input
{
    u32 Location;
    u32 Binding;
    u32 AttributeIndex;
};

struct descriptor_pool_config
{
    ctk::sarray<VkDescriptorPoolSize, 12> Sizes;
    u32 MaxSets;
};

struct graphics_pipeline_config
{
    ctk::sarray<shader_module *, 4> ShaderModules;
    ctk::sarray<vertex_input, 4> VertexInputs;
    vertex_layout *VertexLayout;
    ctk::sarray<VkDescriptorSetLayout, 4> DescriptorSetLayouts;
    VkExtent2D ViewportExtent;
    VkPrimitiveTopology PrimitiveTopology;
    VkBool32 DepthTesting;
};

struct graphics_pipeline
{
    VkPipeline Handle;
    VkPipelineLayout Layout;
};

struct frame
{
    VkSemaphore ImageAquiredSemaphore;
    VkSemaphore RenderFinishedSemaphore;
    VkFence InFlightFence;
};

struct frame_state
{
    ctk::sarray<frame, 4> Frames;
    ctk::sarray<VkFence, 4> PreviousFrameInFlightFences;
    u32 CurrentFrameIndex;
};

struct vk_result_debug_info
{
    VkResult Result;
    cstr ResultName;
    cstr Message;
};

struct device_info
{
    VkPhysicalDeviceProperties Properties;
    VkPhysicalDeviceFeatures Features;
    VkPhysicalDeviceMemoryProperties MemoryProperties;
    VkSurfaceCapabilitiesKHR SurfaceCapabilities;
    ctk::array<VkExtensionProperties> Extensions;
    ctk::array<VkSurfaceFormatKHR> SurfaceFormats;
    ctk::array<VkPresentModeKHR> SurfacePresentModes;
    ctk::array<VkQueueFamilyProperties> QueueFamilies;
    queue_family_indexes QueueFamilyIndexes;
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
        { VTK_VK_RESULT_NAME(VK_SUCCESS), "VULKAN SPEC ERROR MESSAGE: Command successfully completed." },
        { VTK_VK_RESULT_NAME(VK_NOT_READY), "VULKAN SPEC ERROR MESSAGE: A fence or query has not yet completed." },
        { VTK_VK_RESULT_NAME(VK_TIMEOUT), "VULKAN SPEC ERROR MESSAGE: A wait operation has not completed in the specified time." },
        { VTK_VK_RESULT_NAME(VK_EVENT_SET), "VULKAN SPEC ERROR MESSAGE: An event is signaled." },
        { VTK_VK_RESULT_NAME(VK_EVENT_RESET), "VULKAN SPEC ERROR MESSAGE: An event is unsignaled." },
        { VTK_VK_RESULT_NAME(VK_INCOMPLETE), "VULKAN SPEC ERROR MESSAGE: A return array was too small for the result." },
        { VTK_VK_RESULT_NAME(VK_SUBOPTIMAL_KHR), "VULKAN SPEC ERROR MESSAGE: A swapchain no longer matches the surface properties exactly, but can still be used to present to the surface successfully." },
        { VTK_VK_RESULT_NAME(VK_ERROR_OUT_OF_HOST_MEMORY), "VULKAN SPEC ERROR MESSAGE: A host memory allocation has failed." },
        { VTK_VK_RESULT_NAME(VK_ERROR_OUT_OF_DEVICE_MEMORY), "VULKAN SPEC ERROR MESSAGE: A device memory allocation has failed." },
        { VTK_VK_RESULT_NAME(VK_ERROR_INITIALIZATION_FAILED), "VULKAN SPEC ERROR MESSAGE: Initialization of an object could not be completed for implementation-specific reasons." },
        { VTK_VK_RESULT_NAME(VK_ERROR_DEVICE_LOST), "VULKAN SPEC ERROR MESSAGE: The logical or physical device has been lost." },
        { VTK_VK_RESULT_NAME(VK_ERROR_MEMORY_MAP_FAILED), "VULKAN SPEC ERROR MESSAGE: Mapping of a memory object has failed." },
        { VTK_VK_RESULT_NAME(VK_ERROR_LAYER_NOT_PRESENT), "VULKAN SPEC ERROR MESSAGE: A requested layer is not present or could not be loaded." },
        { VTK_VK_RESULT_NAME(VK_ERROR_EXTENSION_NOT_PRESENT), "VULKAN SPEC ERROR MESSAGE: A requested extension is not supported." },
        { VTK_VK_RESULT_NAME(VK_ERROR_FEATURE_NOT_PRESENT), "VULKAN SPEC ERROR MESSAGE: A requested feature is not supported." },
        { VTK_VK_RESULT_NAME(VK_ERROR_INCOMPATIBLE_DRIVER), "VULKAN SPEC ERROR MESSAGE: The requested version of Vulkan is not supported by the driver or is otherwise incompatible for implementation-specific reasons." },
        { VTK_VK_RESULT_NAME(VK_ERROR_TOO_MANY_OBJECTS), "VULKAN SPEC ERROR MESSAGE: Too many objects of the type have already been created." },
        { VTK_VK_RESULT_NAME(VK_ERROR_FORMAT_NOT_SUPPORTED), "VULKAN SPEC ERROR MESSAGE: A requested format is not supported on this device." },
        { VTK_VK_RESULT_NAME(VK_ERROR_FRAGMENTED_POOL), "VULKAN SPEC ERROR MESSAGE: A pool allocation has failed due to fragmentation of the pool’s memory. This must only be returned if no attempt to allocate host or device memory was made to accommodate the new allocation. This should be returned in preference to VK_ERROR_OUT_OF_POOL_MEMORY, but only if the implementation is certain that the pool allocation failure was due to fragmentation." },
        { VTK_VK_RESULT_NAME(VK_ERROR_SURFACE_LOST_KHR), "VULKAN SPEC ERROR MESSAGE: A surface is no longer available." },
        { VTK_VK_RESULT_NAME(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR), "VULKAN SPEC ERROR MESSAGE: The requested window is already in use by Vulkan or another API in a manner which prevents it from being used again." },
        { VTK_VK_RESULT_NAME(VK_ERROR_OUT_OF_DATE_KHR), "VULKAN SPEC ERROR MESSAGE: A surface has changed in such a way that it is no longer compatible with the swapchain, and further presentation requests using the swapchain will fail. Applications must query the new surface properties and recreate their swapchain if they wish to continue presenting to the surface." },
        { VTK_VK_RESULT_NAME(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR), "VULKAN SPEC ERROR MESSAGE: The display used by a swapchain does not use the same presentable image layout, or is incompatible in a way that prevents sharing an image." },
        { VTK_VK_RESULT_NAME(VK_ERROR_INVALID_SHADER_NV), "VULKAN SPEC ERROR MESSAGE: One or more shaders failed to compile or link. More details are reported back to the application via https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html#VK_EXT_debug_report if enabled." },
        { VTK_VK_RESULT_NAME(VK_ERROR_OUT_OF_POOL_MEMORY), "VULKAN SPEC ERROR MESSAGE: A pool memory allocation has failed. This must only be returned if no attempt to allocate host or device memory was made to accommodate the new allocation. If the failure was definitely due to fragmentation of the pool, VK_ERROR_FRAGMENTED_POOL should be returned instead." },
        { VTK_VK_RESULT_NAME(VK_ERROR_INVALID_EXTERNAL_HANDLE), "VULKAN SPEC ERROR MESSAGE: An external handle is not a valid handle of the specified type." },
        // { VTK_VK_RESULT_NAME(VK_ERROR_FRAGMENTATION), "VULKAN SPEC ERROR MESSAGE: A descriptor pool creation has failed due to fragmentation." },
        { VTK_VK_RESULT_NAME(VK_ERROR_INVALID_DEVICE_ADDRESS_EXT), "VULKAN SPEC ERROR MESSAGE: A buffer creation failed because the requested address is not available." },
        // { VTK_VK_RESULT_NAME(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS), "VULKAN SPEC ERROR MESSAGE: A buffer creation or memory allocation failed because the requested address is not available." },
        { VTK_VK_RESULT_NAME(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT), "VULKAN SPEC ERROR MESSAGE: An operation on a swapchain created with VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT failed as it did not have exlusive full-screen access. This may occur due to implementation-dependent reasons, outside of the application’s control." },
        // { VTK_VK_RESULT_NAME(VK_ERROR_UNKNOWN), "VULKAN SPEC ERROR MESSAGE: An unknown error has occurred; either the application has provided invalid input, or an implementation failure has occurred." },
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
AddOnsSupported(ctk::sarray<cstr, size> *AddOnNames, ctk::array<properties> *SupportedAddOns, name_selector NameSelector)
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
AddOnsSupported(ctk::sarray<cstr, size> *AddOnNames, name_selector NameSelector, loader Loader, args... Args)
{
    auto SupportedAddOns = LoadVkObjects<properties>(Loader, Args...);
    b32 AllSupported = AddOnsSupported<properties>(AddOnNames, &SupportedAddOns, NameSelector);
    ctk::Free(&SupportedAddOns);
    return AllSupported;
}

static void
ValidateVkResult(VkResult Result, cstr FunctionName, cstr FailureMessage)
{
    if(Result != VK_SUCCESS)
    {
        OutputVkResult(Result, FunctionName);
        CTK_FATAL(FailureMessage)
    }
}

template<u32 size>
static VkInstance
CreateVulkanInstance(cstr AppName, ctk::sarray<cstr, size> *Extensions, ctk::sarray<cstr, size> *Layers,
                     VkDebugUtilsMessengerCreateInfoEXT *DebugUtilsMessengerCreateInfo)
{
    VkApplicationInfo AppInfo = {};
    AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    AppInfo.pNext = NULL;
    AppInfo.pApplicationName = AppName;
    AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    AppInfo.pEngineName = AppName;
    AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    AppInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo InstanceCreateInfo = {};
    InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    InstanceCreateInfo.pNext = DebugUtilsMessengerCreateInfo;
    InstanceCreateInfo.flags = 0;
    InstanceCreateInfo.pApplicationInfo = &AppInfo;
    InstanceCreateInfo.enabledLayerCount = Layers->Count;
    InstanceCreateInfo.ppEnabledLayerNames = Layers->Data;
    InstanceCreateInfo.enabledExtensionCount = Extensions->Count;
    InstanceCreateInfo.ppEnabledExtensionNames = Extensions->Data;
    VkInstance Instance = VK_NULL_HANDLE;
    ValidateVkResult(vkCreateInstance(&InstanceCreateInfo, NULL, &Instance), "vkCreateInstance", "failed to create Vulkan instance");
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
    QueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    QueueCreateInfo.flags = 0;
    QueueCreateInfo.queueFamilyIndex = QueueFamilyIndex;
    QueueCreateInfo.queueCount = CTK_ARRAY_COUNT(QUEUE_PRIORITIES);
    QueueCreateInfo.pQueuePriorities = QUEUE_PRIORITIES;

    return QueueCreateInfo;
}

static VkImageView
CreateImageView(VkDevice LogicalDevice, VkImage Image, VkFormat Format, VkImageAspectFlags AspectFlags)
{
    VkImageViewCreateInfo ImageViewCreateInfo = {};
    ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ImageViewCreateInfo.image = Image;
    ImageViewCreateInfo.flags = 0;
    ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    ImageViewCreateInfo.format = Format;
    ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    ImageViewCreateInfo.subresourceRange.aspectMask = AspectFlags;
    ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    ImageViewCreateInfo.subresourceRange.levelCount = 1;
    ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    ImageViewCreateInfo.subresourceRange.layerCount = 1;
    VkImageView ImageView = VK_NULL_HANDLE;
    ValidateVkResult(vkCreateImageView(LogicalDevice, &ImageViewCreateInfo, NULL, &ImageView),
                     "vkCreateImageView", "failed to create image view");
    return ImageView;
}

static u32
FindMemoryTypeIndex(VkPhysicalDeviceMemoryProperties *MemoryProperties, u32 MemoryTypeBits, VkMemoryPropertyFlags MemoryPropertyFlags)
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
    CTK_FATAL("failed to find memory type index")
}

static VkSemaphore
CreateSemaphore(VkDevice LogicalDevice)
{
    VkSemaphoreCreateInfo SemaphoreCreateInfo = {};
    SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    SemaphoreCreateInfo.flags = 0;
    VkSemaphore Semaphore = VK_NULL_HANDLE;
    ValidateVkResult(vkCreateSemaphore(LogicalDevice, &SemaphoreCreateInfo, NULL, &Semaphore),
                     "vkCreateSemaphore", "failed to create semaphore");
    return Semaphore;
}

static VkFence
CreateFence(VkDevice LogicalDevice)
{
    VkFenceCreateInfo FenceCreateInfo = {};
    FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    FenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    VkFence Fence = VK_NULL_HANDLE;
    ValidateVkResult(vkCreateFence(LogicalDevice, &FenceCreateInfo, NULL, &Fence), "vkCreateFence", "failed to create fence");
    return Fence;
}

static void
AllocateCommandBuffers(VkDevice LogicalDevice, VkCommandPool CommandPool, u32 Count, VkCommandBuffer *CommandBuffers)
{
    VkCommandBufferAllocateInfo CommandBufferAllocateInfo = {};
    CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    CommandBufferAllocateInfo.commandPool = CommandPool;
    CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    CommandBufferAllocateInfo.commandBufferCount = Count;
    ValidateVkResult(vkAllocateCommandBuffers(LogicalDevice, &CommandBufferAllocateInfo, CommandBuffers),
                     "vkAllocateCommandBuffers", "failed to allocate command buffer");
}

static VkCommandBuffer
BeginOneTimeCommandBuffer(VkDevice LogicalDevice, VkCommandPool CommandPool)
{
    VkCommandBuffer CommandBuffer = {};
    AllocateCommandBuffers(LogicalDevice, CommandPool, 1, &CommandBuffer);

    VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
    CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    CommandBufferBeginInfo.pInheritanceInfo = NULL;

    vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo);
    return CommandBuffer;
}

static void
SubmitOneTimeCommandBuffer(VkDevice LogicalDevice, VkQueue Queue, VkCommandPool CommandPool, VkCommandBuffer CommandBuffer)
{
    vkEndCommandBuffer(CommandBuffer);

    VkSubmitInfo SubmitInfo = {};
    SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    SubmitInfo.commandBufferCount = 1;
    SubmitInfo.pCommandBuffers = &CommandBuffer;
    ValidateVkResult(vkQueueSubmit(Queue, 1, &SubmitInfo, VK_NULL_HANDLE),
                     "vkQueueSubmit", "failed to submit one-time command buffer to queue");
    vkQueueWaitIdle(Queue);

    // Cleanup
    vkFreeCommandBuffers(LogicalDevice, CommandPool, 1, &CommandBuffer);
}

////////////////////////////////////////////////////////////
/// Interface
////////////////////////////////////////////////////////////
static instance
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
        DebugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        DebugUtilsMessengerCreateInfo.pNext = NULL;
        DebugUtilsMessengerCreateInfo.flags = 0;
        DebugUtilsMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                                        // VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                                                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        DebugUtilsMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                                    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                                    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        DebugUtilsMessengerCreateInfo.pfnUserCallback = DebugCallback;
        DebugUtilsMessengerCreateInfo.pUserData = NULL;

        // Create Instance
        Instance.Handle = CreateVulkanInstance(Config->AppName, Extensions, Layers, &DebugUtilsMessengerCreateInfo);

        // Create Debug Utils Messenger
        VTK_LOAD_INSTANCE_EXTENSION_FUNCTION(Instance.Handle, vkCreateDebugUtilsMessengerEXT)
        ValidateVkResult(vkCreateDebugUtilsMessengerEXT(Instance.Handle, &DebugUtilsMessengerCreateInfo, NULL,
                                                        &Instance.DebugUtilsMessenger),
                         "vkCreateDebugUtilsMessengerEXT", "failed to create debug messenger");
    }
    else
    {
        Instance.Handle = CreateVulkanInstance(Config->AppName, Extensions, Layers, NULL);
    }
    return Instance;
}

static device
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
                SelectedDeviceInfo.QueueFamilyIndexes.Graphics = QueueFamilyIndex;
            }
            VkBool32 PresentationSupported = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, QueueFamilyIndex, PlatformSurface, &PresentationSupported);
            if(PresentationSupported == VK_TRUE)
            {
                SelectedDeviceInfo.QueueFamilyIndexes.Present = QueueFamilyIndex;
            }
        }

        ////////////////////////////////////////////////////////////
        /// Device Info Validation
        ////////////////////////////////////////////////////////////

        // Check if device has requested features.
        #define VTK_INTERNAL_DEVICE_FEATURE_CHECK(FEATURE) \
            if(Config->Features.FEATURE && !SelectedDeviceInfo.Features.FEATURE) \
            { \
                DeviceFeaturesSupported = false; \
                ctk::Error("requested device feature \"" #FEATURE "\" not supported"); \
            }

        b32 DeviceFeaturesSupported = true;
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(robustBufferAccess)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(fullDrawIndexUint32)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(imageCubeArray)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(independentBlend)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(geometryShader)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(tessellationShader)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(sampleRateShading)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(dualSrcBlend)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(logicOp)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(multiDrawIndirect)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(drawIndirectFirstInstance)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(depthClamp)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(depthBiasClamp)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(fillModeNonSolid)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(depthBounds)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(wideLines)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(largePoints)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(alphaToOne)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(multiViewport)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(samplerAnisotropy)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(textureCompressionETC2)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(textureCompressionASTC_LDR)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(textureCompressionBC)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(occlusionQueryPrecise)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(pipelineStatisticsQuery)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(vertexPipelineStoresAndAtomics)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(fragmentStoresAndAtomics)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(shaderTessellationAndGeometryPointSize)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(shaderImageGatherExtended)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(shaderStorageImageExtendedFormats)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(shaderStorageImageMultisample)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(shaderStorageImageReadWithoutFormat)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(shaderStorageImageWriteWithoutFormat)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(shaderUniformBufferArrayDynamicIndexing)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(shaderSampledImageArrayDynamicIndexing)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(shaderStorageBufferArrayDynamicIndexing)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(shaderStorageImageArrayDynamicIndexing)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(shaderClipDistance)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(shaderCullDistance)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(shaderFloat64)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(shaderInt64)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(shaderInt16)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(shaderResourceResidency)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(shaderResourceMinLod)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(sparseBinding)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(sparseResidencyBuffer)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(sparseResidencyImage2D)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(sparseResidencyImage3D)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(sparseResidency2Samples)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(sparseResidency4Samples)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(sparseResidency8Samples)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(sparseResidency16Samples)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(sparseResidencyAliased)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(variableMultisampleRate)
        VTK_INTERNAL_DEVICE_FEATURE_CHECK(inheritedQueries)

        // Check if physical device meets other requirements.
        b32 SwapchainsSupported = SelectedDeviceInfo.SurfaceFormats.Count != 0 &&
                                  SelectedDeviceInfo.SurfacePresentModes.Count != 0;
        if(SelectedDeviceInfo.Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
           SelectedDeviceInfo.QueueFamilyIndexes.Graphics != VTK_UNSET_INDEX &&
           SelectedDeviceInfo.QueueFamilyIndexes.Present != VTK_UNSET_INDEX &&
           AddOnsSupported<VkExtensionProperties>(Extensions, &SelectedDeviceInfo.Extensions, ExtensionName);
           DeviceFeaturesSupported &&
           SwapchainsSupported)
        {
            ctk::Info("physical device \"%s\" satisfies all requirements", SelectedDeviceInfo.Properties.deviceName);
            FoundSuitableDevice = true;

            // Initialize physical device with selected device info.
            Device.Physical = PhysicalDevice;
            Device.MemoryProperties = SelectedDeviceInfo.MemoryProperties;
            Device.QueueFamilyIndexes = SelectedDeviceInfo.QueueFamilyIndexes;
            Device.SurfaceCapabilities = SelectedDeviceInfo.SurfaceCapabilities;
            Device.SurfaceFormats = ctk::CreateArray(&SelectedDeviceInfo.SurfaceFormats);
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
    ctk::sarray<VkDeviceQueueCreateInfo, 2> QueueCreateInfos = {};
    Push(&QueueCreateInfos, CreateQueueCreateInfo(Device.QueueFamilyIndexes.Graphics));
    if(Device.QueueFamilyIndexes.Present != Device.QueueFamilyIndexes.Graphics)
    {
        Push(&QueueCreateInfos, CreateQueueCreateInfo(Device.QueueFamilyIndexes.Present));
    }

    VkDeviceCreateInfo LogicalDeviceCreateInfo = {};
    LogicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    LogicalDeviceCreateInfo.flags = 0;
    LogicalDeviceCreateInfo.queueCreateInfoCount = QueueCreateInfos.Count;
    LogicalDeviceCreateInfo.pQueueCreateInfos = QueueCreateInfos.Data;
    LogicalDeviceCreateInfo.enabledLayerCount = 0;
    LogicalDeviceCreateInfo.ppEnabledLayerNames = NULL;
    LogicalDeviceCreateInfo.enabledExtensionCount = Extensions->Count;
    LogicalDeviceCreateInfo.ppEnabledExtensionNames = Extensions->Data;
    LogicalDeviceCreateInfo.pEnabledFeatures = &Config->Features;
    ValidateVkResult(vkCreateDevice(Device.Physical, &LogicalDeviceCreateInfo, NULL, &Device.Logical),
                     "vkCreateDevice", "failed to create logical device");

    // Get logical device queues.
    static const u32 QUEUE_INDEX = 0; // Currently only supporting 1 queue per family.
    vkGetDeviceQueue(Device.Logical, Device.QueueFamilyIndexes.Graphics, QUEUE_INDEX, &Device.GraphicsQueue);
    vkGetDeviceQueue(Device.Logical, Device.QueueFamilyIndexes.Present, QUEUE_INDEX, &Device.PresentQueue);

    // Cleanup
    Free(&PhysicalDevices);

    return Device;
}

static swapchain
CreateSwapchain(device *Device, VkSurfaceKHR PlatformSurface)
{
    swapchain Swapchain = {};

    ////////////////////////////////////////////////////////////
    /// Configuration
    ////////////////////////////////////////////////////////////

    // Configure swapchain based on surface properties.

    // Default to first surface format.
    VkSurfaceFormatKHR SelectedFormat = Device->SurfaceFormats[0];
    for(u32 SurfaceFormatIndex = 0; SurfaceFormatIndex < Device->SurfaceFormats.Count; ++SurfaceFormatIndex)
    {
        VkSurfaceFormatKHR SurfaceFormat = Device->SurfaceFormats[SurfaceFormatIndex];

        // Prefer 4-component 8-bit BGRA unsigned normalized format and sRGB color space.
        if(SurfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM && SurfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            SelectedFormat = SurfaceFormat;
            break;
        }
    }

    // Default to FIFO (only present mode with guarenteed availability).
    VkPresentModeKHR SelectedPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    for(u32 SurfacePresentModeIndex = 0; SurfacePresentModeIndex < Device->SurfacePresentModes.Count; ++SurfacePresentModeIndex)
    {
        VkPresentModeKHR SurfacePresentMode = Device->SurfacePresentModes[SurfacePresentModeIndex];

        // Mailbox is the preferred present mode if available.
        if(SurfacePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            SelectedPresentMode = SurfacePresentMode;
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
    u32 GraphicsQueueFamilyIndex = Device->QueueFamilyIndexes.Graphics;
    u32 PresentQueueFamilyIndex = Device->QueueFamilyIndexes.Present;
    u32 QueueFamilyIndexes[] = { GraphicsQueueFamilyIndex, PresentQueueFamilyIndex };

    VkSwapchainCreateInfoKHR SwapchainCreateInfo = {};
    SwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    SwapchainCreateInfo.surface = PlatformSurface;
    SwapchainCreateInfo.flags = 0;
    SwapchainCreateInfo.minImageCount = SelectedImageCount;
    SwapchainCreateInfo.imageFormat = SelectedFormat.format;
    SwapchainCreateInfo.imageColorSpace = SelectedFormat.colorSpace;
    SwapchainCreateInfo.imageExtent = Device->SurfaceCapabilities.currentExtent;
    SwapchainCreateInfo.imageArrayLayers = 1; // Always 1 for standard images.
    SwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    SwapchainCreateInfo.preTransform = Device->SurfaceCapabilities.currentTransform;
    SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    SwapchainCreateInfo.presentMode = SelectedPresentMode;
    SwapchainCreateInfo.clipped = VK_TRUE;
    SwapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
    if(GraphicsQueueFamilyIndex != PresentQueueFamilyIndex)
    {
        SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        SwapchainCreateInfo.queueFamilyIndexCount = CTK_ARRAY_COUNT(QueueFamilyIndexes);
        SwapchainCreateInfo.pQueueFamilyIndices = QueueFamilyIndexes;
    }
    else
    {
        SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        SwapchainCreateInfo.queueFamilyIndexCount = 0;
        SwapchainCreateInfo.pQueueFamilyIndices = NULL;
    }
    ValidateVkResult(vkCreateSwapchainKHR(Device->Logical, &SwapchainCreateInfo, NULL, &Swapchain.Handle),
                     "vkCreateSwapchainKHR", "failed to create swapchain");

    ////////////////////////////////////////////////////////////
    /// Swapchain Image Creation
    ////////////////////////////////////////////////////////////
    auto Images = LoadVkObjects<VkImage>(vkGetSwapchainImagesKHR, Device->Logical, Swapchain.Handle);
    for(u32 ImageIndex = 0; ImageIndex < Images.Count; ++ImageIndex)
    {
        swapchain_image *SwapchainImage = ctk::Push(&Swapchain.Images);
        SwapchainImage->Handle = Images[ImageIndex];
        SwapchainImage->View = CreateImageView(Device->Logical, SwapchainImage->Handle, SelectedFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);
    }

    // Store surface state used to create swapchain for future reference.
    Swapchain.ImageFormat = SelectedFormat.format;
    Swapchain.Extent = Device->SurfaceCapabilities.currentExtent;

    // Cleanup
    Free(&Images);

    return Swapchain;
}

static VkCommandPool
CreateCommandPool(VkDevice LogicalDevice, u32 QueueFamilyIndex)
{
    VkCommandPoolCreateInfo CommandPoolCreateInfo = {};
    CommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    CommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    CommandPoolCreateInfo.queueFamilyIndex = QueueFamilyIndex;
    VkCommandPool CommandPool = VK_NULL_HANDLE;
    ValidateVkResult(vkCreateCommandPool(LogicalDevice, &CommandPoolCreateInfo, NULL, &CommandPool),
                     "vkCreateCommandPool", "failed to create command pool");
    return CommandPool;
}

static buffer
CreateBuffer(device *Device, buffer_config *Config)
{
    buffer Buffer = {};
    Buffer.Size = Config->Size;

    // Buffer Creation
    VkBufferCreateInfo BufferCreateInfo = {};
    BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    BufferCreateInfo.size = Config->Size;
    BufferCreateInfo.usage = Config->UsageFlags;
    BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    BufferCreateInfo.queueFamilyIndexCount = 0;
    BufferCreateInfo.pQueueFamilyIndices = NULL; // Ignored if sharingMode is not VK_SHARING_MODE_CONCURRENT.
    ValidateVkResult(vkCreateBuffer(Device->Logical, &BufferCreateInfo, NULL, &Buffer.Handle),
                     "vkCreateBuffer", "failed to create buffer");

    // Memory Allocation & Binding
    VkMemoryRequirements MemoryRequirements = {};
    vkGetBufferMemoryRequirements(Device->Logical, Buffer.Handle, &MemoryRequirements);
    VkMemoryAllocateInfo MemoryAllocateInfo = {};
    MemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    MemoryAllocateInfo.allocationSize = MemoryRequirements.size;
    MemoryAllocateInfo.memoryTypeIndex = FindMemoryTypeIndex(&Device->MemoryProperties, MemoryRequirements.memoryTypeBits,
                                                             Config->MemoryPropertyFlags);
    ValidateVkResult(vkAllocateMemory(Device->Logical, &MemoryAllocateInfo, NULL, &Buffer.Memory),
                     "vkAllocateMemory", "failed to allocate memory for buffer");
    ValidateVkResult(vkBindBufferMemory(Device->Logical, Buffer.Handle, Buffer.Memory, 0),
                     "vkBindBufferMemory", "failed to bind buffer memory");

    return Buffer;
}

static void
DestroyBuffer(VkDevice LogicalDevice, buffer *Buffer)
{
    vkDestroyBuffer(LogicalDevice, Buffer->Handle, NULL);
    vkFreeMemory(LogicalDevice, Buffer->Memory, NULL);
}

static region
AllocateRegion(buffer *Buffer, VkDeviceSize Size)
{
    if(Buffer->End + Size >= Buffer->Size)
    {
        CTK_FATAL("buffer (size=%u end=%u) does not have room to allocate region of size %u", Buffer->Size, Buffer->End, Size);
    }
    region Region = {};
    Region.Buffer = Buffer;
    Region.Offset = Buffer->End;
    Region.Size = Size;
    Buffer->End += Size;
    return Region;
}

static image
CreateImage(device *Device, image_config *Config)
{
    image Image = {};
    Image.Width = Config->Width;
    Image.Height = Config->Height;
    Image.Format = Config->Format;

    ////////////////////////////////////////////////////////////
    /// Vulkan Image
    ////////////////////////////////////////////////////////////
    VkImageCreateInfo ImageCreateInfo = {};
    ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    ImageCreateInfo.flags = 0;
    ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    ImageCreateInfo.format = Config->Format;
    ImageCreateInfo.extent.width = Config->Width;
    ImageCreateInfo.extent.height = Config->Height;
    ImageCreateInfo.extent.depth = 1;
    ImageCreateInfo.mipLevels = 1;
    ImageCreateInfo.arrayLayers = 1;
    ImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    ImageCreateInfo.tiling = Config->Tiling;
    ImageCreateInfo.usage = Config->UsageFlags;
    ImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    ImageCreateInfo.queueFamilyIndexCount = 0;
    ImageCreateInfo.pQueueFamilyIndices = NULL; // Ignored if sharingMode is not VK_SHARING_MODE_CONCURRENT.
    ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    ValidateVkResult(vkCreateImage(Device->Logical, &ImageCreateInfo, NULL, &Image.Handle), "vkCreateImage", "failed to create image");

    ////////////////////////////////////////////////////////////
    /// Memory Allocation
    ////////////////////////////////////////////////////////////
    VkMemoryRequirements MemoryRequirements = {};
    vkGetImageMemoryRequirements(Device->Logical, Image.Handle, &MemoryRequirements);

    VkMemoryAllocateInfo MemoryAllocateInfo = {};
    MemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    MemoryAllocateInfo.allocationSize = MemoryRequirements.size;
    MemoryAllocateInfo.memoryTypeIndex = FindMemoryTypeIndex(&Device->MemoryProperties, MemoryRequirements.memoryTypeBits,
                                                             Config->MemoryPropertyFlags);
    ValidateVkResult(vkAllocateMemory(Device->Logical, &MemoryAllocateInfo, NULL, &Image.Memory),
                     "vkAllocateMemory", "failed to allocate memory for image");
    ValidateVkResult(vkBindImageMemory(Device->Logical, Image.Handle, Image.Memory, 0),
                     "vkBindImageMemory", "failed to bind image memory");

    ////////////////////////////////////////////////////////////
    /// View
    ////////////////////////////////////////////////////////////
    Image.View = CreateImageView(Device->Logical, Image.Handle, Image.Format, Config->AspectMask);

    ////////////////////////////////////////////////////////////
    /// Sampler
    ////////////////////////////////////////////////////////////
    VkSamplerCreateInfo SamplerCreateInfo = {};
    SamplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    SamplerCreateInfo.magFilter = VK_FILTER_LINEAR;
    SamplerCreateInfo.minFilter = VK_FILTER_LINEAR;
    SamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    SamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    SamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    SamplerCreateInfo.anisotropyEnable = VK_TRUE;
    SamplerCreateInfo.maxAnisotropy = 16;
    SamplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    SamplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
    SamplerCreateInfo.compareEnable = VK_FALSE;
    SamplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    SamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    SamplerCreateInfo.mipLodBias = 0.0f;
    SamplerCreateInfo.minLod = 0.0f;
    SamplerCreateInfo.maxLod = 0.0f;
    ValidateVkResult(vkCreateSampler(Device->Logical, &SamplerCreateInfo, NULL, &Image.Sampler),
                     "vkCreateSampler", "failed to create sampler");

    return Image;
}

static render_pass
CreateRenderPass(VkDevice LogicalDevice, render_pass_config *Config)
{
    render_pass RenderPass = {};

    ////////////////////////////////////////////////////////////
    /// Attachments
    ////////////////////////////////////////////////////////////
    ctk::sarray<VkAttachmentDescription, 4> AttachmentDescriptions = {};
    for(u32 AttachmentIndex = 0; AttachmentIndex < Config->Attachments.Count; ++AttachmentIndex)
    {
        attachment *Attachment = Config->Attachments + AttachmentIndex;
        ctk::Push(&AttachmentDescriptions, Attachment->Description);

        // Store clear value if attachment uses a clear load operation.
        if(Attachment->Description.loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR)
        {
            ctk::Push(&RenderPass.ClearValues, Attachment->ClearValue);
        }
    }

    ////////////////////////////////////////////////////////////
    /// Subpasses
    ////////////////////////////////////////////////////////////
    ctk::sarray<VkSubpassDescription, 4> SubpassDescriptions = {};
    for(u32 SubpassIndex = 0; SubpassIndex < Config->Subpasses.Count; ++SubpassIndex)
    {
        subpass *Subpass = At(&Config->Subpasses, SubpassIndex);
        VkSubpassDescription *SubpassDescription = ctk::Push(&SubpassDescriptions);
        SubpassDescription->pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        SubpassDescription->inputAttachmentCount = 0;
        SubpassDescription->pInputAttachments = NULL;
        SubpassDescription->colorAttachmentCount = Subpass->ColorAttachmentReferences.Count;
        SubpassDescription->pColorAttachments = Subpass->ColorAttachmentReferences.Data;
        SubpassDescription->pResolveAttachments = NULL;
        SubpassDescription->pDepthStencilAttachment = Subpass->DepthAttachmentReference ? &Subpass->DepthAttachmentReference.Value : NULL;
        SubpassDescription->preserveAttachmentCount = 0;
        SubpassDescription->pPreserveAttachments = NULL;
    }

    VkSubpassDependency SubpassDependencies[1] = {};
    SubpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    SubpassDependencies[0].dstSubpass = 0;
    SubpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    SubpassDependencies[0].srcAccessMask = 0;
    SubpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    SubpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    ////////////////////////////////////////////////////////////
    /// Render Pass
    ////////////////////////////////////////////////////////////
    VkRenderPassCreateInfo RenderPassCreateInfo = {};
    RenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    RenderPassCreateInfo.attachmentCount = AttachmentDescriptions.Count;
    RenderPassCreateInfo.pAttachments = AttachmentDescriptions.Data;
    RenderPassCreateInfo.subpassCount = SubpassDescriptions.Count;
    RenderPassCreateInfo.pSubpasses = SubpassDescriptions.Data;
    RenderPassCreateInfo.dependencyCount = CTK_ARRAY_COUNT(SubpassDependencies);
    RenderPassCreateInfo.pDependencies = SubpassDependencies;
    ValidateVkResult(vkCreateRenderPass(LogicalDevice, &RenderPassCreateInfo, NULL, &RenderPass.Handle),
                     "vkCreateRenderPass", "failed to create render pass");

    return RenderPass;
}

static VkFramebuffer
CreateFramebuffer(VkDevice LogicalDevice, VkRenderPass RenderPass, framebuffer_config *Config)
{
    VkFramebufferCreateInfo FramebufferCreateInfo = {};
    FramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    FramebufferCreateInfo.renderPass = RenderPass;
    FramebufferCreateInfo.attachmentCount = Config->Attachments.Count;
    FramebufferCreateInfo.pAttachments = Config->Attachments.Data;
    FramebufferCreateInfo.width = Config->Extent.width;
    FramebufferCreateInfo.height = Config->Extent.height;
    FramebufferCreateInfo.layers = Config->Layers;
    VkFramebuffer Framebuffer = VK_NULL_HANDLE;
    ValidateVkResult(vkCreateFramebuffer(LogicalDevice, &FramebufferCreateInfo, NULL, &Framebuffer),
                     "vkCreateFramebuffer", "failed to create framebuffer");
    return Framebuffer;
}

static shader_module
CreateShaderModule(VkDevice LogicalDevice, cstr Path, VkShaderStageFlagBits StageBit)
{
    shader_module ShaderModule = {};
    ShaderModule.StageBit = StageBit;
    ctk::array<u8> ShaderByteCode = ctk::ReadFile<u8>(Path);

    VkShaderModuleCreateInfo ShaderModuleCreateInfo = {};
    ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ShaderModuleCreateInfo.flags = 0;
    ShaderModuleCreateInfo.codeSize = ByteSize(&ShaderByteCode);
    ShaderModuleCreateInfo.pCode = (const u32 *)ShaderByteCode.Data;
    ValidateVkResult(vkCreateShaderModule(LogicalDevice, &ShaderModuleCreateInfo, NULL, &ShaderModule.Handle),
                     "vkCreateShaderModule", "failed to create shader module");

    // Cleanup
    Free(&ShaderByteCode);

    return ShaderModule;
}

static u32
PushVertexAttribute(vertex_layout *VertexLayout, u32 ElementCount)
{
    static VkFormat FORMATS[] =
    {
        VK_FORMAT_R32_SFLOAT,
        VK_FORMAT_R32G32_SFLOAT,
        VK_FORMAT_R32G32B32_SFLOAT,
        VK_FORMAT_R32G32B32A32_SFLOAT,
    };
    CTK_ASSERT(ElementCount <= CTK_ARRAY_COUNT(FORMATS));
    u32 AttributeSize = sizeof(f32) * ElementCount;
    u32 AttributeIndex = VertexLayout->Attributes.Count;
    ctk::Push(&VertexLayout->Attributes, { FORMATS[ElementCount - 1], AttributeSize, VertexLayout->Size });
    VertexLayout->Size += AttributeSize;
    return AttributeIndex;
}

static VkDescriptorPool
CreateDescriptorPool(VkDevice LogicalDevice, descriptor_pool_config *Config)
{
    VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo = {};
    DescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    DescriptorPoolCreateInfo.flags = 0;
    DescriptorPoolCreateInfo.maxSets = Config->MaxSets;
    DescriptorPoolCreateInfo.poolSizeCount = Config->Sizes.Count;
    DescriptorPoolCreateInfo.pPoolSizes = Config->Sizes.Data;
    VkDescriptorPool DescriptorPool = VK_NULL_HANDLE;
    ValidateVkResult(vkCreateDescriptorPool(LogicalDevice, &DescriptorPoolCreateInfo, NULL, &DescriptorPool),
                     "vkCreateDescriptorPool", "failed to create descriptor pool");
    return DescriptorPool;
}

static VkDescriptorSetLayout
CreateDescriptorSetLayout(VkDevice LogicalDevice, ctk::sarray<VkDescriptorSetLayoutBinding, 4> *DescriptorSetLayoutBindings)
{
    VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo = {};
    DescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    DescriptorSetLayoutCreateInfo.bindingCount = DescriptorSetLayoutBindings->Count;
    DescriptorSetLayoutCreateInfo.pBindings = DescriptorSetLayoutBindings->Data;
    VkDescriptorSetLayout DescriptorSetLayout = VK_NULL_HANDLE;
    ValidateVkResult(vkCreateDescriptorSetLayout(LogicalDevice, &DescriptorSetLayoutCreateInfo, NULL, &DescriptorSetLayout),
                     "vkCreateDescriptorSetLayout", "error creating descriptor set layout");
    return DescriptorSetLayout;
}

static void
AllocateDescriptorSets(VkDevice LogicalDevice, VkDescriptorPool DescriptorPool, ctk::sarray<VkDescriptorSetLayout, 4> *DescriptorSetLayouts,
                       ctk::sarray<VkDescriptorSet, 4> *DescriptorSets)
{
    VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo = {};
    DescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    DescriptorSetAllocateInfo.descriptorPool = DescriptorPool;
    DescriptorSetAllocateInfo.descriptorSetCount = DescriptorSetLayouts->Count;
    DescriptorSetAllocateInfo.pSetLayouts = DescriptorSetLayouts->Data;
    ValidateVkResult(vkAllocateDescriptorSets(LogicalDevice, &DescriptorSetAllocateInfo, DescriptorSets->Data),
                     "vkAllocateDescriptorSets", "failed to allocate descriptor sets");
    DescriptorSets->Count = DescriptorSetLayouts->Count;
}

static graphics_pipeline
CreateGraphicsPipeline(VkDevice LogicalDevice, VkRenderPass RenderPass, graphics_pipeline_config *Config)
{
    graphics_pipeline GraphicsPipeline = {};

    ////////////////////////////////////////////////////////////
    /// Shader Stages
    ////////////////////////////////////////////////////////////
    ctk::sarray<VkPipelineShaderStageCreateInfo, 4> ShaderStages = {};
    for(u32 ShaderModuleIndex = 0; ShaderModuleIndex < Config->ShaderModules.Count; ++ShaderModuleIndex)
    {
        shader_module *ShaderModule = Config->ShaderModules[ShaderModuleIndex];

        VkPipelineShaderStageCreateInfo *ShaderStageCreateInfo = ctk::Push(&ShaderStages);
        ShaderStageCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ShaderStageCreateInfo->flags = 0;
        ShaderStageCreateInfo->stage = ShaderModule->StageBit;
        ShaderStageCreateInfo->module = ShaderModule->Handle;
        ShaderStageCreateInfo->pName = "main";
        ShaderStageCreateInfo->pSpecializationInfo = NULL;
    }

    ////////////////////////////////////////////////////////////
    /// Vertex Input State
    ////////////////////////////////////////////////////////////
    ctk::sarray<VkVertexInputAttributeDescription, 4> VertexAttributeDescriptions = {};
    for(u32 InputIndex = 0; InputIndex < Config->VertexInputs.Count; ++InputIndex)
    {
        vertex_input *VertexInput = At(&Config->VertexInputs, InputIndex);
        vertex_attribute *VertexAttribute = Config->VertexLayout->Attributes + VertexInput->AttributeIndex;

        VkVertexInputAttributeDescription *AttributeDescription = ctk::Push(&VertexAttributeDescriptions);
        AttributeDescription->location = VertexInput->Location;
        AttributeDescription->binding = VertexInput->Binding;
        AttributeDescription->format = VertexAttribute->Format;
        AttributeDescription->offset = VertexAttribute->Offset;
    }

    ctk::sarray<VkVertexInputBindingDescription, 4> VertexBindingDescriptions = {};
    VkVertexInputBindingDescription *BindingDescription = ctk::Push(&VertexBindingDescriptions);
    BindingDescription->binding = 0;
    BindingDescription->stride = Config->VertexLayout->Size;
    BindingDescription->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkPipelineVertexInputStateCreateInfo VertexInputState = {};
    VertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    VertexInputState.vertexBindingDescriptionCount = VertexBindingDescriptions.Count;
    VertexInputState.pVertexBindingDescriptions = VertexBindingDescriptions.Data;
    VertexInputState.vertexAttributeDescriptionCount = VertexAttributeDescriptions.Count;
    VertexInputState.pVertexAttributeDescriptions = VertexAttributeDescriptions.Data;

    ////////////////////////////////////////////////////////////
    /// Input Assembly State
    ////////////////////////////////////////////////////////////
    VkPipelineInputAssemblyStateCreateInfo InputAssemblyState = {};
    InputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    InputAssemblyState.topology = Config->PrimitiveTopology;
    InputAssemblyState.primitiveRestartEnable = VK_FALSE;

    ////////////////////////////////////////////////////////////
    /// Viewport State
    ////////////////////////////////////////////////////////////
    VkViewport Viewport = {};
    Viewport.x = 0.0f;
    Viewport.y = 0.0f;
    Viewport.width = (f32)Config->ViewportExtent.width;
    Viewport.height = (f32)Config->ViewportExtent.height;
    Viewport.minDepth = 0.0f;
    Viewport.maxDepth = 1.0f;

    // Make scissor fill viewport.
    VkRect2D Scissor = {};
    Scissor.offset.x = 0;
    Scissor.offset.y = 0;
    Scissor.extent.width = Viewport.width;
    Scissor.extent.height = Viewport.height;

    VkPipelineViewportStateCreateInfo ViewportState = {};
    ViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    ViewportState.viewportCount = 1;
    ViewportState.pViewports = &Viewport;
    ViewportState.scissorCount = 1;
    ViewportState.pScissors = &Scissor;

    ////////////////////////////////////////////////////////////
    /// Depth Stencil State
    ////////////////////////////////////////////////////////////
    VkPipelineDepthStencilStateCreateInfo DepthStencilState = {};
    DepthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    DepthStencilState.depthTestEnable = Config->DepthTesting;
    DepthStencilState.depthWriteEnable = Config->DepthTesting;
    DepthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
    DepthStencilState.depthBoundsTestEnable = VK_FALSE;
    DepthStencilState.stencilTestEnable = VK_FALSE;
    DepthStencilState.front = {};
    DepthStencilState.back = {};
    DepthStencilState.minDepthBounds = 0.0f;
    DepthStencilState.maxDepthBounds = 1.0f;

    ////////////////////////////////////////////////////////////
    /// Default State
    ////////////////////////////////////////////////////////////
    VkPipelineRasterizationStateCreateInfo RasterizationState = {};
    RasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    RasterizationState.depthClampEnable = VK_FALSE; // Don't clamp fragments within depth range.
    RasterizationState.rasterizerDiscardEnable = VK_FALSE;
    RasterizationState.polygonMode = VK_POLYGON_MODE_FILL; // Only available mode on AMD gpus?
    RasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
    RasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    RasterizationState.depthBiasEnable = VK_FALSE;
    RasterizationState.depthBiasConstantFactor = 0.0f;
    RasterizationState.depthBiasClamp = 0.0f;
    RasterizationState.depthBiasSlopeFactor = 0.0f;
    RasterizationState.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo MultisampleState = {};
    MultisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    MultisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    MultisampleState.sampleShadingEnable = VK_FALSE;
    MultisampleState.minSampleShading = 1.0f;
    MultisampleState.pSampleMask = NULL;
    MultisampleState.alphaToCoverageEnable = VK_FALSE;
    MultisampleState.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState ColorBlendAttachmentState = {};
    ColorBlendAttachmentState.blendEnable = VK_FALSE;
    ColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    ColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    ColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    ColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    ColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    ColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
    ColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                               VK_COLOR_COMPONENT_G_BIT |
                                               VK_COLOR_COMPONENT_B_BIT |
                                               VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo ColorBlendState = {};
    ColorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    ColorBlendState.logicOpEnable = VK_FALSE;
    ColorBlendState.logicOp = VK_LOGIC_OP_COPY;
    ColorBlendState.attachmentCount = 1;
    ColorBlendState.pAttachments = &ColorBlendAttachmentState;
    ColorBlendState.blendConstants[0] = 0.0f;
    ColorBlendState.blendConstants[1] = 0.0f;
    ColorBlendState.blendConstants[2] = 0.0f;
    ColorBlendState.blendConstants[3] = 0.0f;

    ////////////////////////////////////////////////////////////
    /// Pipeline Layout
    ////////////////////////////////////////////////////////////
    VkPipelineLayoutCreateInfo LayoutCreateInfo = {};
    LayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    LayoutCreateInfo.setLayoutCount = Config->DescriptorSetLayouts.Count;
    LayoutCreateInfo.pSetLayouts = Config->DescriptorSetLayouts.Data;
    LayoutCreateInfo.pushConstantRangeCount = 0;
    LayoutCreateInfo.pPushConstantRanges = NULL;
    ValidateVkResult(vkCreatePipelineLayout(LogicalDevice, &LayoutCreateInfo, NULL, &GraphicsPipeline.Layout),
                     "vkCreatePipelineLayout", "failed to create graphics pipeline layout");

    ////////////////////////////////////////////////////////////
    /// Graphics Pipeline
    ////////////////////////////////////////////////////////////
    VkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo = {};
    GraphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    GraphicsPipelineCreateInfo.stageCount = ShaderStages.Count;
    GraphicsPipelineCreateInfo.pStages = ShaderStages.Data;
    GraphicsPipelineCreateInfo.pVertexInputState = &VertexInputState;
    GraphicsPipelineCreateInfo.pInputAssemblyState = &InputAssemblyState;
    GraphicsPipelineCreateInfo.pTessellationState = NULL;
    GraphicsPipelineCreateInfo.pViewportState = &ViewportState;
    GraphicsPipelineCreateInfo.pRasterizationState = &RasterizationState;
    GraphicsPipelineCreateInfo.pMultisampleState = &MultisampleState;
    GraphicsPipelineCreateInfo.pDepthStencilState = &DepthStencilState;
    GraphicsPipelineCreateInfo.pColorBlendState = &ColorBlendState;
    GraphicsPipelineCreateInfo.pDynamicState = NULL;
    GraphicsPipelineCreateInfo.layout = GraphicsPipeline.Layout;
    GraphicsPipelineCreateInfo.renderPass = RenderPass;
    GraphicsPipelineCreateInfo.subpass = 0;
    GraphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    GraphicsPipelineCreateInfo.basePipelineIndex = -1;
    ValidateVkResult(vkCreateGraphicsPipelines(LogicalDevice,
                                               VK_NULL_HANDLE, // Pipeline Cache
                                               1,
                                               &GraphicsPipelineCreateInfo,
                                               NULL, // Allocation Callbacks
                                               &GraphicsPipeline.Handle),
                     "vkCreateGraphicsPipelines", "failed to create graphics pipeline");

    return GraphicsPipeline;
}

static frame_state
CreateFrameState(VkDevice LogicalDevice, u32 FrameCount, u32 SwapchainImageCount)
{
    frame_state FrameState = {};
    for(u32 _ = 0; _ < FrameCount; ++_)
    {
        frame *Frame = ctk::Push(&FrameState.Frames);
        Frame->ImageAquiredSemaphore = CreateSemaphore(LogicalDevice);
        Frame->RenderFinishedSemaphore = CreateSemaphore(LogicalDevice);
        Frame->InFlightFence = CreateFence(LogicalDevice);
    }
    for(u32 _ = 0; _ < SwapchainImageCount; ++_)
    {
        ctk::Push(&FrameState.PreviousFrameInFlightFences, (VkFence)VK_NULL_HANDLE);
    }
    return FrameState;
}

static void
WriteToHostRegion(VkDevice LogicalDevice, region *Region, void *Data, VkDeviceSize Size, VkDeviceSize OffsetIntoRegion)
{
    if(OffsetIntoRegion + Size > Region->Size)
    {
        CTK_FATAL("cannot write %u bytes at offset %u into region (size=%u)", Size, OffsetIntoRegion, Region->Size);
    }
    void *MappedMemory = NULL;
    ValidateVkResult(vkMapMemory(LogicalDevice, Region->Buffer->Memory, Region->Offset + OffsetIntoRegion, Size, 0, &MappedMemory),
                     "vkMapMemory", "failed to map host coherent buffer to host memory");
    memcpy(MappedMemory, Data, Size);
    vkUnmapMemory(LogicalDevice, Region->Buffer->Memory);
}

static void
WriteToDeviceRegion(device *Device, VkCommandPool CommandPool, region *StagingRegion, region *Region,
                   void *Data, VkDeviceSize Size, VkDeviceSize OffsetIntoRegion)
{
    if(OffsetIntoRegion + Size > Region->Size)
    {
        CTK_FATAL("cannot write %u bytes at offset %u into region (size=%u)", Size, OffsetIntoRegion, Region->Size);
    }
    WriteToHostRegion(Device->Logical, StagingRegion, Data, Size, 0);
    VkBufferCopy BufferCopyRegions[1] = {};
    BufferCopyRegions[0].srcOffset = StagingRegion->Offset;
    BufferCopyRegions[0].dstOffset = Region->Offset + OffsetIntoRegion;
    BufferCopyRegions[0].size = Size;
    VkCommandBuffer CommandBuffer = BeginOneTimeCommandBuffer(Device->Logical, CommandPool);
        vkCmdCopyBuffer(CommandBuffer, StagingRegion->Buffer->Handle, Region->Buffer->Handle,
                        CTK_ARRAY_COUNT(BufferCopyRegions), BufferCopyRegions);
    SubmitOneTimeCommandBuffer(Device->Logical, Device->GraphicsQueue, CommandPool, CommandBuffer);
}

static VkFormat
FindDepthImageFormat(VkPhysicalDevice PhysicalDevice)
{
    static const VkFormat DEPTH_IMAGE_FORMATS[] =
    {
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
    };
    static const VkFormatFeatureFlags DEPTH_IMG_FORMAT_FEATURES = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    for(u32 DepthImageFormatIndex = 0; DepthImageFormatIndex < CTK_ARRAY_COUNT(DEPTH_IMAGE_FORMATS); DepthImageFormatIndex++)
    {
        VkFormat DepthImageFormat = DEPTH_IMAGE_FORMATS[DepthImageFormatIndex];
        VkFormatProperties DepthImageFormatProperties = {};
        vkGetPhysicalDeviceFormatProperties(PhysicalDevice, DepthImageFormat, &DepthImageFormatProperties);
        if((DepthImageFormatProperties.optimalTilingFeatures & DEPTH_IMG_FORMAT_FEATURES) == DEPTH_IMG_FORMAT_FEATURES)
        {
            return DepthImageFormat;
        }
    }
    CTK_FATAL("failed to find format that satisfies feature requirements for depth image")
}

static void
TransitionImageLayout(device *Device, VkCommandPool CommandPool, image *Image, VkImageLayout OldLayout, VkImageLayout NewLayout)
{
    // Calculate source/destination access mask and pipeline stage mask.
    VkAccessFlags SourceAccessMask = 0;
    VkAccessFlags DestinationAccessMask = 0;
    VkPipelineStageFlags SourcePipelineStageMask = 0;
    VkPipelineStageFlags DestinationPipelineStageMask = 0;
    if(OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        SourceAccessMask = 0;
        DestinationAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        SourcePipelineStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        DestinationPipelineStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if(OldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        SourceAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        DestinationAccessMask = VK_ACCESS_SHADER_READ_BIT;
        SourcePipelineStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        DestinationPipelineStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if(OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        SourceAccessMask = 0;
        DestinationAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        SourcePipelineStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        DestinationPipelineStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else
    {
        CTK_FATAL("unsupported layout transition")
    }

    // Calculate aspect flags based on new layout of image.
    VkImageAspectFlags AspectMask = 0;

    // Use depth aspect for depth image attachment.
    if(NewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        AspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        // Add stencil flag if image format has a stencil component.
        if(Image->Format == VK_FORMAT_D32_SFLOAT_S8_UINT || Image->Format == VK_FORMAT_D24_UNORM_S8_UINT)
        {
            AspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    }
    // Use color aspect for all other image attachments.
    else
    {
        AspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    VkImageMemoryBarrier ImageMemoryBarrier = {};
    ImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    ImageMemoryBarrier.srcAccessMask = SourceAccessMask;
    ImageMemoryBarrier.dstAccessMask = DestinationAccessMask;
    ImageMemoryBarrier.oldLayout = OldLayout;
    ImageMemoryBarrier.newLayout = NewLayout;
    ImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    ImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    ImageMemoryBarrier.image = Image->Handle;
    ImageMemoryBarrier.subresourceRange.aspectMask = AspectMask;
    ImageMemoryBarrier.subresourceRange.baseMipLevel = 0;
    ImageMemoryBarrier.subresourceRange.levelCount = 1;
    ImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
    ImageMemoryBarrier.subresourceRange.layerCount = 1;

    VkCommandBuffer CommandBuffer = BeginOneTimeCommandBuffer(Device->Logical, CommandPool);
        vkCmdPipelineBarrier(CommandBuffer,
                             SourcePipelineStageMask,
                             DestinationPipelineStageMask,
                             0, // Dependency Flags
                             0, // Memory Barrier Count
                             NULL, // Memory Barriers
                             0, // Buffer Memory Barrier Count
                             NULL, // Buffer Memory Barriers
                             1, // Image Memory Count
                             &ImageMemoryBarrier); // Image Memory Barriers
    SubmitOneTimeCommandBuffer(Device->Logical, Device->GraphicsQueue, CommandPool, CommandBuffer);
}

} // vtk
