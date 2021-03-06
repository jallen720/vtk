#pragma once

#include <vulkan/vulkan.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <stb/stb_image.h>

#include "ctk/ctk.h"
#include "vtk/debug.h"

////////////////////////////////////////////////////////////
/// Macros
////////////////////////////////////////////////////////////
#define VTK_UNSET_INDEX CTK_U32_MAX
#define VTK_LOAD_INSTANCE_EXTENSION_FUNCTION(INSTANCE, FUNCTION_NAME) \
    auto FUNCTION_NAME = (PFN_ ## FUNCTION_NAME)vkGetInstanceProcAddr(INSTANCE, #FUNCTION_NAME); \
    if(FUNCTION_NAME == NULL) \
    { \
        CTK_FATAL("failed to load instance extension function \"%s\"", #FUNCTION_NAME) \
    }
#define VTK_COLOR_COMPONENT_RGBA VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT

namespace vtk {

////////////////////////////////////////////////////////////
/// Data
////////////////////////////////////////////////////////////
struct instance_info
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

struct device_info
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
    VkPhysicalDeviceProperties Properties;
    VkPhysicalDeviceMemoryProperties MemoryProperties;
    queue_family_indexes QueueFamilyIndexes;
    VkSurfaceCapabilitiesKHR SurfaceCapabilities;
    ctk::array<VkSurfaceFormatKHR> SurfaceFormats;
    ctk::array<VkPresentModeKHR> SurfacePresentModes;

    VkDevice Logical;
    VkQueue GraphicsQueue;
    VkQueue PresentQueue;
};

struct image_info
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
    VkImageLayout Layout;
};

struct swapchain
{
    VkSwapchainKHR Handle;
    ctk::sarray<image, 4> Images;
    VkFormat ImageFormat;
    VkExtent2D Extent;
};

struct buffer_info
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

struct attachment
{
    VkAttachmentDescription Description;
    VkClearValue ClearValue;
};

struct subpass
{
    ctk::sarray<u32, 4> PreserveAttachmentIndexes;
    ctk::sarray<VkAttachmentReference, 4> InputAttachmentReferences;
    ctk::sarray<VkAttachmentReference, 4> ColorAttachmentReferences;
    ctk::optional<VkAttachmentReference> DepthAttachmentReference;
};

struct framebuffer_info
{
    ctk::sarray<VkImageView, 16> Attachments;
    VkExtent2D Extent;
    u32 Layers;
};

struct render_pass_info
{
    ctk::sarray<attachment, 16> Attachments;
    ctk::sarray<subpass, 64> Subpasses;
    ctk::sarray<VkSubpassDependency, 64> SubpassDependencies;
    ctk::sarray<framebuffer_info, 4> FramebufferInfos;
};

struct render_pass
{
    VkRenderPass Handle;
    ctk::sarray<VkClearValue, 16> ClearValues;
    ctk::sarray<VkFramebuffer, 4> Framebuffers;
    ctk::sarray<VkCommandBuffer, 4> CommandBuffers;
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
    u32 Binding;
    u32 Location;
    u32 AttributeIndex;
};

struct descriptor_pool_info
{
    ctk::sarray<VkDescriptorPoolSize, 12> Sizes;
    u32 MaxSets;
};

struct graphics_pipeline_info
{
    ctk::sarray<shader_module *, 8> ShaderModules;
    ctk::sarray<VkDescriptorSetLayout, 8> DescriptorSetLayouts;
    ctk::sarray<VkPushConstantRange, 8> PushConstantRanges;
    ctk::sarray<vertex_input, 8> VertexInputs;
    vertex_layout *VertexLayout;
    ctk::sarray<VkViewport, 4> Viewports;
    ctk::sarray<VkRect2D, 4> Scissors;
    ctk::sarray<VkPipelineColorBlendAttachmentState, 4> ColorBlendAttachmentStates;

    VkPipelineInputAssemblyStateCreateInfo InputAssemblyState;
    VkPipelineDepthStencilStateCreateInfo DepthStencilState;
    VkPipelineRasterizationStateCreateInfo RasterizationState;
    VkPipelineMultisampleStateCreateInfo MultisampleState;
    VkPipelineColorBlendStateCreateInfo ColorBlendState;
    u32 Subpass;
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

struct device_query_results
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

struct uniform_buffer
{
    ctk::sarray<region, 4> Regions;
    u32 ElementSize;
};

struct texture_info
{
    VkFilter Filter;
};

struct texture
{
    image Image;
    VkSampler Sampler;
};

struct descriptor_info
{
    VkDescriptorType Type;
    VkShaderStageFlags ShaderStageFlags;
    u32 Count;
    uniform_buffer *UniformBuffer;
    texture *Texture;
};

struct descriptor_binding
{
    u32 Index;
    descriptor_info *Info;
};

struct descriptor_set_info
{
    ctk::sarray<descriptor_binding, 4> DescriptorBindings;
    u32 InstanceCount;
};

struct descriptor_set
{
    ctk::sarray<VkDescriptorSet, 4> Instances;
    ctk::sarray<u32, 4> DynamicOffsets;
    VkDescriptorSetLayout Layout;
};

struct image_memory_info
{
    VkAccessFlags AccessMask;
    VkImageLayout ImageLayout;
    VkPipelineStageFlags PipelineStageMask;
};

struct image_memory_access_mask
{
    VkAccessFlags Source;
    VkAccessFlags Destination;
};

struct image_memory_image_layout
{
    VkImageLayout Source;
    VkImageLayout Destination;
};

struct image_memory_pipeline_stage_mask
{
    VkPipelineStageFlags Source;
    VkPipelineStageFlags Destination;
};

////////////////////////////////////////////////////////////
/// Declarations
////////////////////////////////////////////////////////////
static void
write_to_host_region(VkDevice LogicalDevice, region *Region, void *Data, VkDeviceSize Size, VkDeviceSize OffsetIntoRegion);

static void
transition_image_layout(device *Device, VkCommandPool CommandPool, image *Image, VkImageLayout OldLayout, VkImageLayout NewLayout);

static void
validate_vk_result(VkResult Result, cstr FunctionName, cstr FailureMessage);

////////////////////////////////////////////////////////////
/// Internal
////////////////////////////////////////////////////////////
static VKAPI_ATTR VkBool32 VKAPI_CALL
debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverityFlagBits, VkDebugUtilsMessageTypeFlagsEXT MessageTypeFlags,
               VkDebugUtilsMessengerCallbackDataEXT const *CallbackData, void *UserData)
{
    cstr MessageID = CallbackData->pMessageIdName ? CallbackData->pMessageIdName : "";
    if(VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT & MessageSeverityFlagBits)
    {
        CTK_FATAL("VALIDATION LAYER [%s]: %s\n", MessageID, CallbackData->pMessage)
    }
    else if(VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT & MessageSeverityFlagBits)
    {
        ctk::warning("VALIDATION LAYER [%s]: %s\n", MessageID, CallbackData->pMessage);
    }
    else
    {
        ctk::info("VALIDATION LAYER [%s]: %s\n", MessageID, CallbackData->pMessage);
    }
    return VK_FALSE;
}

template<typename vk_object, typename loader, typename ...args>
static ctk::array<vk_object>
load_vk_objects(loader Loader, args... Args)
{
    u32 Count = 0;
    Loader(Args..., &Count, NULL);
    auto VkObjects = ctk::create_array<vk_object>(Count);
    Loader(Args..., &Count, VkObjects.Data);
    return VkObjects;
}

static cstr
layer_name(VkLayerProperties *Properties)
{
    return Properties->layerName;
}

static cstr
extension_name(VkExtensionProperties *Properties)
{
    return Properties->extensionName;
}

template<typename properties, typename name_selector>
static b32
add_ons_supported(cstr *AddOns, u32 AddOnCount, ctk::array<properties> *SupportedAddOns, name_selector NameSelector)
{
    b32 AllSupported = true;
    for(u32 AddOnIndex = 0; AddOnIndex < AddOnCount; ++AddOnIndex)
    {
        b32 Supported = false;
        cstr AddOn = AddOns[AddOnIndex];
        for(u32 SupportedAddOnsIndex = 0; SupportedAddOnsIndex < SupportedAddOns->Count; ++SupportedAddOnsIndex)
        {
            cstr SupportedAddOnName = NameSelector(ctk::at(SupportedAddOns, SupportedAddOnsIndex));
            if(ctk::equal(AddOn, SupportedAddOnName))
            {
                Supported = true;
                break;
            }
        }
        if(!Supported)
        {
            AllSupported = false;
            ctk::error("add-on \"%s\" is not supported", AddOn);
        }
    }
    return AllSupported;
}

template<typename properties, typename name_selector, typename loader, typename ...args>
static b32
add_ons_supported(cstr *AddOns, u32 AddOnCount, name_selector NameSelector, loader Loader, args... Args)
{
    auto SupportedAddOns = load_vk_objects<properties>(Loader, Args...);
    b32 AllSupported = add_ons_supported<properties>(AddOns, AddOnCount, &SupportedAddOns, NameSelector);
    ctk::_free(&SupportedAddOns);
    return AllSupported;
}

static VkInstance
create_vulkan_instance(cstr AppName, cstr *Extensions, u32 ExtensionCount, cstr *Layers, u32 LayerCount,
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
    InstanceCreateInfo.enabledLayerCount = LayerCount;
    InstanceCreateInfo.ppEnabledLayerNames = Layers;
    InstanceCreateInfo.enabledExtensionCount = ExtensionCount;
    InstanceCreateInfo.ppEnabledExtensionNames = Extensions;
    VkInstance Instance = VK_NULL_HANDLE;
    validate_vk_result(vkCreateInstance(&InstanceCreateInfo, NULL, &Instance), "vkCreateInstance", "failed to create Vulkan instance");
    return Instance;
}

static void
_free(device_query_results *DeviceQueryResults)
{
    ctk::_free(&DeviceQueryResults->Extensions);
    ctk::_free(&DeviceQueryResults->SurfaceFormats);
    ctk::_free(&DeviceQueryResults->SurfacePresentModes);
    ctk::_free(&DeviceQueryResults->QueueFamilies);
    *DeviceQueryResults = {};
}

static VkDeviceQueueCreateInfo
create_queue_create_info(u32 QueueFamilyIndex)
{
    static f32 const QUEUE_PRIORITIES[] = { 1.0f };

    VkDeviceQueueCreateInfo QueueCreateInfo = {};
    QueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    QueueCreateInfo.flags = 0;
    QueueCreateInfo.queueFamilyIndex = QueueFamilyIndex;
    QueueCreateInfo.queueCount = CTK_ARRAY_COUNT(QUEUE_PRIORITIES);
    QueueCreateInfo.pQueuePriorities = QUEUE_PRIORITIES;

    return QueueCreateInfo;
}

static VkImageView
create_image_view(VkDevice LogicalDevice, VkImage Image, VkFormat Format, VkImageAspectFlags AspectFlags)
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
    validate_vk_result(vkCreateImageView(LogicalDevice, &ImageViewCreateInfo, NULL, &ImageView),
                       "vkCreateImageView", "failed to create image view");
    return ImageView;
}

static void
allocate_command_buffers(VkDevice LogicalDevice, VkCommandPool CommandPool, u32 Count, VkCommandBuffer *CommandBuffers)
{
    VkCommandBufferAllocateInfo CommandBufferAllocateInfo = {};
    CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    CommandBufferAllocateInfo.commandPool = CommandPool;
    CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    CommandBufferAllocateInfo.commandBufferCount = Count;
    validate_vk_result(vkAllocateCommandBuffers(LogicalDevice, &CommandBufferAllocateInfo, CommandBuffers),
                       "vkAllocateCommandBuffers", "failed to allocate command buffer");
}

static VkCommandBuffer
begin_one_time_command_buffer(VkDevice LogicalDevice, VkCommandPool CommandPool)
{
    VkCommandBuffer CommandBuffer = {};
    allocate_command_buffers(LogicalDevice, CommandPool, 1, &CommandBuffer);

    VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
    CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    CommandBufferBeginInfo.pInheritanceInfo = NULL;

    vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo);
    return CommandBuffer;
}

static void
submit_one_time_command_buffer(VkDevice LogicalDevice, VkQueue Queue, VkCommandPool CommandPool, VkCommandBuffer CommandBuffer)
{
    vkEndCommandBuffer(CommandBuffer);

    VkSubmitInfo SubmitInfo = {};
    SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    SubmitInfo.commandBufferCount = 1;
    SubmitInfo.pCommandBuffers = &CommandBuffer;
    validate_vk_result(vkQueueSubmit(Queue, 1, &SubmitInfo, VK_NULL_HANDLE),
                       "vkQueueSubmit", "failed to submit one-time command buffer to queue");
    vkQueueWaitIdle(Queue);

    // Cleanup
    vkFreeCommandBuffers(LogicalDevice, CommandPool, 1, &CommandBuffer);
}

////////////////////////////////////////////////////////////
/// Interface
////////////////////////////////////////////////////////////
static instance
create_instance(instance_info *InstanceInfo)
{
    instance Instance = {};
    auto *Layers = &InstanceInfo->Layers;
    auto *Extensions = &InstanceInfo->Extensions;
    if(!add_ons_supported<VkLayerProperties>(Layers->Data, Layers->Count, layer_name, vkEnumerateInstanceLayerProperties))
    {
        CTK_FATAL("not all requested layers supported")
    }

    if(!add_ons_supported<VkExtensionProperties>(Extensions->Data, Extensions->Count, extension_name,
                                                 vkEnumerateInstanceExtensionProperties, (cstr)NULL))
    {
        CTK_FATAL("not all requested extensions supported")
    }

    if(InstanceInfo->Debug)
    {
        // Ensure add-on arrays can fit debug add-ons.
        CTK_ASSERT(Layers->Count < Layers->Size)
        CTK_ASSERT(Extensions->Count < Extensions->Size)

        // Add debug extensions and layers.
        ctk::push(Extensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        ctk::push(Layers, "VK_LAYER_LUNARG_standard_validation");

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
        DebugUtilsMessengerCreateInfo.pfnUserCallback = debug_callback;
        DebugUtilsMessengerCreateInfo.pUserData = NULL;

        // Create Instance
        Instance.Handle = create_vulkan_instance(InstanceInfo->AppName, Extensions->Data, Extensions->Count, Layers->Data, Layers->Count,
                                                 &DebugUtilsMessengerCreateInfo);

        // Create Debug Utils Messenger
        VTK_LOAD_INSTANCE_EXTENSION_FUNCTION(Instance.Handle, vkCreateDebugUtilsMessengerEXT)
        validate_vk_result(vkCreateDebugUtilsMessengerEXT(Instance.Handle, &DebugUtilsMessengerCreateInfo, NULL,
                                                        &Instance.DebugUtilsMessenger),
                           "vkCreateDebugUtilsMessengerEXT", "failed to create debug messenger");
    }
    else
    {
        Instance.Handle = create_vulkan_instance(InstanceInfo->AppName, Extensions->Data, Extensions->Count, Layers->Data, Layers->Count,
                                                 NULL);
    }
    return Instance;
}

static device
create_device(VkInstance Instance, VkSurfaceKHR PlatformSurface, device_info *DeviceInfo)
{
    device Device = {};
    auto *Extensions = &DeviceInfo->Extensions;

    ////////////////////////////////////////////////////////////
    /// Physical Device
    ////////////////////////////////////////////////////////////
    auto PhysicalDevices = load_vk_objects<VkPhysicalDevice>(vkEnumeratePhysicalDevices, Instance);
    b32 FoundSuitableDevice = false;
    for(u32 PhysicalDeviceIndex = 0; PhysicalDeviceIndex < PhysicalDevices.Count && !FoundSuitableDevice; ++PhysicalDeviceIndex)
    {
        device_query_results SelectedDeviceQueryResults = {};
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
        vkGetPhysicalDeviceProperties(PhysicalDevice, &SelectedDeviceQueryResults.Properties);
        vkGetPhysicalDeviceFeatures(PhysicalDevice, &SelectedDeviceQueryResults.Features);
        vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &SelectedDeviceQueryResults.MemoryProperties);
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, PlatformSurface, &SelectedDeviceQueryResults.SurfaceCapabilities);
        SelectedDeviceQueryResults.Extensions =
            load_vk_objects<VkExtensionProperties>(vkEnumerateDeviceExtensionProperties, PhysicalDevice, (cstr)NULL);
        SelectedDeviceQueryResults.SurfaceFormats =
            load_vk_objects<VkSurfaceFormatKHR>(vkGetPhysicalDeviceSurfaceFormatsKHR, PhysicalDevice, PlatformSurface);
        SelectedDeviceQueryResults.SurfacePresentModes =
            load_vk_objects<VkPresentModeKHR>(vkGetPhysicalDeviceSurfacePresentModesKHR, PhysicalDevice, PlatformSurface);
        SelectedDeviceQueryResults.QueueFamilies =
            load_vk_objects<VkQueueFamilyProperties>(vkGetPhysicalDeviceQueueFamilyProperties, PhysicalDevice);

        // Find queue family indexes.
        for(u32 QueueFamilyIndex = 0; QueueFamilyIndex < SelectedDeviceQueryResults.QueueFamilies.Count; ++QueueFamilyIndex)
        {
            VkQueueFamilyProperties *QueueFamily = SelectedDeviceQueryResults.QueueFamilies + QueueFamilyIndex;
            if(QueueFamily->queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                SelectedDeviceQueryResults.QueueFamilyIndexes.Graphics = QueueFamilyIndex;
            }
            VkBool32 PresentationSupported = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, QueueFamilyIndex, PlatformSurface, &PresentationSupported);
            if(PresentationSupported == VK_TRUE)
            {
                SelectedDeviceQueryResults.QueueFamilyIndexes.Present = QueueFamilyIndex;
            }
        }

        // Display device limits.
        print_device_limits(&SelectedDeviceQueryResults.Properties.limits);

        ////////////////////////////////////////////////////////////
        /// Device Info Validation
        ////////////////////////////////////////////////////////////

        // Check if device has requested features.
        #define VTK_INTERNAL_DEVICE_FEATURE_CHECK(FEATURE) \
            if(DeviceInfo->Features.FEATURE && !SelectedDeviceQueryResults.Features.FEATURE) \
            { \
                DeviceFeaturesSupported = false; \
                ctk::error("requested device feature \"" #FEATURE "\" not supported"); \
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
        b32 SwapchainsSupported = SelectedDeviceQueryResults.SurfaceFormats.Count != 0 &&
                                  SelectedDeviceQueryResults.SurfacePresentModes.Count != 0;
        if(SelectedDeviceQueryResults.Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
           SelectedDeviceQueryResults.QueueFamilyIndexes.Graphics != VTK_UNSET_INDEX &&
           SelectedDeviceQueryResults.QueueFamilyIndexes.Present != VTK_UNSET_INDEX &&
           add_ons_supported<VkExtensionProperties>(Extensions->Data, Extensions->Count, &SelectedDeviceQueryResults.Extensions,
                                                    extension_name);
           DeviceFeaturesSupported &&
           SwapchainsSupported)
        {
            ctk::info("physical device \"%s\" satisfies all requirements", SelectedDeviceQueryResults.Properties.deviceName);
            FoundSuitableDevice = true;

            // Initialize physical device with selected device info.
            Device.Physical = PhysicalDevice;
            Device.Properties = SelectedDeviceQueryResults.Properties;
            Device.MemoryProperties = SelectedDeviceQueryResults.MemoryProperties;
            Device.QueueFamilyIndexes = SelectedDeviceQueryResults.QueueFamilyIndexes;
            Device.SurfaceCapabilities = SelectedDeviceQueryResults.SurfaceCapabilities;
            Device.SurfaceFormats = ctk::create_array(&SelectedDeviceQueryResults.SurfaceFormats);
            Device.SurfacePresentModes = ctk::create_array(&SelectedDeviceQueryResults.SurfacePresentModes);
        }
        else
        {
            ctk::error("physical device \"%s\" does not satisfy all requirements", SelectedDeviceQueryResults.Properties.deviceName);
        }
        _free(&SelectedDeviceQueryResults);
    }
    if(!FoundSuitableDevice)
    {
        CTK_FATAL("failed to find suitable device")
    }

    ////////////////////////////////////////////////////////////
    /// Logical Device
    ////////////////////////////////////////////////////////////
    ctk::sarray<VkDeviceQueueCreateInfo, 2> QueueCreateInfos = {};
    push(&QueueCreateInfos, create_queue_create_info(Device.QueueFamilyIndexes.Graphics));
    if(Device.QueueFamilyIndexes.Present != Device.QueueFamilyIndexes.Graphics)
    {
        push(&QueueCreateInfos, create_queue_create_info(Device.QueueFamilyIndexes.Present));
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
    LogicalDeviceCreateInfo.pEnabledFeatures = &DeviceInfo->Features;
    validate_vk_result(vkCreateDevice(Device.Physical, &LogicalDeviceCreateInfo, NULL, &Device.Logical),
                       "vkCreateDevice", "failed to create logical device");

    // Get logical device queues.
    static u32 const QUEUE_INDEX = 0; // Currently only supporting 1 queue per family.
    vkGetDeviceQueue(Device.Logical, Device.QueueFamilyIndexes.Graphics, QUEUE_INDEX, &Device.GraphicsQueue);
    vkGetDeviceQueue(Device.Logical, Device.QueueFamilyIndexes.Present, QUEUE_INDEX, &Device.PresentQueue);

    // Cleanup
    ctk::_free(&PhysicalDevices);

    return Device;
}

static swapchain
create_swapchain(device *Device, VkSurfaceKHR PlatformSurface)
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

        // Prefer 4-component 8-bit BGRA unnormalized format and sRGB color space.
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
    SwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
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
    validate_vk_result(vkCreateSwapchainKHR(Device->Logical, &SwapchainCreateInfo, NULL, &Swapchain.Handle),
                       "vkCreateSwapchainKHR", "failed to create swapchain");

    // Store surface state used to create swapchain for future reference.
    Swapchain.ImageFormat = SelectedFormat.format;
    Swapchain.Extent = Device->SurfaceCapabilities.currentExtent;

    ////////////////////////////////////////////////////////////
    /// Swapchain Image Creation
    ////////////////////////////////////////////////////////////
    auto Images = load_vk_objects<VkImage>(vkGetSwapchainImagesKHR, Device->Logical, Swapchain.Handle);
    for(u32 ImageIndex = 0; ImageIndex < Images.Count; ++ImageIndex)
    {
        image *SwapchainImage = ctk::push(&Swapchain.Images);
        SwapchainImage->Handle = Images[ImageIndex];
        SwapchainImage->View = create_image_view(Device->Logical, SwapchainImage->Handle, SelectedFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);
        SwapchainImage->Width = Swapchain.Extent.width;
        SwapchainImage->Height = Swapchain.Extent.height;
        SwapchainImage->Format = Swapchain.ImageFormat;
    }

    // Cleanup
    ctk::_free(&Images);

    return Swapchain;
}

static VkCommandPool
create_command_pool(VkDevice LogicalDevice, u32 QueueFamilyIndex)
{
    VkCommandPoolCreateInfo CommandPoolCreateInfo = {};
    CommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    CommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    CommandPoolCreateInfo.queueFamilyIndex = QueueFamilyIndex;
    VkCommandPool CommandPool = VK_NULL_HANDLE;
    validate_vk_result(vkCreateCommandPool(LogicalDevice, &CommandPoolCreateInfo, NULL, &CommandPool),
                       "vkCreateCommandPool", "failed to create command pool");
    return CommandPool;
}

static VkDeviceMemory
allocate_device_memory(device *Device, VkMemoryRequirements *MemoryRequirements, VkMemoryPropertyFlags MemoryPropertyFlags)
{
    // Find memory type index from device based on memory property flags.
    u32 SelectedMemoryTypeIndex = VTK_UNSET_INDEX;
    for(u32 MemoryTypeIndex = 0; MemoryTypeIndex < Device->MemoryProperties.memoryTypeCount; ++MemoryTypeIndex)
    {
        // Ensure index refers to memory type from memory requirements.
        if(!(MemoryRequirements->memoryTypeBits & (1 << MemoryTypeIndex)))
        {
            continue;
        }

        // Check if memory at index has correct properties.
        if((Device->MemoryProperties.memoryTypes[MemoryTypeIndex].propertyFlags & MemoryPropertyFlags) == MemoryPropertyFlags)
        {
            SelectedMemoryTypeIndex = MemoryTypeIndex;
            break;
        }
    }
    if(SelectedMemoryTypeIndex == VTK_UNSET_INDEX)
    {
        CTK_FATAL("failed to find memory type that satisfies property requirements")
    }

    // Allocate memory
    VkMemoryAllocateInfo MemoryAllocateInfo = {};
    MemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    MemoryAllocateInfo.allocationSize = MemoryRequirements->size;
    MemoryAllocateInfo.memoryTypeIndex = SelectedMemoryTypeIndex;
    VkDeviceMemory Memory = VK_NULL_HANDLE;
    validate_vk_result(vkAllocateMemory(Device->Logical, &MemoryAllocateInfo, NULL, &Memory),
                       "vkAllocateMemory", "failed to allocate memory");

    return Memory;
}

static buffer
create_buffer(device *Device, buffer_info *BufferInfo)
{
    buffer Buffer = {};
    Buffer.Size = BufferInfo->Size;

    // Buffer Creation
    VkBufferCreateInfo BufferCreateInfo = {};
    BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    BufferCreateInfo.size = BufferInfo->Size;
    BufferCreateInfo.usage = BufferInfo->UsageFlags;
    BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    BufferCreateInfo.queueFamilyIndexCount = 0;
    BufferCreateInfo.pQueueFamilyIndices = NULL; // Ignored if sharingMode is not VK_SHARING_MODE_CONCURRENT.
    validate_vk_result(vkCreateBuffer(Device->Logical, &BufferCreateInfo, NULL, &Buffer.Handle),
                       "vkCreateBuffer", "failed to create buffer");

    // Memory Allocation & Binding
    VkMemoryRequirements MemoryRequirements = {};
    vkGetBufferMemoryRequirements(Device->Logical, Buffer.Handle, &MemoryRequirements);
    Buffer.Memory = allocate_device_memory(Device, &MemoryRequirements, BufferInfo->MemoryPropertyFlags);
    validate_vk_result(vkBindBufferMemory(Device->Logical, Buffer.Handle, Buffer.Memory, 0),
                       "vkBindBufferMemory", "failed to bind buffer memory");

    return Buffer;
}

static void
destroy_buffer(VkDevice LogicalDevice, buffer *Buffer)
{
    vkDestroyBuffer(LogicalDevice, Buffer->Handle, NULL);
    vkFreeMemory(LogicalDevice, Buffer->Memory, NULL);
}

static region
allocate_region(buffer *Buffer, u32 Size, VkDeviceSize Alignment = 8)
{
    VkDeviceSize AlignmentOffset = Buffer->End % Alignment;
    region Region = {};
    Region.Buffer = Buffer;
    Region.Offset = AlignmentOffset ? Buffer->End - AlignmentOffset + Alignment : Buffer->End;
    if(Region.Offset + Size > Buffer->Size)
    {
        CTK_FATAL("buffer (size=%u end=%u) cannot allocate region of size %u and alignment %u (only %u bytes left)",
                  Buffer->Size, Buffer->End, Size, Alignment, Buffer->Size - Buffer->End);
    }
    Region.Size = Size;
    Buffer->End += Size;
    return Region;
}

static image
create_image(device *Device, image_info *ImageInfo)
{
    image Image = {};
    Image.Width = ImageInfo->Width;
    Image.Height = ImageInfo->Height;
    Image.Format = ImageInfo->Format;

    ////////////////////////////////////////////////////////////
    /// Vulkan Image
    ////////////////////////////////////////////////////////////
    VkImageCreateInfo ImageCreateInfo = {};
    ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    ImageCreateInfo.flags = 0;
    ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    ImageCreateInfo.format = ImageInfo->Format;
    ImageCreateInfo.extent.width = ImageInfo->Width;
    ImageCreateInfo.extent.height = ImageInfo->Height;
    ImageCreateInfo.extent.depth = 1;
    ImageCreateInfo.mipLevels = 1;
    ImageCreateInfo.arrayLayers = 1;
    ImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    ImageCreateInfo.tiling = ImageInfo->Tiling;
    ImageCreateInfo.usage = ImageInfo->UsageFlags;
    ImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    ImageCreateInfo.queueFamilyIndexCount = 0;
    ImageCreateInfo.pQueueFamilyIndices = NULL; // Ignored if sharingMode is not VK_SHARING_MODE_CONCURRENT.
    ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    validate_vk_result(vkCreateImage(Device->Logical, &ImageCreateInfo, NULL, &Image.Handle), "vkCreateImage", "failed to create image");

    ////////////////////////////////////////////////////////////
    /// Memory Allocation
    ////////////////////////////////////////////////////////////
    VkMemoryRequirements MemoryRequirements = {};
    vkGetImageMemoryRequirements(Device->Logical, Image.Handle, &MemoryRequirements);
    Image.Memory = allocate_device_memory(Device, &MemoryRequirements, ImageInfo->MemoryPropertyFlags);
    validate_vk_result(vkBindImageMemory(Device->Logical, Image.Handle, Image.Memory, 0),
                       "vkBindImageMemory", "failed to bind image memory");

    ////////////////////////////////////////////////////////////
    /// View
    ////////////////////////////////////////////////////////////
    Image.View = create_image_view(Device->Logical, Image.Handle, Image.Format, ImageInfo->AspectMask);

    return Image;
}

static VkSampler
create_sampler(VkDevice LogicalDevice, VkFilter Filter) {
    VkSampler Sampler = VK_NULL_HANDLE;

    // Create texture sampler.
    VkSamplerCreateInfo SamplerCreateInfo = {};
    SamplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    SamplerCreateInfo.magFilter = Filter;
    SamplerCreateInfo.minFilter = Filter;
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
    validate_vk_result(vkCreateSampler(LogicalDevice, &SamplerCreateInfo, NULL, &Sampler),
                       "vkCreateSampler", "failed to create texture sampler");

    return Sampler;
}

static texture
create_texture(device *Device, VkCommandPool CommandPool, region *StagingRegion, cstr Path, texture_info *TextureInfo)
{
    texture Texture = {};

    // Load image from path.
    static u32 const IMAGE_CHANNELS = STBI_rgb_alpha;
    s32 ImageWidth = 0;
    s32 ImageHeight = 0;
    s32 ImageChannelCount = 0;
    stbi_uc *ImageData = stbi_load(Path, &ImageWidth, &ImageHeight, &ImageChannelCount, IMAGE_CHANNELS);
    if(ImageData == NULL)
    {
        CTK_FATAL("failed to load image from \"%s\"", Path)
    }

    // Create texture image.
    image_info TextureImageConfig = {};
    TextureImageConfig.Width = ImageWidth;
    TextureImageConfig.Height = ImageHeight;
    TextureImageConfig.Format = VK_FORMAT_R8G8B8A8_UNORM; // Since stbi_load() used STBI_rgb_alpha.
    TextureImageConfig.Tiling = VK_IMAGE_TILING_OPTIMAL;
    TextureImageConfig.UsageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    TextureImageConfig.MemoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    TextureImageConfig.AspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    Texture.Image = create_image(Device, &TextureImageConfig);

    // Write to texture image.
    write_to_host_region(Device->Logical, StagingRegion, ImageData, Texture.Image.Width * Texture.Image.Height * IMAGE_CHANNELS, 0);
    transition_image_layout(Device, CommandPool, &Texture.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    VkBufferImageCopy BufferImageCopyRegions[1] = {};
    BufferImageCopyRegions[0].bufferOffset = 0;
    BufferImageCopyRegions[0].bufferRowLength = 0;
    BufferImageCopyRegions[0].bufferImageHeight = 0;
    BufferImageCopyRegions[0].imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    BufferImageCopyRegions[0].imageSubresource.mipLevel = 0;
    BufferImageCopyRegions[0].imageSubresource.baseArrayLayer = 0;
    BufferImageCopyRegions[0].imageSubresource.layerCount = 1;
    BufferImageCopyRegions[0].imageOffset.x = 0;
    BufferImageCopyRegions[0].imageOffset.y = 0;
    BufferImageCopyRegions[0].imageOffset.z = 0;
    BufferImageCopyRegions[0].imageExtent.width = Texture.Image.Width;
    BufferImageCopyRegions[0].imageExtent.height = Texture.Image.Height;
    BufferImageCopyRegions[0].imageExtent.depth = 1;
    VkCommandBuffer CommandBuffer = begin_one_time_command_buffer(Device->Logical, CommandPool);
        vkCmdCopyBufferToImage(CommandBuffer, StagingRegion->Buffer->Handle,
                               Texture.Image.Handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               CTK_ARRAY_COUNT(BufferImageCopyRegions), BufferImageCopyRegions);
    submit_one_time_command_buffer(Device->Logical, Device->GraphicsQueue, CommandPool, CommandBuffer);
    transition_image_layout(Device, CommandPool, &Texture.Image,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // Create texture sampler.
    Texture.Sampler = create_sampler(Device->Logical, TextureInfo->Filter);

    // Cleanup
    stbi_image_free(ImageData);

    return Texture;
}

static VkFramebuffer
create_framebuffer(VkDevice LogicalDevice, VkRenderPass RenderPass, framebuffer_info *FramebufferInfo)
{
    VkFramebufferCreateInfo FramebufferCreateInfo = {};
    FramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    FramebufferCreateInfo.renderPass = RenderPass;
    FramebufferCreateInfo.attachmentCount = FramebufferInfo->Attachments.Count;
    FramebufferCreateInfo.pAttachments = FramebufferInfo->Attachments.Data;
    FramebufferCreateInfo.width = FramebufferInfo->Extent.width;
    FramebufferCreateInfo.height = FramebufferInfo->Extent.height;
    FramebufferCreateInfo.layers = FramebufferInfo->Layers;
    VkFramebuffer Framebuffer = VK_NULL_HANDLE;
    validate_vk_result(vkCreateFramebuffer(LogicalDevice, &FramebufferCreateInfo, NULL, &Framebuffer),
                       "vkCreateFramebuffer", "failed to create framebuffer");
    return Framebuffer;
}

static render_pass
create_render_pass(VkDevice LogicalDevice, VkCommandPool CommandPool, render_pass_info *RenderPassInfo)
{
    render_pass RenderPass = {};

    // Attachments
    ctk::sarray<VkAttachmentDescription, 16> AttachmentDescriptions = {};
    for(u32 AttachmentIndex = 0; AttachmentIndex < RenderPassInfo->Attachments.Count; ++AttachmentIndex)
    {
        attachment *Attachment = RenderPassInfo->Attachments + AttachmentIndex;
        ctk::push(&AttachmentDescriptions, Attachment->Description);
        ctk::push(&RenderPass.ClearValues, Attachment->ClearValue);
    }

    // Subpasses
    ctk::sarray<VkSubpassDescription, 64> SubpassDescriptions = {};
    for(u32 SubpassIndex = 0; SubpassIndex < RenderPassInfo->Subpasses.Count; ++SubpassIndex)
    {
        subpass *Subpass = ctk::at(&RenderPassInfo->Subpasses, SubpassIndex);
        VkSubpassDescription *SubpassDescription = ctk::push(&SubpassDescriptions);
        SubpassDescription->pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        SubpassDescription->inputAttachmentCount = Subpass->InputAttachmentReferences.Count;
        SubpassDescription->pInputAttachments = Subpass->InputAttachmentReferences.Data;
        SubpassDescription->colorAttachmentCount = Subpass->ColorAttachmentReferences.Count;
        SubpassDescription->pColorAttachments = Subpass->ColorAttachmentReferences.Data;
        SubpassDescription->pResolveAttachments = NULL;
        SubpassDescription->pDepthStencilAttachment = Subpass->DepthAttachmentReference ? &Subpass->DepthAttachmentReference.Value : NULL;
        SubpassDescription->preserveAttachmentCount = Subpass->PreserveAttachmentIndexes.Count;
        SubpassDescription->pPreserveAttachments = Subpass->PreserveAttachmentIndexes.Data;
    }

    // Render Pass
    VkRenderPassCreateInfo RenderPassCreateInfo = {};
    RenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    RenderPassCreateInfo.attachmentCount = AttachmentDescriptions.Count;
    RenderPassCreateInfo.pAttachments = AttachmentDescriptions.Data;
    RenderPassCreateInfo.subpassCount = SubpassDescriptions.Count;
    RenderPassCreateInfo.pSubpasses = SubpassDescriptions.Data;
    RenderPassCreateInfo.dependencyCount = RenderPassInfo->SubpassDependencies.Count;
    RenderPassCreateInfo.pDependencies = RenderPassInfo->SubpassDependencies.Data;
    validate_vk_result(vkCreateRenderPass(LogicalDevice, &RenderPassCreateInfo, NULL, &RenderPass.Handle),
                       "vkCreateRenderPass", "failed to create render pass");

    // Framebuffers
    u32 FramebufferCount = RenderPassInfo->FramebufferInfos.Count;
    for(u32 FramebufferIndex = 0; FramebufferIndex < FramebufferCount; ++FramebufferIndex)
    {
        ctk::push(&RenderPass.Framebuffers,
                  create_framebuffer(LogicalDevice, RenderPass.Handle, RenderPassInfo->FramebufferInfos + FramebufferIndex));
    }

    // Command Buffers (1 per framebuffer)
    RenderPass.CommandBuffers.Count = FramebufferCount;
    allocate_command_buffers(LogicalDevice, CommandPool, FramebufferCount, RenderPass.CommandBuffers.Data);

    return RenderPass;
}

static shader_module
create_shader_module(VkDevice LogicalDevice, cstr Path, VkShaderStageFlagBits StageBit)
{
    shader_module ShaderModule = {};
    ShaderModule.StageBit = StageBit;
    ctk::array<u8> ShaderByteCode = ctk::read_file<u8>(Path);

    VkShaderModuleCreateInfo ShaderModuleCreateInfo = {};
    ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ShaderModuleCreateInfo.flags = 0;
    ShaderModuleCreateInfo.codeSize = ctk::byte_size(&ShaderByteCode);
    ShaderModuleCreateInfo.pCode = (u32 const *)ShaderByteCode.Data;
    validate_vk_result(vkCreateShaderModule(LogicalDevice, &ShaderModuleCreateInfo, NULL, &ShaderModule.Handle),
                       "vkCreateShaderModule", "failed to create shader module");

    // Cleanup
    ctk::_free(&ShaderByteCode);

    return ShaderModule;
}

static u32
push_vertex_attribute(vertex_layout *VertexLayout, u32 ElementCount)
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
    ctk::push(&VertexLayout->Attributes, { FORMATS[ElementCount - 1], AttributeSize, VertexLayout->Size });
    VertexLayout->Size += AttributeSize;
    return AttributeIndex;
}

static graphics_pipeline_info
default_graphics_pipeline_info()
{
    graphics_pipeline_info GraphicsPipelineInfo = {};
    GraphicsPipelineInfo.InputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    GraphicsPipelineInfo.InputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    GraphicsPipelineInfo.InputAssemblyState.primitiveRestartEnable = VK_FALSE;

    GraphicsPipelineInfo.DepthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    // Depth
    GraphicsPipelineInfo.DepthStencilState.depthTestEnable = VK_FALSE;
    GraphicsPipelineInfo.DepthStencilState.depthWriteEnable = VK_FALSE;
    GraphicsPipelineInfo.DepthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
    GraphicsPipelineInfo.DepthStencilState.depthBoundsTestEnable = VK_FALSE;
    GraphicsPipelineInfo.DepthStencilState.minDepthBounds = 0.0f;
    GraphicsPipelineInfo.DepthStencilState.maxDepthBounds = 1.0f;
    // Stencil
    GraphicsPipelineInfo.DepthStencilState.stencilTestEnable = VK_FALSE;
    GraphicsPipelineInfo.DepthStencilState.front.compareOp = VK_COMPARE_OP_NEVER;
    GraphicsPipelineInfo.DepthStencilState.front.passOp = VK_STENCIL_OP_KEEP;
    GraphicsPipelineInfo.DepthStencilState.front.failOp = VK_STENCIL_OP_KEEP;
    GraphicsPipelineInfo.DepthStencilState.front.depthFailOp = VK_STENCIL_OP_KEEP;
    GraphicsPipelineInfo.DepthStencilState.front.compareMask = 0xFF;
    GraphicsPipelineInfo.DepthStencilState.front.writeMask = 0xFF;
    GraphicsPipelineInfo.DepthStencilState.front.reference = 1;
    GraphicsPipelineInfo.DepthStencilState.back = GraphicsPipelineInfo.DepthStencilState.front;

    GraphicsPipelineInfo.RasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    GraphicsPipelineInfo.RasterizationState.depthClampEnable = VK_FALSE; // Don't clamp fragments within depth range.
    GraphicsPipelineInfo.RasterizationState.rasterizerDiscardEnable = VK_FALSE;
    GraphicsPipelineInfo.RasterizationState.polygonMode = VK_POLYGON_MODE_FILL; // Only available mode on AMD gpus?
    GraphicsPipelineInfo.RasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
    GraphicsPipelineInfo.RasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    GraphicsPipelineInfo.RasterizationState.depthBiasEnable = VK_FALSE;
    GraphicsPipelineInfo.RasterizationState.depthBiasConstantFactor = 0.0f;
    GraphicsPipelineInfo.RasterizationState.depthBiasClamp = 0.0f;
    GraphicsPipelineInfo.RasterizationState.depthBiasSlopeFactor = 0.0f;
    GraphicsPipelineInfo.RasterizationState.lineWidth = 1.0f;

    GraphicsPipelineInfo.MultisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    GraphicsPipelineInfo.MultisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    GraphicsPipelineInfo.MultisampleState.sampleShadingEnable = VK_FALSE;
    GraphicsPipelineInfo.MultisampleState.minSampleShading = 1.0f;
    GraphicsPipelineInfo.MultisampleState.pSampleMask = NULL;
    GraphicsPipelineInfo.MultisampleState.alphaToCoverageEnable = VK_FALSE;
    GraphicsPipelineInfo.MultisampleState.alphaToOneEnable = VK_FALSE;

    GraphicsPipelineInfo.ColorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    GraphicsPipelineInfo.ColorBlendState.logicOpEnable = VK_FALSE;
    GraphicsPipelineInfo.ColorBlendState.logicOp = VK_LOGIC_OP_COPY;
    GraphicsPipelineInfo.ColorBlendState.attachmentCount = 0;
    GraphicsPipelineInfo.ColorBlendState.pAttachments = NULL;
    GraphicsPipelineInfo.ColorBlendState.blendConstants[0] = 1.0f;
    GraphicsPipelineInfo.ColorBlendState.blendConstants[1] = 1.0f;
    GraphicsPipelineInfo.ColorBlendState.blendConstants[2] = 1.0f;
    GraphicsPipelineInfo.ColorBlendState.blendConstants[3] = 1.0f;

    return GraphicsPipelineInfo;
}


static VkPipelineColorBlendAttachmentState
default_color_blend_attachment_state()
{
    VkPipelineColorBlendAttachmentState ColorBlendAttachmentState = {};
    ColorBlendAttachmentState.blendEnable = VK_FALSE;
    ColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    ColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    ColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    ColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    ColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    ColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
    ColorBlendAttachmentState.colorWriteMask = VTK_COLOR_COMPONENT_RGBA;
    return ColorBlendAttachmentState;
}

static graphics_pipeline
create_graphics_pipeline(VkDevice LogicalDevice, render_pass *RenderPass, u32 SubpassIndex, graphics_pipeline_info *GraphicsPipelineInfo)
{
    graphics_pipeline GraphicsPipeline = {};

    ////////////////////////////////////////////////////////////
    /// Shader Stages
    ////////////////////////////////////////////////////////////
    ctk::sarray<VkPipelineShaderStageCreateInfo, 4> ShaderStages = {};
    for(u32 ShaderModuleIndex = 0; ShaderModuleIndex < GraphicsPipelineInfo->ShaderModules.Count; ++ShaderModuleIndex)
    {
        shader_module *ShaderModule = GraphicsPipelineInfo->ShaderModules[ShaderModuleIndex];

        VkPipelineShaderStageCreateInfo *ShaderStageCreateInfo = ctk::push(&ShaderStages);
        ShaderStageCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ShaderStageCreateInfo->flags = 0;
        ShaderStageCreateInfo->stage = ShaderModule->StageBit;
        ShaderStageCreateInfo->module = ShaderModule->Handle;
        ShaderStageCreateInfo->pName = "main";
        ShaderStageCreateInfo->pSpecializationInfo = NULL;
    }

    ////////////////////////////////////////////////////////////
    /// Pipeline Layout
    ////////////////////////////////////////////////////////////
    VkPipelineLayoutCreateInfo LayoutCreateInfo = {};
    LayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    LayoutCreateInfo.setLayoutCount = GraphicsPipelineInfo->DescriptorSetLayouts.Count;
    LayoutCreateInfo.pSetLayouts = GraphicsPipelineInfo->DescriptorSetLayouts.Data;
    LayoutCreateInfo.pushConstantRangeCount = GraphicsPipelineInfo->PushConstantRanges.Count;
    LayoutCreateInfo.pPushConstantRanges = GraphicsPipelineInfo->PushConstantRanges.Data;
    validate_vk_result(vkCreatePipelineLayout(LogicalDevice, &LayoutCreateInfo, NULL, &GraphicsPipeline.Layout),
                       "vkCreatePipelineLayout", "failed to create graphics pipeline layout");

    ////////////////////////////////////////////////////////////
    /// Vertex Input State
    ////////////////////////////////////////////////////////////
    ctk::sarray<VkVertexInputAttributeDescription, 4> VertexAttributeDescriptions = {};
    for(u32 InputIndex = 0; InputIndex < GraphicsPipelineInfo->VertexInputs.Count; ++InputIndex)
    {
        vertex_input *VertexInput = ctk::at(&GraphicsPipelineInfo->VertexInputs, InputIndex);
        vertex_attribute *VertexAttribute = GraphicsPipelineInfo->VertexLayout->Attributes + VertexInput->AttributeIndex;

        VkVertexInputAttributeDescription *AttributeDescription = ctk::push(&VertexAttributeDescriptions);
        AttributeDescription->location = VertexInput->Location;
        AttributeDescription->binding = VertexInput->Binding;
        AttributeDescription->format = VertexAttribute->Format;
        AttributeDescription->offset = VertexAttribute->Offset;
    }

    ctk::sarray<VkVertexInputBindingDescription, 4> VertexBindingDescriptions = {};
    VkVertexInputBindingDescription *BindingDescription = ctk::push(&VertexBindingDescriptions);
    BindingDescription->binding = 0;
    BindingDescription->stride = GraphicsPipelineInfo->VertexLayout->Size;
    BindingDescription->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkPipelineVertexInputStateCreateInfo VertexInputState = {};
    VertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    VertexInputState.vertexBindingDescriptionCount = VertexBindingDescriptions.Count;
    VertexInputState.pVertexBindingDescriptions = VertexBindingDescriptions.Data;
    VertexInputState.vertexAttributeDescriptionCount = VertexAttributeDescriptions.Count;
    VertexInputState.pVertexAttributeDescriptions = VertexAttributeDescriptions.Data;

    ////////////////////////////////////////////////////////////
    /// Viewport State
    ////////////////////////////////////////////////////////////
    VkPipelineViewportStateCreateInfo ViewportState = {};
    ViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    ViewportState.viewportCount = GraphicsPipelineInfo->Viewports.Count;
    ViewportState.pViewports = GraphicsPipelineInfo->Viewports.Data;
    ViewportState.scissorCount = GraphicsPipelineInfo->Scissors.Count;
    ViewportState.pScissors = GraphicsPipelineInfo->Scissors.Data;

    ////////////////////////////////////////////////////////////
    /// Color Blend State
    ////////////////////////////////////////////////////////////
    GraphicsPipelineInfo->ColorBlendState.attachmentCount = GraphicsPipelineInfo->ColorBlendAttachmentStates.Count;
    GraphicsPipelineInfo->ColorBlendState.pAttachments = GraphicsPipelineInfo->ColorBlendAttachmentStates.Data;

    ////////////////////////////////////////////////////////////
    /// Graphics Pipeline
    ////////////////////////////////////////////////////////////
    VkGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo = {};
    GraphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    GraphicsPipelineCreateInfo.stageCount = ShaderStages.Count;
    GraphicsPipelineCreateInfo.pStages = ShaderStages.Data;
    GraphicsPipelineCreateInfo.pVertexInputState = &VertexInputState;
    GraphicsPipelineCreateInfo.pInputAssemblyState = &GraphicsPipelineInfo->InputAssemblyState;
    GraphicsPipelineCreateInfo.pTessellationState = NULL;
    GraphicsPipelineCreateInfo.pViewportState = &ViewportState;
    GraphicsPipelineCreateInfo.pRasterizationState = &GraphicsPipelineInfo->RasterizationState;
    GraphicsPipelineCreateInfo.pMultisampleState = &GraphicsPipelineInfo->MultisampleState;
    GraphicsPipelineCreateInfo.pDepthStencilState = &GraphicsPipelineInfo->DepthStencilState;
    GraphicsPipelineCreateInfo.pColorBlendState = &GraphicsPipelineInfo->ColorBlendState;
    GraphicsPipelineCreateInfo.pDynamicState = NULL;
    GraphicsPipelineCreateInfo.layout = GraphicsPipeline.Layout;
    GraphicsPipelineCreateInfo.renderPass = RenderPass->Handle;
    GraphicsPipelineCreateInfo.subpass = SubpassIndex;
    GraphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    GraphicsPipelineCreateInfo.basePipelineIndex = -1;
    validate_vk_result(vkCreateGraphicsPipelines(LogicalDevice, VK_NULL_HANDLE, 1, &GraphicsPipelineCreateInfo, NULL,
                                                 &GraphicsPipeline.Handle),
                       "vkCreateGraphicsPipelines", "failed to create graphics pipeline");

    return GraphicsPipeline;
}

static VkSemaphore
create_semaphore(VkDevice LogicalDevice)
{
    VkSemaphoreCreateInfo SemaphoreCreateInfo = {};
    SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    SemaphoreCreateInfo.flags = 0;
    VkSemaphore Semaphore = VK_NULL_HANDLE;
    validate_vk_result(vkCreateSemaphore(LogicalDevice, &SemaphoreCreateInfo, NULL, &Semaphore),
                       "vkCreateSemaphore", "failed to create semaphore");
    return Semaphore;
}

static VkFence
create_fence(VkDevice LogicalDevice)
{
    VkFenceCreateInfo FenceCreateInfo = {};
    FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    FenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    VkFence Fence = VK_NULL_HANDLE;
    validate_vk_result(vkCreateFence(LogicalDevice, &FenceCreateInfo, NULL, &Fence), "vkCreateFence", "failed to create fence");
    return Fence;
}

static frame_state
create_frame_state(VkDevice LogicalDevice, u32 FrameCount, u32 SwapchainImageCount)
{
    frame_state FrameState = {};
    CTK_ITERATE(FrameCount)
    {
        frame *Frame = ctk::push(&FrameState.Frames);
        Frame->ImageAquiredSemaphore = create_semaphore(LogicalDevice);
        Frame->RenderFinishedSemaphore = create_semaphore(LogicalDevice);
        Frame->InFlightFence = create_fence(LogicalDevice);
    }
    CTK_ITERATE(SwapchainImageCount)
    {
        ctk::push(&FrameState.PreviousFrameInFlightFences, (VkFence)VK_NULL_HANDLE);
    }
    return FrameState;
}

static void
write_to_host_region(VkDevice LogicalDevice, region *Region, void *Data, VkDeviceSize Size, VkDeviceSize OffsetIntoRegion)
{
    if(OffsetIntoRegion + Size > Region->Size)
    {
        CTK_FATAL("cannot write %u bytes at offset %u into region (size=%u)", Size, OffsetIntoRegion, Region->Size);
    }
    void *MappedMemory = NULL;
    validate_vk_result(vkMapMemory(LogicalDevice, Region->Buffer->Memory, Region->Offset + OffsetIntoRegion, Size, 0, &MappedMemory),
                       "vkMapMemory", "failed to map host coherent buffer to host memory");
    memcpy(MappedMemory, Data, Size);
    vkUnmapMemory(LogicalDevice, Region->Buffer->Memory);
}

static void
write_to_device_region(device *Device, VkCommandPool CommandPool, region *StagingRegion, region *Region,
                       void *Data, VkDeviceSize Size, VkDeviceSize OffsetIntoRegion)
{
    if(OffsetIntoRegion + Size > Region->Size)
    {
        CTK_FATAL("cannot write %u bytes at offset %u into region (size=%u)", Size, OffsetIntoRegion, Region->Size);
    }
    write_to_host_region(Device->Logical, StagingRegion, Data, Size, 0);
    VkBufferCopy BufferCopyRegions[1] = {};
    BufferCopyRegions[0].srcOffset = StagingRegion->Offset;
    BufferCopyRegions[0].dstOffset = Region->Offset + OffsetIntoRegion;
    BufferCopyRegions[0].size = Size;
    VkCommandBuffer CommandBuffer = begin_one_time_command_buffer(Device->Logical, CommandPool);
        vkCmdCopyBuffer(CommandBuffer, StagingRegion->Buffer->Handle, Region->Buffer->Handle,
                        CTK_ARRAY_COUNT(BufferCopyRegions), BufferCopyRegions);
    submit_one_time_command_buffer(Device->Logical, Device->GraphicsQueue, CommandPool, CommandBuffer);
}

static VkFormat
find_depth_image_format(VkPhysicalDevice PhysicalDevice)
{
    static VkFormat const DEPTH_IMAGE_FORMATS[] =
    {
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM
    };
    static VkFormatFeatureFlags const DEPTH_IMG_FORMAT_FEATURES = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
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
image_memory_barrier(VkCommandBuffer CommandBuffer, VkImage Image, VkImageAspectFlags AspectMask,
                     image_memory_access_mask AccessMask,
                     image_memory_image_layout ImageLayout,
                     image_memory_pipeline_stage_mask PipelineStageMask)
{
    VkImageMemoryBarrier ImageMemoryBarrier = {};
    ImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    ImageMemoryBarrier.srcAccessMask = AccessMask.Source;
    ImageMemoryBarrier.dstAccessMask = AccessMask.Destination;
    ImageMemoryBarrier.oldLayout = ImageLayout.Source;
    ImageMemoryBarrier.newLayout = ImageLayout.Destination;
    ImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    ImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    ImageMemoryBarrier.image = Image;
    ImageMemoryBarrier.subresourceRange.aspectMask = AspectMask;
    ImageMemoryBarrier.subresourceRange.baseMipLevel = 0;
    ImageMemoryBarrier.subresourceRange.levelCount = 1;
    ImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
    ImageMemoryBarrier.subresourceRange.layerCount = 1;
    vkCmdPipelineBarrier(CommandBuffer,
                         PipelineStageMask.Source,
                         PipelineStageMask.Destination,
                         0, // Dependency Flags
                         0, NULL, // Memory Barriers
                         0, NULL, // Buffer Memory Barriers
                         1, &ImageMemoryBarrier); // Image Memory Barriers
}

static void
transition_image_layout(device *Device, VkCommandPool CommandPool, image *Image, VkImageLayout OldLayout, VkImageLayout NewLayout)
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
    else if(OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
    {
        SourceAccessMask = 0;
        DestinationAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
        SourcePipelineStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        DestinationPipelineStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
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

    VkCommandBuffer CommandBuffer = begin_one_time_command_buffer(Device->Logical, CommandPool);
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
    submit_one_time_command_buffer(Device->Logical, Device->GraphicsQueue, CommandPool, CommandBuffer);
    Image->Layout = NewLayout;
}

static uniform_buffer
create_uniform_buffer(buffer *Buffer, device *Device, VkDeviceSize ElementCount, VkDeviceSize ElementSize, u32 InstanceCount)
{
    CTK_ASSERT(InstanceCount > 0)
    uniform_buffer UniformBuffer = {};
    UniformBuffer.ElementSize = ElementSize;
    CTK_ITERATE(InstanceCount)
    {
        ctk::push(&UniformBuffer.Regions, allocate_region(Buffer, ElementCount * ElementSize,
                                                          Device->Properties.limits.minUniformBufferOffsetAlignment));
    }
    return UniformBuffer;
}

static VkDescriptorPool
create_descriptor_pool(VkDevice LogicalDevice, descriptor_set_info *DescriptorSetInfos, u32 DescriptorSetInfoCount)
{
    // Calculate total count of descriptors of each type for all descriptor sets.
    ctk::sarray<VkDescriptorPoolSize, 4> DescriptorPoolSizes = {};
    for(u32 DescriptorSetIndex = 0; DescriptorSetIndex < DescriptorSetInfoCount; ++DescriptorSetIndex)
    {
        descriptor_set_info *DescriptorSetInfo = DescriptorSetInfos + DescriptorSetIndex;
        for(u32 DescriptorIndex = 0; DescriptorIndex < DescriptorSetInfo->DescriptorBindings.Count; ++DescriptorIndex)
        {
            descriptor_info *DescriptorInfo = DescriptorSetInfo->DescriptorBindings[DescriptorIndex].Info;
            VkDescriptorPoolSize *SelectedDescriptorPoolSize = NULL;
            for(u32 DescriptorPoolSizeIndex = 0; DescriptorPoolSizeIndex < DescriptorPoolSizes.Count; ++DescriptorPoolSizeIndex)
            {
                VkDescriptorPoolSize *DescriptorPoolSize = DescriptorPoolSizes + DescriptorPoolSizeIndex;
                if(DescriptorPoolSize->type == DescriptorInfo->Type)
                {
                    SelectedDescriptorPoolSize = DescriptorPoolSize;
                    break;
                }
            }
            if(SelectedDescriptorPoolSize == NULL)
            {
                SelectedDescriptorPoolSize = ctk::push(&DescriptorPoolSizes);
                SelectedDescriptorPoolSize->type = DescriptorInfo->Type;
            }
            SelectedDescriptorPoolSize->descriptorCount += DescriptorInfo->Count * DescriptorSetInfo->InstanceCount;
        }
    }

    // Calculate total descriptor set count.
    u32 TotalDescriptorSetCount = 0;
    for(u32 DescriptorSetIndex = 0; DescriptorSetIndex < DescriptorSetInfoCount; ++DescriptorSetIndex)
    {
        TotalDescriptorSetCount += DescriptorSetInfos[DescriptorSetIndex].InstanceCount;
    }

    VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo = {};
    DescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    DescriptorPoolCreateInfo.flags = 0;
    DescriptorPoolCreateInfo.maxSets = TotalDescriptorSetCount;
    DescriptorPoolCreateInfo.poolSizeCount = DescriptorPoolSizes.Count;
    DescriptorPoolCreateInfo.pPoolSizes = DescriptorPoolSizes.Data;
    VkDescriptorPool DescriptorPool = VK_NULL_HANDLE;
    validate_vk_result(vkCreateDescriptorPool(LogicalDevice, &DescriptorPoolCreateInfo, NULL, &DescriptorPool),
                       "vkCreateDescriptorPool", "failed to create descriptor pool");
    return DescriptorPool;
}

static void
create_descriptor_sets(VkDevice LogicalDevice, VkDescriptorPool DescriptorPool,
                       descriptor_set_info *DescriptorSetInfos, u32 DescriptorSetInfoCount,
                       descriptor_set *DescriptorSets)
{
    for(u32 DescriptorSetIndex = 0; DescriptorSetIndex < DescriptorSetInfoCount; ++DescriptorSetIndex)
    {
        descriptor_set_info *DescriptorSetInfo = DescriptorSetInfos + DescriptorSetIndex;
        descriptor_set *DescriptorSet = DescriptorSets + DescriptorSetIndex;

        // Require (1) 1:1 uniform buffer region to descriptor set instance for uniform buffers that need to be written to; or (2) 1 uniform
        // buffer region to be shared between all descriptor set instances for uniform buffers that are read-only.
        for(u32 DescriptorIndex = 0; DescriptorIndex < DescriptorSetInfo->DescriptorBindings.Count; ++DescriptorIndex)
        {
            descriptor_info *DescriptorInfo = DescriptorSetInfo->DescriptorBindings[DescriptorIndex].Info;
            b32 DescriptorIsUniformBuffer = DescriptorInfo->Type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ||
                                            DescriptorInfo->Type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            if(DescriptorIsUniformBuffer &&
               DescriptorInfo->UniformBuffer->Regions.Count != DescriptorSetInfo->InstanceCount &&
               DescriptorInfo->UniformBuffer->Regions.Count != 1)
            {
                CTK_FATAL("uniform buffer region count: %u; descriptor set instance count: %u; uniform buffer must either have 1 region per "
                          "descriptor set instance or only 1 region, which will be shared between all descriptor set instances.",
                          DescriptorInfo->UniformBuffer->Regions.Count,
                          DescriptorSetInfo->InstanceCount)
            }
        }

        // Cache dynamic offsets for dynamic uniform buffer descriptors.
        for(u32 DescriptorIndex = 0; DescriptorIndex < DescriptorSetInfo->DescriptorBindings.Count; ++DescriptorIndex)
        {
            descriptor_info *DescriptorInfo = DescriptorSetInfo->DescriptorBindings[DescriptorIndex].Info;
            if(DescriptorInfo->Type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)
            {
                ctk::push(&DescriptorSet->DynamicOffsets, DescriptorInfo->UniformBuffer->ElementSize);
            }
        }

        // Create descriptor set layout.
        ctk::sarray<VkDescriptorSetLayoutBinding, 4> DescriptorSetLayoutBindings = {};
        for(u32 DescriptorIndex = 0; DescriptorIndex < DescriptorSetInfo->DescriptorBindings.Count; ++DescriptorIndex)
        {
            descriptor_binding *DescriptorBinding = DescriptorSetInfo->DescriptorBindings + DescriptorIndex;
            VkDescriptorSetLayoutBinding *DescriptorSetLayoutBinding = ctk::push(&DescriptorSetLayoutBindings);
            DescriptorSetLayoutBinding->binding = DescriptorBinding->Index;
            DescriptorSetLayoutBinding->descriptorType = DescriptorBinding->Info->Type;
            DescriptorSetLayoutBinding->descriptorCount = DescriptorBinding->Info->Count;
            DescriptorSetLayoutBinding->stageFlags = DescriptorBinding->Info->ShaderStageFlags;
            DescriptorSetLayoutBinding->pImmutableSamplers = NULL;
        }

        VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo = {};
        DescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        DescriptorSetLayoutCreateInfo.bindingCount = DescriptorSetLayoutBindings.Count;
        DescriptorSetLayoutCreateInfo.pBindings = DescriptorSetLayoutBindings.Data;
        validate_vk_result(vkCreateDescriptorSetLayout(LogicalDevice, &DescriptorSetLayoutCreateInfo, NULL, &DescriptorSet->Layout),
                           "vkCreateDescriptorSetLayout", "error creating descriptor set layout");

        // Duplicate layout for each instance.
        ctk::sarray<VkDescriptorSetLayout, 4> DescriptorSetLayouts = {};
        CTK_ITERATE(DescriptorSetInfo->InstanceCount)
        {
            ctk::push(&DescriptorSetLayouts, DescriptorSet->Layout);
        }

        // Allocate descriptor set instances.
        VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo = {};
        DescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        DescriptorSetAllocateInfo.descriptorPool = DescriptorPool;
        DescriptorSetAllocateInfo.descriptorSetCount = DescriptorSetLayouts.Count;
        DescriptorSetAllocateInfo.pSetLayouts = DescriptorSetLayouts.Data;
        validate_vk_result(vkAllocateDescriptorSets(LogicalDevice, &DescriptorSetAllocateInfo, DescriptorSet->Instances.Data),
                           "vkAllocateDescriptorSets", "failed to allocate descriptor sets");
        DescriptorSet->Instances.Count = DescriptorSetLayouts.Count;

        // Update descriptor set instances with their associated data.
        ctk::sarray<VkDescriptorBufferInfo, 6> DescriptorBufferInfos = {};
        ctk::sarray<VkDescriptorImageInfo, 6> DescriptorImageInfos = {};
        ctk::sarray<VkWriteDescriptorSet, 6> WriteDescriptorSets = {};
        for(u32 InstanceIndex = 0; InstanceIndex < DescriptorSet->Instances.Count; ++InstanceIndex)
        {
            for(u32 DescriptorIndex = 0; DescriptorIndex < DescriptorSetInfo->DescriptorBindings.Count; ++DescriptorIndex)
            {
                descriptor_binding *DescriptorBinding = DescriptorSetInfo->DescriptorBindings + DescriptorIndex;
                descriptor_info *DescriptorInfo = DescriptorBinding->Info;
                VkWriteDescriptorSet *WriteDescriptorSet = ctk::push(&WriteDescriptorSets);
                WriteDescriptorSet->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                WriteDescriptorSet->dstSet = DescriptorSet->Instances[InstanceIndex];
                WriteDescriptorSet->dstBinding = DescriptorBinding->Index;
                WriteDescriptorSet->dstArrayElement = 0;
                WriteDescriptorSet->descriptorCount = DescriptorInfo->Count;
                WriteDescriptorSet->descriptorType = DescriptorInfo->Type;

                if(WriteDescriptorSet->descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ||
                   WriteDescriptorSet->descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)
                {
                    uniform_buffer *UniformBuffer = DescriptorInfo->UniformBuffer;

                    // If there is a uniform buffer region for every descriptor set instance, bind each to its respective instance; else
                    // bind the only uniform buffer region to each descriptor set instance.
                    u32 UniformBufferRegionIndex = UniformBuffer->Regions.Count == DescriptorSet->Instances.Count ? InstanceIndex : 0;

                    region *UniformBufferRegion = ctk::at(&UniformBuffer->Regions, UniformBufferRegionIndex);
                    WriteDescriptorSet->pBufferInfo = DescriptorBufferInfos.Data + DescriptorBufferInfos.Count;
                    for(u32 DescriptorElementIndex = 0; DescriptorElementIndex < DescriptorInfo->Count; ++DescriptorElementIndex)
                    {
                        VkDescriptorBufferInfo *DescriptorBufferInfo = ctk::push(&DescriptorBufferInfos);
                        DescriptorBufferInfo->buffer = UniformBufferRegion->Buffer->Handle;

                        // Map each element in descriptor to its UniformBuffer->ElementSize offset in uniform buffer region.
                        DescriptorBufferInfo->offset = UniformBufferRegion->Offset + (UniformBuffer->ElementSize * DescriptorElementIndex);

                        DescriptorBufferInfo->range = UniformBuffer->ElementSize;
                    }
                }
                else if(WriteDescriptorSet->descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
                {
                    texture *Texture = DescriptorInfo->Texture;
                    VkDescriptorImageInfo *DescriptorImageInfo = ctk::push(&DescriptorImageInfos);
                    DescriptorImageInfo->sampler = Texture->Sampler;
                    DescriptorImageInfo->imageView = Texture->Image.View;
                    DescriptorImageInfo->imageLayout = Texture->Image.Layout;
                    WriteDescriptorSet->pImageInfo = DescriptorImageInfo;
                }
                else
                {
                    CTK_FATAL("unhandled descriptor type when updating descriptor sets")
                }
            }
        }
        vkUpdateDescriptorSets(LogicalDevice, WriteDescriptorSets.Count, WriteDescriptorSets.Data, 0, NULL);
    }
}

static void
bind_descriptor_sets(VkCommandBuffer CommandBuffer, VkPipelineLayout GraphicsPipelineLayout, u32 FirstDescriptorSetIndex,
                     descriptor_set **DescriptorSets, u32 DescriptorSetCount, u32 InstanceIndex = 0, u32 DynamicIndex = 0)
{
    ctk::sarray<VkDescriptorSet, 4> DescriptorSetsToBind = {};
    ctk::sarray<u32, 16> DynamicOffsets = {};
    for(u32 DescriptorSetIndex = 0; DescriptorSetIndex < DescriptorSetCount; ++DescriptorSetIndex)
    {
        descriptor_set *DescriptorSet = DescriptorSets[DescriptorSetIndex];
        u32 DescriptorSetInstanceIndex = DescriptorSet->Instances.Count > 1 ? InstanceIndex : 0;
        ctk::push(&DescriptorSetsToBind, DescriptorSet->Instances[DescriptorSetInstanceIndex]);
        for(u32 DynamicOffsetIndex = 0; DynamicOffsetIndex < DescriptorSet->DynamicOffsets.Count; ++DynamicOffsetIndex)
        {
            ctk::push(&DynamicOffsets, DescriptorSet->DynamicOffsets[DynamicOffsetIndex] * DynamicIndex);
        }
    }
    vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipelineLayout,
                            FirstDescriptorSetIndex, // First Set Number
                            DescriptorSetsToBind.Count,
                            DescriptorSetsToBind.Data, // Sets to be bound to [first .. first + count]
                            DynamicOffsets.Count,
                            DynamicOffsets.Data);
}

static VkDescriptorType
to_vk_descriptor_type(cstr Name)
{
    static ctk::pair<cstr, VkDescriptorType> DESCRIPTOR_TYPES[] =
    {
        CTK_NAME_VALUE_PAIR(VK_DESCRIPTOR_TYPE_SAMPLER),
        CTK_NAME_VALUE_PAIR(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER),
        CTK_NAME_VALUE_PAIR(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE),
        CTK_NAME_VALUE_PAIR(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE),
        CTK_NAME_VALUE_PAIR(VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER),
        CTK_NAME_VALUE_PAIR(VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER),
        CTK_NAME_VALUE_PAIR(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER),
        CTK_NAME_VALUE_PAIR(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER),
        CTK_NAME_VALUE_PAIR(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC),
        CTK_NAME_VALUE_PAIR(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC),
        CTK_NAME_VALUE_PAIR(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT),
        CTK_NAME_VALUE_PAIR(VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT),
        // CTK_NAME_VALUE_PAIR(VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR),
        CTK_NAME_VALUE_PAIR(VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV),
    };
    auto DescriptorType = ctk::find_pair(DESCRIPTOR_TYPES, CTK_ARRAY_COUNT(DESCRIPTOR_TYPES), Name, ctk::equal);
    if(!DescriptorType)
    {
        CTK_FATAL("failed to find VkDescriptorType type for \"%s\"", Name)
    }
    return DescriptorType->Value;
}

static VkShaderStageFlagBits
to_vk_shader_stage_flag_bits(cstr Name)
{
    static ctk::pair<cstr, VkShaderStageFlagBits> SHADER_STAGES[] =
    {
        CTK_NAME_VALUE_PAIR(VK_SHADER_STAGE_VERTEX_BIT),
        CTK_NAME_VALUE_PAIR(VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT),
        CTK_NAME_VALUE_PAIR(VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT),
        CTK_NAME_VALUE_PAIR(VK_SHADER_STAGE_GEOMETRY_BIT),
        CTK_NAME_VALUE_PAIR(VK_SHADER_STAGE_FRAGMENT_BIT),
        CTK_NAME_VALUE_PAIR(VK_SHADER_STAGE_COMPUTE_BIT),
        CTK_NAME_VALUE_PAIR(VK_SHADER_STAGE_ALL_GRAPHICS),
        CTK_NAME_VALUE_PAIR(VK_SHADER_STAGE_ALL),
        // CTK_NAME_VALUE_PAIR(VK_SHADER_STAGE_RAYGEN_BIT_KHR),
        // CTK_NAME_VALUE_PAIR(VK_SHADER_STAGE_ANY_HIT_BIT_KHR),
        // CTK_NAME_VALUE_PAIR(VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR),
        // CTK_NAME_VALUE_PAIR(VK_SHADER_STAGE_MISS_BIT_KHR),
        // CTK_NAME_VALUE_PAIR(VK_SHADER_STAGE_INTERSECTION_BIT_KHR),
        // CTK_NAME_VALUE_PAIR(VK_SHADER_STAGE_CALLABLE_BIT_KHR),
        CTK_NAME_VALUE_PAIR(VK_SHADER_STAGE_TASK_BIT_NV),
        CTK_NAME_VALUE_PAIR(VK_SHADER_STAGE_MESH_BIT_NV),
        CTK_NAME_VALUE_PAIR(VK_SHADER_STAGE_RAYGEN_BIT_NV),
        CTK_NAME_VALUE_PAIR(VK_SHADER_STAGE_ANY_HIT_BIT_NV),
        CTK_NAME_VALUE_PAIR(VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV),
        CTK_NAME_VALUE_PAIR(VK_SHADER_STAGE_MISS_BIT_NV),
        CTK_NAME_VALUE_PAIR(VK_SHADER_STAGE_INTERSECTION_BIT_NV),
        CTK_NAME_VALUE_PAIR(VK_SHADER_STAGE_CALLABLE_BIT_NV),
    };
    auto ShaderStage = ctk::find_pair(SHADER_STAGES, CTK_ARRAY_COUNT(SHADER_STAGES), Name, ctk::equal);
    if(!ShaderStage)
    {
        CTK_FATAL("failed to find VkShaderStageFlagBits for \"%s\"", Name)
    }
    return ShaderStage->Value;
}

static VkBool32
to_vk_bool_32(cstr Name)
{
    static ctk::pair<cstr, VkBool32> BOOLS[] =
    {
        CTK_NAME_VALUE_PAIR(VK_TRUE),
        CTK_NAME_VALUE_PAIR(VK_FALSE),
    };
    auto VkBool = ctk::find_pair(BOOLS, CTK_ARRAY_COUNT(BOOLS), Name, ctk::equal);
    if(!VkBool)
    {
        CTK_FATAL("failed to find VkBool32 for \"%s\"", Name)
    }
    return VkBool->Value;
}

static VkPrimitiveTopology
to_vk_primitive_topology(cstr Name)
{
    static ctk::pair<cstr, VkPrimitiveTopology> PRIMITIVE_TOPOLOGIES[] =
    {
        CTK_NAME_VALUE_PAIR(VK_PRIMITIVE_TOPOLOGY_POINT_LIST),
        CTK_NAME_VALUE_PAIR(VK_PRIMITIVE_TOPOLOGY_LINE_LIST),
        CTK_NAME_VALUE_PAIR(VK_PRIMITIVE_TOPOLOGY_LINE_STRIP),
        CTK_NAME_VALUE_PAIR(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST),
        CTK_NAME_VALUE_PAIR(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP),
        CTK_NAME_VALUE_PAIR(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN),
        CTK_NAME_VALUE_PAIR(VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY),
        CTK_NAME_VALUE_PAIR(VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY),
        CTK_NAME_VALUE_PAIR(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY),
        CTK_NAME_VALUE_PAIR(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY),
        CTK_NAME_VALUE_PAIR(VK_PRIMITIVE_TOPOLOGY_PATCH_LIST),
    };
    auto PrimitiveTopology = ctk::find_pair(PRIMITIVE_TOPOLOGIES, CTK_ARRAY_COUNT(PRIMITIVE_TOPOLOGIES), Name, ctk::equal);
    if(!PrimitiveTopology)
    {
        CTK_FATAL("failed to find VkPrimitiveTopology for \"%s\"", Name)
    }
    return PrimitiveTopology->Value;
}

static VkFilter
to_vk_filter(cstr Name)
{
    static ctk::pair<cstr, VkFilter> FILTERS[] =
    {
        CTK_NAME_VALUE_PAIR(VK_FILTER_NEAREST),
        CTK_NAME_VALUE_PAIR(VK_FILTER_LINEAR),
    };
    auto Filter = ctk::find_pair(FILTERS, CTK_ARRAY_COUNT(FILTERS), Name, ctk::equal);
    if(!Filter)
    {
        CTK_FATAL("failed to find VkFilter for \"%s\"", Name)
    }
    return Filter->Value;
}

static void
validate_vk_result(VkResult Result, cstr FunctionName, cstr FailureMessage)
{
    if(Result != VK_SUCCESS)
    {
        output_vk_result(Result, FunctionName);
        CTK_FATAL(FailureMessage)
    }
}

} // vtk
