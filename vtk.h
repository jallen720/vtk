#pragma once

#include <vulkan/vulkan.h>
#include "ctk/ctk.h"
#include "ctk/memory.h"
#include "ctk/containers.h"

#define _VTK_VK_RESULT_NAME(VK_RESULT) VK_RESULT, #VK_RESULT

#define VTK_LOAD_INSTANCE_EXTENSION_FUNCTION(INSTANCE, FUNC_NAME)\
    auto FUNC_NAME = (PFN_ ## FUNC_NAME)vkGetInstanceProcAddr(INSTANCE, #FUNC_NAME);\
    if (FUNC_NAME == NULL)\
        CTK_FATAL("failed to load instance extension function \"%s\"", #FUNC_NAME)

////////////////////////////////////////////////////////////
/// Data
////////////////////////////////////////////////////////////
struct VTK_VkResultInfo {
    VkResult result;
    cstr name;
    cstr message;
};

struct VTK_BufferInfo {
    VkDeviceSize size;
    VkBufferUsageFlags usage_flags;
    VkMemoryPropertyFlags memory_property_flags;
    VkSharingMode sharing_mode;
};

struct VTK_Buffer {
    VkBuffer handle;
    VkDeviceMemory memory;
    VkDeviceSize size;
    VkDeviceSize end;
};

struct VTK_Region {
    VTK_Buffer *buffer;
    VkDeviceSize size;
    VkDeviceSize offset;
};

////////////////////////////////////////////////////////////
/// Debugging
////////////////////////////////////////////////////////////
static void vtk_log_result(VkResult result) {
    static VTK_VkResultInfo VK_RESULT_DEBUG_INFOS[] = {
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

    VTK_VkResultInfo *info = NULL;
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

template<typename ...Args>
static void vtk_validate_result(VkResult result, cstr fail_msg, Args... args) {
    if (result != VK_SUCCESS) {
        vtk_log_result(result);
        CTK_FATAL(fail_msg, args...)
    }
}


static VKAPI_ATTR VkBool32 VKAPI_CALL
vtk_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity_flag_bit,
                   VkDebugUtilsMessageTypeFlagsEXT msg_type_flags,
                   VkDebugUtilsMessengerCallbackDataEXT const *cb_data,
                   void *user_data)
{
    cstr msg_id = cb_data->pMessageIdName ? cb_data->pMessageIdName : "";

    if (VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT & msg_severity_flag_bit)
        CTK_FATAL("VALIDATION LAYER [%s]: %s\n", msg_id, cb_data->pMessage)
    else if (VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT & msg_severity_flag_bit)
        ctk_warning("VALIDATION LAYER [%s]: %s\n", msg_id, cb_data->pMessage);
    else
        ctk_info("VALIDATION LAYER [%s]: %s\n", msg_id, cb_data->pMessage);

    return VK_FALSE;
}

////////////////////////////////////////////////////////////
/// Interface
////////////////////////////////////////////////////////////
template<typename Object, typename Loader, typename ...Args>
static CTK_Array<Object> *vtk_load_vk_objects(CTK_Allocator *allocator, Loader loader, Args... args) {
    u32 count = 0;
    loader(args..., &count, NULL);
    CTK_ASSERT(count > 0);
    auto vk_objects = ctk_create_array_full<Object>(count, 0, allocator);
    loader(args..., &vk_objects->count, vk_objects->data);
    return vk_objects;
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

    // Find format that supports depth-stencil attachment feature for physical device.
    for (u32 i = 0; i < CTK_ARRAY_COUNT(DEPTH_IMAGE_FORMATS); i++) {
        VkFormat depth_img_fmt = DEPTH_IMAGE_FORMATS[i];
        VkFormatProperties depth_img_fmt_props = {};
        vkGetPhysicalDeviceFormatProperties(physical_device, depth_img_fmt, &depth_img_fmt_props);
        if ((depth_img_fmt_props.optimalTilingFeatures & DEPTH_IMG_FMT_FEATS) == DEPTH_IMG_FMT_FEATS)
            return depth_img_fmt;
    }

    CTK_FATAL("failed to find physical device depth format that supports the depth-stencil attachment feature")
}

static VkDeviceQueueCreateInfo vtk_default_queue_info(u32 queue_fam_idx) {
    static f32 const QUEUE_PRIORITIES[] = { 1.0f };

    VkDeviceQueueCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    info.flags = 0;
    info.queueFamilyIndex = queue_fam_idx;
    info.queueCount = CTK_ARRAY_COUNT(QUEUE_PRIORITIES);
    info.pQueuePriorities = QUEUE_PRIORITIES;

    return info;
}

static VkDeviceMemory vtk_allocate_device_memory(VkDevice device, VkPhysicalDeviceMemoryProperties mem_props,
                                                 VkMemoryRequirements mem_reqs, VkMemoryPropertyFlags mem_prop_flags) {
    // Find memory type index from device based on memory property flags.
    u32 selected_mem_type_idx = CTK_U32_MAX;
    for (u32 mem_type_idx = 0; mem_type_idx < mem_props.memoryTypeCount; ++mem_type_idx) {
        // Ensure index refers to memory type from memory requirements.
        if (!(mem_reqs.memoryTypeBits & (1 << mem_type_idx)))
            continue;

        // Check if memory at index has correct properties.
        if ((mem_props.memoryTypes[mem_type_idx].propertyFlags & mem_prop_flags) == mem_prop_flags) {
            selected_mem_type_idx = mem_type_idx;
            break;
        }
    }

    if (selected_mem_type_idx == CTK_U32_MAX)
        CTK_FATAL("failed to find memory type that satisfies property requirements")

    // Allocate memory
    VkMemoryAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    info.allocationSize = mem_reqs.size;
    info.memoryTypeIndex = selected_mem_type_idx;
    VkDeviceMemory mem = VK_NULL_HANDLE;
    vtk_validate_result(vkAllocateMemory(device, &info, NULL, &mem), "failed to allocate memory");
    return mem;
}

static VTK_Buffer vtk_create_buffer(VkDevice device, VkPhysicalDeviceMemoryProperties mem_props,
                                    VTK_BufferInfo *buf_info) {
    VTK_Buffer buf = {};
    buf.size = buf_info->size;

    VkBufferCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.size = buf_info->size;
    info.usage = buf_info->usage_flags;
    info.sharingMode = buf_info->sharing_mode;
    info.queueFamilyIndexCount = 0;
    info.pQueueFamilyIndices = NULL; // Ignored if sharingMode is not VK_SHARING_MODE_CONCURRENT.
    vtk_validate_result(vkCreateBuffer(device, &info, NULL, &buf.handle), "failed to create buffer");

    // Allocate / Bind Memory
    VkMemoryRequirements mem_reqs = {};
    vkGetBufferMemoryRequirements(device, buf.handle, &mem_reqs);
    buf.memory = vtk_allocate_device_memory(device, mem_props, mem_reqs, buf_info->memory_property_flags);
    vtk_validate_result(vkBindBufferMemory(device, buf.handle, buf.memory, 0), "failed to bind buffer memory");

    return buf;
}

static VTK_Region vtk_allocate_region(VTK_Buffer *buf, u32 size, VkDeviceSize align = 1) {
    VTK_Region region = {};
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

static VkCommandBuffer vtk_allocate_command_buffer(VkDevice logical_device, VkCommandPool cmd_pool,
                                                   VkCommandBufferLevel level) {
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
    vtk_validate_result(vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE),
                        "failed to submit one-time command buffer");
    vkQueueWaitIdle(queue);
}
