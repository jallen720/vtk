#include "ctk/ctk_new.h"

#define _VTK_VK_RESULT_NAME(VK_RESULT) VK_RESULT, #VK_RESULT
#define VTK_LOAD_INSTANCE_EXTENSION_FUNCTION(INSTANCE, FUNC_NAME) \
    auto FUNC_NAME = (PFN_ ## FUNC_NAME)vkGetInstanceProcAddr(INSTANCE, #FUNC_NAME); \
    if (FUNC_NAME == NULL) \
        CTK_FATAL("failed to load instance extension function \"%s\"", #FUNC_NAME)

////////////////////////////////////////////////////////////
/// Debugging
////////////////////////////////////////////////////////////
struct vtk_vk_result_debug_info {
    VkResult result;
    cstr name;
    cstr message;
};

static void vtk_log_result(VkResult result) {
    static struct vtk_vk_result_debug_info VK_RESULT_DEBUG_INFOS[] = {
        { _VTK_VK_RESULT_NAME(VK_SUCCESS), "VULKAN SPEC ERROR MESSAGE: Command successfully completed." },
        { _VTK_VK_RESULT_NAME(VK_NOT_READY), "VULKAN SPEC ERROR MESSAGE: A fence or query has not yet completed." },
        { _VTK_VK_RESULT_NAME(VK_TIMEOUT), "VULKAN SPEC ERROR MESSAGE: A wait operation has not completed in the specified time." },
        { _VTK_VK_RESULT_NAME(VK_EVENT_SET), "VULKAN SPEC ERROR MESSAGE: An event is signaled." },
        { _VTK_VK_RESULT_NAME(VK_EVENT_RESET), "VULKAN SPEC ERROR MESSAGE: An event is unsignaled." },
        { _VTK_VK_RESULT_NAME(VK_INCOMPLETE), "VULKAN SPEC ERROR MESSAGE: A return array was too small for the result." },
        { _VTK_VK_RESULT_NAME(VK_SUBOPTIMAL_KHR), "VULKAN SPEC ERROR MESSAGE: A swapchain no longer matches the surface properties exactly, but can still be used to present to the surface successfully." },
        { _VTK_VK_RESULT_NAME(VK_ERROR_OUT_OF_HOST_MEMORY), "VULKAN SPEC ERROR MESSAGE: A host memory allocation has failed." },
        { _VTK_VK_RESULT_NAME(VK_ERROR_OUT_OF_DEVICE_MEMORY), "VULKAN SPEC ERROR MESSAGE: A device memory allocation has failed." },
        { _VTK_VK_RESULT_NAME(VK_ERROR_INITIALIZATION_FAILED), "VULKAN SPEC ERROR MESSAGE: Initialization of an object could not be completed for implementation-specific reasons." },
        { _VTK_VK_RESULT_NAME(VK_ERROR_DEVICE_LOST), "VULKAN SPEC ERROR MESSAGE: The logical or physical device has been lost." },
        { _VTK_VK_RESULT_NAME(VK_ERROR_MEMORY_MAP_FAILED), "VULKAN SPEC ERROR MESSAGE: Mapping of a memory object has failed." },
        { _VTK_VK_RESULT_NAME(VK_ERROR_LAYER_NOT_PRESENT), "VULKAN SPEC ERROR MESSAGE: A requested layer is not present or could not be loaded." },
        { _VTK_VK_RESULT_NAME(VK_ERROR_EXTENSION_NOT_PRESENT), "VULKAN SPEC ERROR MESSAGE: A requested extension is not supported." },
        { _VTK_VK_RESULT_NAME(VK_ERROR_FEATURE_NOT_PRESENT), "VULKAN SPEC ERROR MESSAGE: A requested feature is not supported." },
        { _VTK_VK_RESULT_NAME(VK_ERROR_INCOMPATIBLE_DRIVER), "VULKAN SPEC ERROR MESSAGE: The requested version of Vulkan is not supported by the driver or is otherwise incompatible for implementation-specific reasons." },
        { _VTK_VK_RESULT_NAME(VK_ERROR_TOO_MANY_OBJECTS), "VULKAN SPEC ERROR MESSAGE: Too many objects of the type have already been created." },
        { _VTK_VK_RESULT_NAME(VK_ERROR_FORMAT_NOT_SUPPORTED), "VULKAN SPEC ERROR MESSAGE: A requested format is not supported on this device." },
        { _VTK_VK_RESULT_NAME(VK_ERROR_FRAGMENTED_POOL), "VULKAN SPEC ERROR MESSAGE: A pool allocation has failed due to fragmentation of the pool’s memory. This must only be returned if no attempt to allocate host or device memory was made to accommodate the new allocation. This should be returned in preference to VK_ERROR_OUT_OF_POOL_MEMORY, but only if the implementation is certain that the pool allocation failure was due to fragmentation." },
        { _VTK_VK_RESULT_NAME(VK_ERROR_SURFACE_LOST_KHR), "VULKAN SPEC ERROR MESSAGE: A surface is no longer available." },
        { _VTK_VK_RESULT_NAME(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR), "VULKAN SPEC ERROR MESSAGE: The requested window is already in use by Vulkan or another API in a manner which prevents it from being used again." },
        { _VTK_VK_RESULT_NAME(VK_ERROR_OUT_OF_DATE_KHR), "VULKAN SPEC ERROR MESSAGE: A surface has changed in such a way that it is no longer compatible with the swapchain, and further presentation requests using the swapchain will fail. Applications must query the new surface properties and recreate their swapchain if they wish to continue presenting to the surface." },
        { _VTK_VK_RESULT_NAME(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR), "VULKAN SPEC ERROR MESSAGE: The display used by a swapchain does not use the same presentable image layout, or is incompatible in a way that prevents sharing an image." },
        { _VTK_VK_RESULT_NAME(VK_ERROR_INVALID_SHADER_NV), "VULKAN SPEC ERROR MESSAGE: One or more shaders failed to compile or link. More details are reported back to the application via https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html#VK_EXT_debug_report if enabled." },
        { _VTK_VK_RESULT_NAME(VK_ERROR_OUT_OF_POOL_MEMORY), "VULKAN SPEC ERROR MESSAGE: A pool memory allocation has failed. This must only be returned if no attempt to allocate host or device memory was made to accommodate the new allocation. If the failure was definitely due to fragmentation of the pool, VK_ERROR_FRAGMENTED_POOL should be returned instead." },
        { _VTK_VK_RESULT_NAME(VK_ERROR_INVALID_EXTERNAL_HANDLE), "VULKAN SPEC ERROR MESSAGE: An external handle is not a valid handle of the specified type." },
        // { _VTK_VK_RESULT_NAME(VK_ERROR_FRAGMENTATION), "VULKAN SPEC ERROR MESSAGE: A descriptor pool creation has failed due to fragmentation." },
        { _VTK_VK_RESULT_NAME(VK_ERROR_INVALID_DEVICE_ADDRESS_EXT), "VULKAN SPEC ERROR MESSAGE: A buffer creation failed because the requested address is not available." },
        // { _VTK_VK_RESULT_NAME(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS), "VULKAN SPEC ERROR MESSAGE: A buffer creation or memory allocation failed because the requested address is not available." },
        { _VTK_VK_RESULT_NAME(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT), "VULKAN SPEC ERROR MESSAGE: An operation on a swapchain created with VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT failed as it did not have exlusive full-screen access. This may occur due to implementation-dependent reasons, outside of the application’s control." },
        // { _VTK_VK_RESULT_NAME(VK_ERROR_UNKNOWN), "VULKAN SPEC ERROR MESSAGE: An unknown error has occurred; either the application has provided invalid input, or an implementation failure has occurred." },
    };
    struct vtk_vk_result_debug_info *info = NULL;
    for (u32 i = 0; i < CTK_ARRAY_COUNT(VK_RESULT_DEBUG_INFOS); ++i) {
        info = VK_RESULT_DEBUG_INFOS + i;
        if (info->result == result)
            break;
    }
    if (!info)
        CTK_FATAL("failed to find debug info for VkResult %d", result)

    if (info->result == 0)
        ctk_info("vulkan function returned %s: %s", info->name, info->message);
    else if (info->result > 0)
        ctk_warning("vulkan function returned %s: %s", info->name, info->message);
    else
        ctk_error("vulkan function returned %s: %s", info->name, info->message);
}

template<typename ..._args>
static void vtk_validate_result(VkResult result, cstr fail_msg, _args... args) {
    if (result != VK_SUCCESS) {
        vtk_log_result(result);
        CTK_FATAL(fail_msg, args...)
    }
}

template<typename vk_object, u32 size, typename _loader, typename ..._args>
static void vtk_load_vk_objects(struct ctk_array<vk_object, size> *arr, _loader loader, _args... args) {
    loader(args..., &arr->count, NULL);
    CTK_ASSERT(arr->count < arr->size)
    loader(args..., &arr->count, arr->data);
}

////////////////////////////////////////////////////////////
/// Vulkan State
////////////////////////////////////////////////////////////
struct vtk_instance {
    VkInstance handle;
    VkDebugUtilsMessengerEXT debug_messenger;
};

struct vtk_device {
    VkPhysicalDevice physical;
    VkDevice logical;
    struct {
        u32 graphics;
        u32 present;
    } queue_family_indexes;
    struct {
        VkQueue graphics;
        VkQueue present;
    } queues;
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceMemoryProperties memory_properties;
    VkFormat depth_image_format;
};

struct vtk_image_info {
    u32 width;
    u32 height;
    VkFormat format;
    VkImageTiling tiling;
    VkImageUsageFlags usage_flags;
    VkMemoryPropertyFlags memory_property_flags;
    VkImageAspectFlags aspect_mask;
};

struct vtk_swapchain {
    VkSwapchainKHR handle;
    struct ctk_array<VkImageView, 4> image_views;
    u32 image_count;
    VkFormat image_format;
    VkExtent2D extent;
};

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity_flag_bit,
                                                     VkDebugUtilsMessageTypeFlagsEXT msg_type_flags,
                                                     VkDebugUtilsMessengerCallbackDataEXT const *cb_data, void *user_data) {
    cstr msg_id = cb_data->pMessageIdName ? cb_data->pMessageIdName : "";
    if (VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT & msg_severity_flag_bit)
        CTK_FATAL("VALIDATION LAYER [%s]: %s\n", msg_id, cb_data->pMessage)
    else if (VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT & msg_severity_flag_bit)
        ctk_warning("VALIDATION LAYER [%s]: %s\n", msg_id, cb_data->pMessage);
    else
        ctk_info("VALIDATION LAYER [%s]: %s\n", msg_id, cb_data->pMessage);
    return VK_FALSE;
}

static struct vtk_instance vtk_create_instance() {
    struct vtk_instance instance = {};

    VkDebugUtilsMessengerCreateInfoEXT debug_messenger_info = {};
    debug_messenger_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_messenger_info.pNext = NULL;
    debug_messenger_info.flags = 0;
    debug_messenger_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                           // VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_messenger_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                       VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_messenger_info.pfnUserCallback = debug_callback;
    debug_messenger_info.pUserData = NULL;

    // Create Instance
    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext = NULL;
    app_info.pApplicationName = "test";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "test";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    struct ctk_array<cstr, 8> extensions = {};
    struct ctk_array<cstr, 8> layers = {};
    cstr *glfw_extensions = glfwGetRequiredInstanceExtensions(&extensions.count);
    memcpy(extensions.data, glfw_extensions, ctk_byte_count(&extensions));
    ctk_push(&extensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    ctk_push(&layers, "VK_LAYER_LUNARG_standard_validation");

    VkInstanceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pNext = &debug_messenger_info;
    info.flags = 0;
    info.pApplicationInfo = &app_info;
    info.enabledLayerCount = layers.count;
    info.ppEnabledLayerNames = layers.data;
    info.enabledExtensionCount = extensions.count;
    info.ppEnabledExtensionNames = extensions.data;
    vtk_validate_result(vkCreateInstance(&info, NULL, &instance.handle), "failed to create Vulkan instance");

    // Create Debug Messenger
    VTK_LOAD_INSTANCE_EXTENSION_FUNCTION(instance.handle, vkCreateDebugUtilsMessengerEXT)
    vtk_validate_result(vkCreateDebugUtilsMessengerEXT(instance.handle, &debug_messenger_info, NULL, &instance.debug_messenger),
                        "failed to create debug messenger");

    return instance;
}

static VkDeviceQueueCreateInfo vtk_default_queue_info(u32 q_family_idx) {
    static f32 const Q_PRIORITIES[] = { 1.0f };

    VkDeviceQueueCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    info.flags = 0;
    info.queueFamilyIndex = q_family_idx;
    info.queueCount = CTK_ARRAY_COUNT(Q_PRIORITIES);
    info.pQueuePriorities = Q_PRIORITIES;

    return info;
}

static VkFormat vtk_find_depth_image_format(VkPhysicalDevice physical_device) {
    static VkFormat const DEPTH_IMAGE_FORMATS[] = {
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM
    };
    static VkFormatFeatureFlags const DEPTH_IMG_FMT_FEATS = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    for (u32 i = 0; i < CTK_ARRAY_COUNT(DEPTH_IMAGE_FORMATS); i++) {
        VkFormat depth_img_fmt = DEPTH_IMAGE_FORMATS[i];
        VkFormatProperties depth_img_fmt_props = {};
        vkGetPhysicalDeviceFormatProperties(physical_device, depth_img_fmt, &depth_img_fmt_props);
        if ((depth_img_fmt_props.optimalTilingFeatures & DEPTH_IMG_FMT_FEATS) == DEPTH_IMG_FMT_FEATS)
            return depth_img_fmt;
    }
    CTK_FATAL("failed to find format that satisfies feature requirements for depth image")
}

static struct vtk_device vtk_create_device(VkInstance instance, VkSurfaceKHR surface, VkPhysicalDeviceFeatures *features) {
    struct vtk_device device = {};

    ////////////////////////////////////////////////////////////
    /// Physical
    ////////////////////////////////////////////////////////////
    struct ctk_array<VkPhysicalDevice, 8> phys_devices = {};
    vtk_load_vk_objects(&phys_devices, vkEnumeratePhysicalDevices, instance);
    device.physical = phys_devices[0];

    // Find queue family indexes.
    struct ctk_array<VkQueueFamilyProperties, 8> q_family_props_arr = {};
    vtk_load_vk_objects(&q_family_props_arr, vkGetPhysicalDeviceQueueFamilyProperties, device.physical);
    for (u32 i = 0; i < q_family_props_arr.count; ++i) {
        VkQueueFamilyProperties *q_family_props = q_family_props_arr + i;
        if (q_family_props->queueFlags & VK_QUEUE_GRAPHICS_BIT)
            device.queue_family_indexes.graphics = i;
        VkBool32 present_supported = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device.physical, i, surface, &present_supported);
        if (present_supported == VK_TRUE)
            device.queue_family_indexes.present = i;
    }

    vkGetPhysicalDeviceProperties(device.physical, &device.properties);
    vkGetPhysicalDeviceMemoryProperties(device.physical, &device.memory_properties);
    device.depth_image_format = vtk_find_depth_image_format(device.physical);

    ////////////////////////////////////////////////////////////
    /// Logical
    ////////////////////////////////////////////////////////////
    struct ctk_array<VkDeviceQueueCreateInfo, 2> q_infos = {};
    ctk_push(&q_infos, vtk_default_queue_info(device.queue_family_indexes.graphics));
    if (device.queue_family_indexes.present != device.queue_family_indexes.graphics)
        ctk_push(&q_infos, vtk_default_queue_info(device.queue_family_indexes.present));

    cstr extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    VkDeviceCreateInfo logical_device_info = {};
    logical_device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    logical_device_info.flags = 0;
    logical_device_info.queueCreateInfoCount = q_infos.count;
    logical_device_info.pQueueCreateInfos = q_infos.data;
    logical_device_info.enabledLayerCount = 0;
    logical_device_info.ppEnabledLayerNames = NULL;
    logical_device_info.enabledExtensionCount = CTK_ARRAY_COUNT(extensions);
    logical_device_info.ppEnabledExtensionNames = extensions;
    logical_device_info.pEnabledFeatures = features;
    vtk_validate_result(vkCreateDevice(device.physical, &logical_device_info, NULL, &device.logical), "failed to create logical device");

    // Get logical device queues.
    vkGetDeviceQueue(device.logical, device.queue_family_indexes.graphics, 0, &device.queues.graphics);
    vkGetDeviceQueue(device.logical, device.queue_family_indexes.present, 0, &device.queues.present);

    return device;
}

static struct vtk_swapchain vtk_create_swapchain(struct vtk_device *device, VkSurfaceKHR surface) {
    struct vtk_swapchain swapchain = {};

    ////////////////////////////////////////////////////////////
    /// Configuration
    ////////////////////////////////////////////////////////////

    // Configure swapchain based on surface properties.
    struct ctk_array<VkSurfaceFormatKHR, 16> surface_fmts = {};
    struct ctk_array<VkPresentModeKHR, 8> surface_present_modes = {};
    VkSurfaceCapabilitiesKHR surface_capabilities = {};
    vtk_load_vk_objects(&surface_fmts, vkGetPhysicalDeviceSurfaceFormatsKHR, device->physical, surface);
    vtk_load_vk_objects(&surface_present_modes, vkGetPhysicalDeviceSurfacePresentModesKHR, device->physical, surface);
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->physical, surface, &surface_capabilities);

    // Default to first surface format.
    VkSurfaceFormatKHR selected_fmt = surface_fmts[0];
    for (u32 i = 0; i < surface_fmts.count; ++i) {
        VkSurfaceFormatKHR surface_fmt = surface_fmts[i];

        // Prefer 4-component 8-bit BGRA unnormalized format and sRGB color space.
        if (surface_fmt.format == VK_FORMAT_B8G8R8A8_UNORM && surface_fmt.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            selected_fmt = surface_fmt;
            break;
        }
    }

    // Default to FIFO (only present mode with guarenteed availability).
    VkPresentModeKHR selected_present_mode = VK_PRESENT_MODE_FIFO_KHR;
    for (u32 i = 0; i < surface_present_modes.count; ++i) {
        VkPresentModeKHR surface_present_mode = surface_present_modes[i];

        // Mailbox is the preferred present mode if available.
        if (surface_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            selected_present_mode = surface_present_mode;
            break;
        }
    }

    // Set image count to min image count + 1 or max image count (whichever is smaller).
    u32 selected_image_count = surface_capabilities.minImageCount + 1;
    if (surface_capabilities.maxImageCount > 0 && selected_image_count > surface_capabilities.maxImageCount)
        selected_image_count = surface_capabilities.maxImageCount;

    // Verify current extent has been set for surface.
    if (surface_capabilities.currentExtent.width == UINT32_MAX)
        CTK_FATAL("current extent not set for surface")

    ////////////////////////////////////////////////////////////
    /// Creation
    ////////////////////////////////////////////////////////////
    u32 graphics_q_family_idx = device->queue_family_indexes.graphics;
    u32 present_q_family_idx = device->queue_family_indexes.present;
    u32 q_family_idxs[] = { graphics_q_family_idx, present_q_family_idx };

    VkSwapchainCreateInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.surface = surface;
    info.flags = 0;
    info.minImageCount = selected_image_count;
    info.imageFormat = selected_fmt.format;
    info.imageColorSpace = selected_fmt.colorSpace;
    info.imageExtent = surface_capabilities.currentExtent;
    info.imageArrayLayers = 1; // Always 1 for standard images.
    info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    info.preTransform = surface_capabilities.currentTransform;
    info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    info.presentMode = selected_present_mode;
    info.clipped = VK_TRUE;
    info.oldSwapchain = VK_NULL_HANDLE;
    if (graphics_q_family_idx != present_q_family_idx) {
        info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        info.queueFamilyIndexCount = CTK_ARRAY_COUNT(q_family_idxs);
        info.pQueueFamilyIndices = q_family_idxs;
    } else {
        info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        info.queueFamilyIndexCount = 0;
        info.pQueueFamilyIndices = NULL;
    }
    vtk_validate_result(vkCreateSwapchainKHR(device->logical, &info, NULL, &swapchain.handle), "failed to create swapchain");

    // Store surface state used to create swapchain for future reference.
    swapchain.image_format = selected_fmt.format;
    swapchain.extent = surface_capabilities.currentExtent;

    ////////////////////////////////////////////////////////////
    /// Image View Creation
    ////////////////////////////////////////////////////////////
    struct ctk_array<VkImage, 4> swapchain_images = {};
    vtk_load_vk_objects(&swapchain_images, vkGetSwapchainImagesKHR, device->logical, swapchain.handle);
    swapchain.image_count = swapchain_images.count;
    for (u32 i = 0; i < swapchain_images.count; ++i) {
        VkImageViewCreateInfo view_info = {};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = swapchain_images[i];
        view_info.flags = 0;
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = swapchain.image_format;
        view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;
        vtk_validate_result(vkCreateImageView(device->logical, &view_info, NULL, swapchain.image_views + i), "failed to create image view");
    }

    return swapchain;
}

////////////////////////////////////////////////////////////
/// Memory
////////////////////////////////////////////////////////////
struct vtk_buffer_info {
    VkDeviceSize size;
    VkBufferUsageFlags usage_flags;
    VkMemoryPropertyFlags memory_property_flags;
    VkSharingMode sharing_mode;
};

struct vtk_buffer {
    VkBuffer handle;
    VkDeviceMemory memory;
    VkDeviceSize size;
    VkDeviceSize end;
};

struct vtk_region {
    struct vtk_buffer *buffer;
    VkDeviceSize size;
    VkDeviceSize offset;
};

static VkDeviceMemory vtk_allocate_device_memory(struct vtk_device *device, VkMemoryRequirements *mem_reqs,
                                                 VkMemoryPropertyFlags mem_prop_flags) {
    // Find memory type index from device based on memory property flags.
    u32 selected_mem_type_idx = CTK_U32_MAX;
    for (u32 mem_type_idx = 0; mem_type_idx < device->memory_properties.memoryTypeCount; ++mem_type_idx) {
        // Ensure index refers to memory type from memory requirements.
        if (!(mem_reqs->memoryTypeBits & (1 << mem_type_idx)))
            continue;

        // Check if memory at index has correct properties.
        if ((device->memory_properties.memoryTypes[mem_type_idx].propertyFlags & mem_prop_flags) == mem_prop_flags) {
            selected_mem_type_idx = mem_type_idx;
            break;
        }
    }
    if (selected_mem_type_idx == CTK_U32_MAX)
        CTK_FATAL("failed to find memory type that satisfies property requirements")

    // Allocate memory
    VkMemoryAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    info.allocationSize = mem_reqs->size;
    info.memoryTypeIndex = selected_mem_type_idx;
    VkDeviceMemory mem = VK_NULL_HANDLE;
    vtk_validate_result(vkAllocateMemory(device->logical, &info, NULL, &mem), "failed to allocate memory");
    return mem;
}

static struct vtk_buffer vtk_create_buffer(struct vtk_device *device, struct vtk_buffer_info *buf_info) {
    struct vtk_buffer buf = {};
    buf.size = buf_info->size;

    VkBufferCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.size = buf_info->size;
    info.usage = buf_info->usage_flags;
    info.sharingMode = buf_info->sharing_mode;
    info.queueFamilyIndexCount = 0;
    info.pQueueFamilyIndices = NULL; // Ignored if sharingMode is not VK_SHARING_MODE_CONCURRENT.
    vtk_validate_result(vkCreateBuffer(device->logical, &info, NULL, &buf.handle), "failed to create buffer");

    // Allocate / Bind Memory
    VkMemoryRequirements mem_reqs = {};
    vkGetBufferMemoryRequirements(device->logical, buf.handle, &mem_reqs);
    buf.memory = vtk_allocate_device_memory(device, &mem_reqs, buf_info->memory_property_flags);
    vtk_validate_result(vkBindBufferMemory(device->logical, buf.handle, buf.memory, 0), "failed to bind buffer memory");

    return buf;
}

static struct vtk_region vtk_allocate_region(struct vtk_buffer *buf, u32 size, VkDeviceSize align = 1) {
    struct vtk_region region = {};
    region.buffer = buf;
    VkDeviceSize align_offset = buf->end % align;
    region.offset = align_offset ? buf->end - align_offset + align : buf->end;
    if (region.offset + size > buf->size) {
        CTK_FATAL("buf (size=%u end=%u) cannot allocate region of size %u and alignment %u (only %u bytes left)",
                  buf->size, buf->end, size, align, buf->size - buf->end);
    }
    region.size = size;
    buf->end = region.offset + region.size;
    return region;
}

static void vtk_write_to_host_region(VkDevice logical_device, void *data, VkDeviceSize size,
                                     struct vtk_region *region, VkDeviceSize region_offset) {
    if (region_offset + size > region->size)
        CTK_FATAL("cannot write %u bytes at offset %u into region (size=%u)", size, region_offset, region->size);
    void *mem = NULL;
    vtk_validate_result(vkMapMemory(logical_device, region->buffer->memory, region->offset + region_offset, size, 0, &mem),
                        "failed to map host coherent buffer region to memory");
    memcpy(mem, data, size);
    vkUnmapMemory(logical_device, region->buffer->memory);
}

static void vtk_write_to_device_region(struct vtk_device *device, VkCommandBuffer cmd_buf, void *data, VkDeviceSize size,
                                       struct vtk_region *staging_region, VkDeviceSize staging_region_offset,
                                       struct vtk_region *region, VkDeviceSize region_offset) {
    if (region_offset + size > region->size)
        CTK_FATAL("cannot write %u bytes at offset %u into region (size=%u)", size, region_offset, region->size);
    vtk_write_to_host_region(device->logical, data, size, staging_region, staging_region_offset);
    VkBufferCopy copy = {};
    copy.srcOffset = staging_region->offset + staging_region_offset;
    copy.dstOffset = region->offset + region_offset;
    copy.size = size;
    vkCmdCopyBuffer(cmd_buf, staging_region->buffer->handle, region->buffer->handle, 1, &copy);
}

////////////////////////////////////////////////////////////
/// Shader
////////////////////////////////////////////////////////////
struct vtk_shader {
    VkShaderModule handle;
    VkShaderStageFlagBits stage;
};

static struct vtk_shader vtk_create_shader(VkDevice logical_device, cstr spirv_path, VkShaderStageFlagBits stage) {
    struct vtk_shader shader = {};
    shader.stage = stage;
    struct ctk_buffer<u8> byte_code = ctk_read_file<u8>(spirv_path);

    VkShaderModuleCreateInfo ShaderModuleCreateInfo = {};
    ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ShaderModuleCreateInfo.flags = 0;
    ShaderModuleCreateInfo.codeSize = ctk_byte_size(&byte_code);
    ShaderModuleCreateInfo.pCode = (u32 const *)byte_code.data;
    vtk_validate_result(vkCreateShaderModule(logical_device, &ShaderModuleCreateInfo, NULL, &shader.handle),
                        "failed to create shader from SPIR-V bytecode in \"%p\"", spirv_path);

    ctk_free(&byte_code);
    return shader;
}

////////////////////////////////////////////////////////////
/// Command Buffer
////////////////////////////////////////////////////////////
static void vtk_allocate_command_buffers(VkDevice logical_device, VkCommandPool cmd_pool, VkCommandBufferLevel level,
                                         u32 count, VkCommandBuffer *cmd_bufs) {
    VkCommandBufferAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.commandPool = cmd_pool;
    info.level = level;
    info.commandBufferCount = count;
    vtk_validate_result(vkAllocateCommandBuffers(logical_device, &info, cmd_bufs), "failed to allocate command buffer");
}

static VkCommandBuffer vtk_allocate_command_buffer(VkDevice logical_device, VkCommandPool cmd_pool, VkCommandBufferLevel level) {
    VkCommandBuffer cmd_buf = VK_NULL_HANDLE;
    vtk_allocate_command_buffers(logical_device, cmd_pool, level, 1, &cmd_buf);
    return cmd_buf;
}

static void vtk_begin_one_time_command_buffer(VkCommandBuffer cmd_buf) {
    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    info.pInheritanceInfo = NULL;
    vkBeginCommandBuffer(cmd_buf, &info);
}

static void vtk_submit_one_time_command_buffer(VkCommandBuffer cmd_buf, VkQueue queue) {
    vkEndCommandBuffer(cmd_buf);
    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &cmd_buf;
    vtk_validate_result(vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE), "failed to submit one-time command buffer");
    vkQueueWaitIdle(queue);
}

////////////////////////////////////////////////////////////
/// Descriptor Set
////////////////////////////////////////////////////////////
struct vtk_descriptor_set {
    struct ctk_array<VkDescriptorSet, 4> instances;
    struct ctk_array<u32, 8> dynamic_offsets;
};

static void vtk_allocate_descriptor_set(struct vtk_descriptor_set *set, VkDescriptorSetLayout layout, u32 instance_count,
                                        VkDevice logical_device, VkDescriptorPool pool) {
    struct ctk_array<VkDescriptorSetLayout, 4> layouts = {};
    CTK_ASSERT(instance_count < layouts.size)
    CTK_REPEAT(instance_count)
        ctk_push(&layouts, layout);

    VkDescriptorSetAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = pool;
    alloc_info.descriptorSetCount = layouts.count;
    alloc_info.pSetLayouts = layouts.data;
    vtk_validate_result(vkAllocateDescriptorSets(logical_device, &alloc_info, set->instances.data), "failed to allocate descriptor sets");
}

static void vtk_bind_descriptor_sets(VkCommandBuffer cmd_buf, VkPipelineLayout layout, struct vtk_descriptor_set **sets, u32 set_count,
                                     u32 first_set_idx, u32 instance_idx = 0, u32 dynamic_idx = 0) {
    struct ctk_array<VkDescriptorSet, 4> sets_to_bind = {};
    struct ctk_array<u32, 16> dynamic_offsets = {};
    for (u32 set_idx = 0; set_idx < set_count; ++set_idx) {
        struct vtk_descriptor_set *set = sets[set_idx];
        ctk_push(&sets_to_bind, set->instances[instance_idx]);
        for (u32 dynamic_offset_idx = 0; dynamic_offset_idx < set->dynamic_offsets.count; ++dynamic_offset_idx)
            ctk_push(&dynamic_offsets, set->dynamic_offsets[dynamic_offset_idx] * dynamic_idx);
    }
    vkCmdBindDescriptorSets(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, layout,
                            first_set_idx, sets_to_bind.count, sets_to_bind.data, // Sets to be bound to [first .. first + count]
                            dynamic_offsets.count, dynamic_offsets.data);
}

////////////////////////////////////////////////////////////
/// Uniform Buffer
////////////////////////////////////////////////////////////
struct vtk_uniform_buffer {
    struct ctk_array<struct vtk_region, 4> regions;
    u32 element_size;
};

static struct vtk_uniform_buffer vtk_create_uniform_buffer(struct vtk_buffer *buf, struct vtk_device *device, VkDeviceSize elem_count,
                                                           VkDeviceSize elem_size, u32 instance_count) {
    CTK_ASSERT(instance_count > 0)
    struct vtk_uniform_buffer uniform_buf = {};
    uniform_buf.element_size = elem_size;
    CTK_REPEAT(instance_count) {
        ctk_push(&uniform_buf.regions,
                 vtk_allocate_region(buf, elem_count * elem_size, device->properties.limits.minUniformBufferOffsetAlignment));
    }
    return uniform_buf;
}

////////////////////////////////////////////////////////////
/// Render Pass
////////////////////////////////////////////////////////////
struct vtk_subpass_info {
    struct ctk_array<u32, 16> preserve_attachment_indexes;
    struct ctk_array<VkAttachmentReference, 8> input_attachment_refs;
    struct ctk_array<VkAttachmentReference, 8> color_attachment_refs;
    struct ctk_optional<VkAttachmentReference> depth_attachment_ref;
};

struct vtk_framebuffer_info {
    struct ctk_array<VkImageView, 16> attachments;
    VkExtent2D extent;
    u32 layers;
};

struct vtk_render_pass_info {
    struct ctk_array<VkAttachmentDescription, 16> attachment_descriptions;
    struct ctk_array<struct vtk_subpass_info, 64> subpass_infos;
    struct ctk_array<VkSubpassDependency, 64> subpass_dependencies;
    struct ctk_array<struct vtk_framebuffer_info, 4> framebuffer_infos;
    struct ctk_array<VkClearValue, 16> clear_values;
};

struct vtk_render_pass {
    VkRenderPass handle;
    struct ctk_array<VkClearValue, 16> clear_values;
    struct ctk_array<VkFramebuffer, 4> framebuffers;
};

static VkFramebuffer vtk_create_framebuffer(VkDevice logical_device, VkRenderPass rp, struct vtk_framebuffer_info *info) {
    VkFramebufferCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    create_info.renderPass = rp;
    create_info.attachmentCount = info->attachments.count;
    create_info.pAttachments = info->attachments.data;
    create_info.width = info->extent.width;
    create_info.height = info->extent.height;
    create_info.layers = info->layers;
    VkFramebuffer fb = VK_NULL_HANDLE;
    vtk_validate_result(vkCreateFramebuffer(logical_device, &create_info, NULL, &fb), "failed to create framebuffer");
    return fb;
}

static struct vtk_render_pass vtk_create_render_pass(VkDevice logical_device, VkCommandPool cmd_pool, struct vtk_render_pass_info *info) {
    struct vtk_render_pass rp = {};

    // Clear Values
    ctk_push(&rp.clear_values, info->clear_values.data, info->clear_values.count);

    // Subpass Descriptions
    struct ctk_array<VkSubpassDescription, 64> subpass_descriptions = {};
    CTK_EACH(struct vtk_subpass_info, subpass_info, info->subpass_infos) {
        VkSubpassDescription *description = ctk_push(&subpass_descriptions);
        description->pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        description->inputAttachmentCount = subpass_info->input_attachment_refs.count;
        description->pInputAttachments = subpass_info->input_attachment_refs.data;
        description->colorAttachmentCount = subpass_info->color_attachment_refs.count;
        description->pColorAttachments = subpass_info->color_attachment_refs.data;
        description->pResolveAttachments = NULL;
        description->pDepthStencilAttachment = subpass_info->depth_attachment_ref ? &subpass_info->depth_attachment_ref.value : NULL;
        description->preserveAttachmentCount = subpass_info->preserve_attachment_indexes.count;
        description->pPreserveAttachments = subpass_info->preserve_attachment_indexes.data;
    }

    // Render Pass
    VkRenderPassCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    create_info.attachmentCount = info->attachment_descriptions.count;
    create_info.pAttachments = info->attachment_descriptions.data;
    create_info.subpassCount = subpass_descriptions.count;
    create_info.pSubpasses = subpass_descriptions.data;
    create_info.dependencyCount = info->subpass_dependencies.count;
    create_info.pDependencies = info->subpass_dependencies.data;
    vtk_validate_result(vkCreateRenderPass(logical_device, &create_info, NULL, &rp.handle), "failed to create render pass");

    // Framebuffers
    u32 framebuffer_count = info->framebuffer_infos.count;
    for (u32 i = 0; i < framebuffer_count; ++i)
        ctk_push(&rp.framebuffers, vtk_create_framebuffer(logical_device, rp.handle, info->framebuffer_infos + i));

    return rp;
}

////////////////////////////////////////////////////////////
/// Image
////////////////////////////////////////////////////////////
struct vtk_image {
    VkImage handle;
    VkDeviceMemory memory;
    VkImageView view;
};

static struct vtk_image vtk_create_image(struct vtk_device *device, struct vtk_image_info *info) {
    struct vtk_image image = {};

    VkImageCreateInfo image_ci = {};
    image_ci.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_ci.flags = 0;
    image_ci.imageType = VK_IMAGE_TYPE_2D;
    image_ci.format = info->format;
    image_ci.extent.width = info->width;
    image_ci.extent.height = info->height;
    image_ci.extent.depth = 1;
    image_ci.mipLevels = 1;
    image_ci.arrayLayers = 1;
    image_ci.samples = VK_SAMPLE_COUNT_1_BIT;
    image_ci.tiling = info->tiling;
    image_ci.usage = info->usage_flags;
    image_ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_ci.queueFamilyIndexCount = 0;
    image_ci.pQueueFamilyIndices = NULL; // Ignored if sharingMode is not VK_SHARING_MODE_CONCURRENT.
    image_ci.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    vtk_validate_result(vkCreateImage(device->logical, &image_ci, NULL, &image.handle), "failed to create image");

    // Allocate / Bind Memory
    VkMemoryRequirements mem_reqs = {};
    vkGetImageMemoryRequirements(device->logical, image.handle, &mem_reqs);
    image.memory = vtk_allocate_device_memory(device, &mem_reqs, info->memory_property_flags);
    vtk_validate_result(vkBindImageMemory(device->logical, image.handle, image.memory, 0), "failed to bind image memory");

    VkImageViewCreateInfo view_ci = {};
    view_ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_ci.image = image.handle;
    view_ci.flags = 0;
    view_ci.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_ci.format = info->format;
    view_ci.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_ci.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_ci.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_ci.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_ci.subresourceRange.aspectMask = info->aspect_mask;
    view_ci.subresourceRange.baseMipLevel = 0;
    view_ci.subresourceRange.levelCount = 1;
    view_ci.subresourceRange.baseArrayLayer = 0;
    view_ci.subresourceRange.layerCount = 1;
    vtk_validate_result(vkCreateImageView(device->logical, &view_ci, NULL, &image.view), "failed to create image view");

    return image;
}

////////////////////////////////////////////////////////////
/// Vertex Layout
////////////////////////////////////////////////////////////
struct vtk_vertex_attribute {
    VkFormat format;
    u32 size;
    u32 offset;
};

struct vtk_vertex_layout {
    struct ctk_map<struct vtk_vertex_attribute, 4> attributes;
    u32 size;
};

static void vtk_push_vertex_attribute(struct vtk_vertex_layout *layout, cstr name, u32 elem_count) {
    static VkFormat const FORMATS[] = {
        VK_FORMAT_R32_SFLOAT,
        VK_FORMAT_R32G32_SFLOAT,
        VK_FORMAT_R32G32B32_SFLOAT,
        VK_FORMAT_R32G32B32A32_SFLOAT,
    };
    CTK_ASSERT(elem_count <= CTK_ARRAY_COUNT(FORMATS));
    u32 attrib_size = sizeof(f32) * elem_count;
    ctk_push(&layout->attributes, name, { FORMATS[elem_count - 1], attrib_size, layout->size });
    layout->size += attrib_size;
}

////////////////////////////////////////////////////////////
/// Graphics Pipeline
////////////////////////////////////////////////////////////
#define VTK_COLOR_COMPONENT_RGBA VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT

struct vtk_vertex_input {
    u32 binding;
    u32 location;
    struct vtk_vertex_attribute *attribute;
};

struct vtk_graphics_pipeline_info {
    struct ctk_array<struct vtk_shader *, 8> shaders;
    struct ctk_array<VkDescriptorSetLayout, 8> descriptor_set_layouts;
    struct ctk_array<VkPushConstantRange, 8> push_constant_ranges;
    struct ctk_array<struct vtk_vertex_input, 8> vertex_inputs;
    struct ctk_array<VkVertexInputBindingDescription, 4> vertex_input_binding_descriptions;
    struct ctk_array<VkViewport, 4> viewports;
    struct ctk_array<VkRect2D, 4> scissors;
    struct ctk_array<VkPipelineColorBlendAttachmentState, 4> color_blend_attachment_states;

    VkPipelineInputAssemblyStateCreateInfo input_assembly_state;
    VkPipelineDepthStencilStateCreateInfo depth_stencil_state;
    VkPipelineRasterizationStateCreateInfo rasterization_state;
    VkPipelineMultisampleStateCreateInfo multisample_state;
    VkPipelineColorBlendStateCreateInfo color_blend_state;
    u32 subpass;
};

struct vtk_graphics_pipeline {
    VkPipeline handle;
    VkPipelineLayout layout;
};

static struct vtk_graphics_pipeline_info vtk_default_graphics_pipeline_info() {
    struct vtk_graphics_pipeline_info info = {};
    info.input_assembly_state.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    info.input_assembly_state.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    info.input_assembly_state.primitiveRestartEnable = VK_FALSE;

    info.depth_stencil_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    // Depth
    info.depth_stencil_state.depthTestEnable = VK_FALSE;
    info.depth_stencil_state.depthWriteEnable = VK_FALSE;
    info.depth_stencil_state.depthCompareOp = VK_COMPARE_OP_LESS;
    info.depth_stencil_state.depthBoundsTestEnable = VK_FALSE;
    info.depth_stencil_state.minDepthBounds = 0.0f;
    info.depth_stencil_state.maxDepthBounds = 1.0f;
    // Stencil
    info.depth_stencil_state.stencilTestEnable = VK_FALSE;
    info.depth_stencil_state.front.compareOp = VK_COMPARE_OP_NEVER;
    info.depth_stencil_state.front.passOp = VK_STENCIL_OP_KEEP;
    info.depth_stencil_state.front.failOp = VK_STENCIL_OP_KEEP;
    info.depth_stencil_state.front.depthFailOp = VK_STENCIL_OP_KEEP;
    info.depth_stencil_state.front.compareMask = 0xFF;
    info.depth_stencil_state.front.writeMask = 0xFF;
    info.depth_stencil_state.front.reference = 1;
    info.depth_stencil_state.back = info.depth_stencil_state.front;

    info.rasterization_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    info.rasterization_state.depthClampEnable = VK_FALSE; // Don't clamp fragments within depth range.
    info.rasterization_state.rasterizerDiscardEnable = VK_FALSE;
    info.rasterization_state.polygonMode = VK_POLYGON_MODE_FILL; // Only available mode on AMD gpus?
    info.rasterization_state.cullMode = VK_CULL_MODE_BACK_BIT;
    info.rasterization_state.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    info.rasterization_state.depthBiasEnable = VK_FALSE;
    info.rasterization_state.depthBiasConstantFactor = 0.0f;
    info.rasterization_state.depthBiasClamp = 0.0f;
    info.rasterization_state.depthBiasSlopeFactor = 0.0f;
    info.rasterization_state.lineWidth = 1.0f;

    info.multisample_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    info.multisample_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    info.multisample_state.sampleShadingEnable = VK_FALSE;
    info.multisample_state.minSampleShading = 1.0f;
    info.multisample_state.pSampleMask = NULL;
    info.multisample_state.alphaToCoverageEnable = VK_FALSE;
    info.multisample_state.alphaToOneEnable = VK_FALSE;

    info.color_blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    info.color_blend_state.logicOpEnable = VK_FALSE;
    info.color_blend_state.logicOp = VK_LOGIC_OP_COPY;
    info.color_blend_state.attachmentCount = 0;
    info.color_blend_state.pAttachments = NULL;
    info.color_blend_state.blendConstants[0] = 1.0f;
    info.color_blend_state.blendConstants[1] = 1.0f;
    info.color_blend_state.blendConstants[2] = 1.0f;
    info.color_blend_state.blendConstants[3] = 1.0f;

    return info;
}

static VkPipelineColorBlendAttachmentState vtk_default_color_blend_attachment_state() {
    VkPipelineColorBlendAttachmentState state = {};
    state.blendEnable = VK_FALSE;
    state.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    state.colorBlendOp = VK_BLEND_OP_ADD;
    state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    state.alphaBlendOp = VK_BLEND_OP_ADD;
    state.colorWriteMask = VTK_COLOR_COMPONENT_RGBA;
    return state;
}

static struct vtk_graphics_pipeline vtk_create_graphics_pipeline(VkDevice logical_device, struct vtk_render_pass *rp, u32 subpass_index,
                                                                 struct vtk_graphics_pipeline_info *info) {
    struct vtk_graphics_pipeline gp = {};

    // Shader Stages
    struct ctk_array<VkPipelineShaderStageCreateInfo, 4> shader_stages = {};
    for (u32 i = 0; i < info->shaders.count; ++i) {
        struct vtk_shader *shader = info->shaders[i];
        VkPipelineShaderStageCreateInfo *shader_stage_ci = ctk_push(&shader_stages);
        shader_stage_ci->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stage_ci->flags = 0;
        shader_stage_ci->stage = shader->stage;
        shader_stage_ci->module = shader->handle;
        shader_stage_ci->pName = "main";
        shader_stage_ci->pSpecializationInfo = NULL;
    }

    VkPipelineLayoutCreateInfo layout_ci = {};
    layout_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_ci.setLayoutCount = info->descriptor_set_layouts.count;
    layout_ci.pSetLayouts = info->descriptor_set_layouts.data;
    layout_ci.pushConstantRangeCount = info->push_constant_ranges.count;
    layout_ci.pPushConstantRanges = info->push_constant_ranges.data;
    vtk_validate_result(vkCreatePipelineLayout(logical_device, &layout_ci, NULL, &gp.layout), "failed to create graphics pipeline layout");

    // Vertex Attribute Descriptions
    struct ctk_array<VkVertexInputAttributeDescription, 4> vert_attrib_descs = {};
    CTK_EACH(struct vtk_vertex_input, vert_input, info->vertex_inputs) {
        VkVertexInputAttributeDescription *attrib_desc = ctk_push(&vert_attrib_descs);
        attrib_desc->location = vert_input->location;
        attrib_desc->binding = vert_input->binding;
        attrib_desc->format = vert_input->attribute->format;
        attrib_desc->offset = vert_input->attribute->offset;
    }

    VkPipelineVertexInputStateCreateInfo vert_input_state = {};
    vert_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vert_input_state.vertexBindingDescriptionCount = info->vertex_input_binding_descriptions.count;
    vert_input_state.pVertexBindingDescriptions = info->vertex_input_binding_descriptions.data;
    vert_input_state.vertexAttributeDescriptionCount = vert_attrib_descs.count;
    vert_input_state.pVertexAttributeDescriptions = vert_attrib_descs.data;

    VkPipelineViewportStateCreateInfo viewport_state = {};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = info->viewports.count;
    viewport_state.pViewports = info->viewports.data;
    viewport_state.scissorCount = info->scissors.count;
    viewport_state.pScissors = info->scissors.data;

    info->color_blend_state.attachmentCount = info->color_blend_attachment_states.count;
    info->color_blend_state.pAttachments = info->color_blend_attachment_states.data;

    VkGraphicsPipelineCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    create_info.stageCount = shader_stages.count;
    create_info.pStages = shader_stages.data;
    create_info.pVertexInputState = &vert_input_state;
    create_info.pInputAssemblyState = &info->input_assembly_state;
    create_info.pTessellationState = NULL;
    create_info.pViewportState = &viewport_state;
    create_info.pRasterizationState = &info->rasterization_state;
    create_info.pMultisampleState = &info->multisample_state;
    create_info.pDepthStencilState = &info->depth_stencil_state;
    create_info.pColorBlendState = &info->color_blend_state;
    create_info.pDynamicState = NULL;
    create_info.layout = gp.layout;
    create_info.renderPass = rp->handle;
    create_info.subpass = subpass_index;
    create_info.basePipelineHandle = VK_NULL_HANDLE;
    create_info.basePipelineIndex = -1;
    vtk_validate_result(vkCreateGraphicsPipelines(logical_device, VK_NULL_HANDLE, 1, &create_info, NULL, &gp.handle),
                        "failed to create graphics pipeline");

    return gp;
}
