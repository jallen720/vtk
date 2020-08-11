#pragma once

#include <vulkan/vulkan.h>
#include "ctk/ctk.h"

////////////////////////////////////////////////////////////
/// Macros
////////////////////////////////////////////////////////////
#define VTK_VK_RESULT_NAME(VK_RESULT) VK_RESULT, #VK_RESULT

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
/// Interface
////////////////////////////////////////////////////////////
static cstr
vk_format_name(VkFormat Format)
{
    static ctk::pair<VkFormat, cstr> FORMATS[] =
    {
        CTK_VALUE_NAME_PAIR(VK_FORMAT_UNDEFINED),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R4G4_UNORM_PACK8),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R4G4B4A4_UNORM_PACK16),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_B4G4R4A4_UNORM_PACK16),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R5G6B5_UNORM_PACK16),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_B5G6R5_UNORM_PACK16),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R5G5B5A1_UNORM_PACK16),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_B5G5R5A1_UNORM_PACK16),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_A1R5G5B5_UNORM_PACK16),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R8_UNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R8_SNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R8_USCALED),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R8_SSCALED),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R8_UINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R8_SINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R8_SRGB),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R8G8_UNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R8G8_SNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R8G8_USCALED),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R8G8_SSCALED),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R8G8_UINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R8G8_SINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R8G8_SRGB),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R8G8B8_UNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R8G8B8_SNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R8G8B8_USCALED),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R8G8B8_SSCALED),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R8G8B8_UINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R8G8B8_SINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R8G8B8_SRGB),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_B8G8R8_UNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_B8G8R8_SNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_B8G8R8_USCALED),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_B8G8R8_SSCALED),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_B8G8R8_UINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_B8G8R8_SINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_B8G8R8_SRGB),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R8G8B8A8_UNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R8G8B8A8_SNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R8G8B8A8_USCALED),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R8G8B8A8_SSCALED),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R8G8B8A8_UINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R8G8B8A8_SINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R8G8B8A8_SRGB),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_B8G8R8A8_UNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_B8G8R8A8_SNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_B8G8R8A8_USCALED),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_B8G8R8A8_SSCALED),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_B8G8R8A8_UINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_B8G8R8A8_SINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_B8G8R8A8_SRGB),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_A8B8G8R8_UNORM_PACK32),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_A8B8G8R8_SNORM_PACK32),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_A8B8G8R8_USCALED_PACK32),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_A8B8G8R8_SSCALED_PACK32),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_A8B8G8R8_UINT_PACK32),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_A8B8G8R8_SINT_PACK32),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_A8B8G8R8_SRGB_PACK32),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_A2R10G10B10_UNORM_PACK32),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_A2R10G10B10_SNORM_PACK32),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_A2R10G10B10_USCALED_PACK32),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_A2R10G10B10_SSCALED_PACK32),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_A2R10G10B10_UINT_PACK32),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_A2R10G10B10_SINT_PACK32),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_A2B10G10R10_UNORM_PACK32),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_A2B10G10R10_SNORM_PACK32),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_A2B10G10R10_USCALED_PACK32),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_A2B10G10R10_SSCALED_PACK32),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_A2B10G10R10_UINT_PACK32),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_A2B10G10R10_SINT_PACK32),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R16_UNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R16_SNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R16_USCALED),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R16_SSCALED),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R16_UINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R16_SINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R16_SFLOAT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R16G16_UNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R16G16_SNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R16G16_USCALED),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R16G16_SSCALED),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R16G16_UINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R16G16_SINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R16G16_SFLOAT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R16G16B16_UNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R16G16B16_SNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R16G16B16_USCALED),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R16G16B16_SSCALED),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R16G16B16_UINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R16G16B16_SINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R16G16B16_SFLOAT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R16G16B16A16_UNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R16G16B16A16_SNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R16G16B16A16_USCALED),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R16G16B16A16_SSCALED),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R16G16B16A16_UINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R16G16B16A16_SINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R16G16B16A16_SFLOAT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R32_UINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R32_SINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R32_SFLOAT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R32G32_UINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R32G32_SINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R32G32_SFLOAT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R32G32B32_UINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R32G32B32_SINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R32G32B32_SFLOAT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R32G32B32A32_UINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R32G32B32A32_SINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R32G32B32A32_SFLOAT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R64_UINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R64_SINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R64_SFLOAT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R64G64_UINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R64G64_SINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R64G64_SFLOAT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R64G64B64_UINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R64G64B64_SINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R64G64B64_SFLOAT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R64G64B64A64_UINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R64G64B64A64_SINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R64G64B64A64_SFLOAT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_B10G11R11_UFLOAT_PACK32),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_E5B9G9R9_UFLOAT_PACK32),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_D16_UNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_X8_D24_UNORM_PACK32),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_D32_SFLOAT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_S8_UINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_D16_UNORM_S8_UINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_D24_UNORM_S8_UINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_D32_SFLOAT_S8_UINT),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_BC1_RGB_UNORM_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_BC1_RGB_SRGB_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_BC1_RGBA_UNORM_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_BC1_RGBA_SRGB_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_BC2_UNORM_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_BC2_SRGB_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_BC3_UNORM_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_BC3_SRGB_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_BC4_UNORM_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_BC4_SNORM_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_BC5_UNORM_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_BC5_SNORM_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_BC6H_UFLOAT_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_BC6H_SFLOAT_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_BC7_UNORM_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_BC7_SRGB_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_EAC_R11_UNORM_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_EAC_R11_SNORM_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_EAC_R11G11_UNORM_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_EAC_R11G11_SNORM_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ASTC_4x4_UNORM_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ASTC_4x4_SRGB_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ASTC_5x4_UNORM_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ASTC_5x4_SRGB_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ASTC_5x5_UNORM_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ASTC_5x5_SRGB_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ASTC_6x5_UNORM_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ASTC_6x5_SRGB_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ASTC_6x6_UNORM_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ASTC_6x6_SRGB_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ASTC_8x5_UNORM_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ASTC_8x5_SRGB_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ASTC_8x6_UNORM_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ASTC_8x6_SRGB_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ASTC_8x8_UNORM_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ASTC_8x8_SRGB_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ASTC_10x5_UNORM_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ASTC_10x5_SRGB_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ASTC_10x6_UNORM_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ASTC_10x6_SRGB_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ASTC_10x8_UNORM_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ASTC_10x8_SRGB_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ASTC_10x10_UNORM_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ASTC_10x10_SRGB_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ASTC_12x10_UNORM_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ASTC_12x10_SRGB_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ASTC_12x12_UNORM_BLOCK),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_ASTC_12x12_SRGB_BLOCK),

        // Provided by VK_VERSION_1_1
        CTK_VALUE_NAME_PAIR(VK_FORMAT_G8B8G8R8_422_UNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_B8G8R8G8_422_UNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_G8_B8R8_2PLANE_420_UNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_G8_B8R8_2PLANE_422_UNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R10X6_UNORM_PACK16),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R10X6G10X6_UNORM_2PACK16),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R12X4_UNORM_PACK16),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R12X4G12X4_UNORM_2PACK16),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_G16B16G16R16_422_UNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_B16G16R16G16_422_UNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_G16_B16R16_2PLANE_420_UNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_G16_B16R16_2PLANE_422_UNORM),
        CTK_VALUE_NAME_PAIR(VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM),
    };
    ctk::pair<VkFormat, cstr> *Pair = ctk::find_pair(FORMATS, CTK_ARRAY_COUNT(FORMATS), Format);
    if(Pair == NULL) {
        CTK_FATAL("unable to find name for VkFormat %u", Format);
    }
    return Pair->Value;
}

static void
output_vk_result(VkResult Result, cstr FunctionName)
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
        if(DebugInfo->Result == Result) break;
    }
    if(!DebugInfo) CTK_FATAL("failed to find debug info for VkResult %d", Result)

    if(DebugInfo->Result == 0)
    {
        ctk::info("%s() returned %s: %s", FunctionName, DebugInfo->ResultName, DebugInfo->Message);
    }
    else if(DebugInfo->Result > 0)
    {
        ctk::warning("%s() returned %s: %s", FunctionName, DebugInfo->ResultName, DebugInfo->Message);
    }
    else
    {
        ctk::error("%s() returned %s: %s", FunctionName, DebugInfo->ResultName, DebugInfo->Message);
    }
}

static void
print_device_limits(VkPhysicalDeviceLimits *DeviceLimits)
{
    ctk::info("device limits:");
    ctk::print_line(1, "maxImageDimension1D: %u", DeviceLimits->maxImageDimension1D);
    ctk::print_line(1, "maxImageDimension2D: %u", DeviceLimits->maxImageDimension2D);
    ctk::print_line(1, "maxImageDimension3D: %u", DeviceLimits->maxImageDimension3D);
    ctk::print_line(1, "maxImageDimensionCube: %u", DeviceLimits->maxImageDimensionCube);
    ctk::print_line(1, "maxImageArrayLayers: %u", DeviceLimits->maxImageArrayLayers);
    ctk::print_line(1, "maxTexelBufferElements: %u", DeviceLimits->maxTexelBufferElements);
    ctk::print_line(1, "maxUniformBufferRange: %u", DeviceLimits->maxUniformBufferRange);
    ctk::print_line(1, "maxStorageBufferRange: %u", DeviceLimits->maxStorageBufferRange);
    ctk::print_line(1, "maxPushConstantsSize: %u", DeviceLimits->maxPushConstantsSize);
    ctk::print_line(1, "maxMemoryAllocationCount: %u", DeviceLimits->maxMemoryAllocationCount);
    ctk::print_line(1, "maxSamplerAllocationCount: %u", DeviceLimits->maxSamplerAllocationCount);
    ctk::print_line(1, "bufferImageGranularity: %llu", DeviceLimits->bufferImageGranularity);
    ctk::print_line(1, "sparseAddressSpaceSize: %llu", DeviceLimits->sparseAddressSpaceSize);
    ctk::print_line(1, "maxBoundDescriptorSets: %u", DeviceLimits->maxBoundDescriptorSets);
    ctk::print_line(1, "maxPerStageDescriptorSamplers: %u", DeviceLimits->maxPerStageDescriptorSamplers);
    ctk::print_line(1, "maxPerStageDescriptorUniformBuffers: %u", DeviceLimits->maxPerStageDescriptorUniformBuffers);
    ctk::print_line(1, "maxPerStageDescriptorStorageBuffers: %u", DeviceLimits->maxPerStageDescriptorStorageBuffers);
    ctk::print_line(1, "maxPerStageDescriptorSampledImages: %u", DeviceLimits->maxPerStageDescriptorSampledImages);
    ctk::print_line(1, "maxPerStageDescriptorStorageImages: %u", DeviceLimits->maxPerStageDescriptorStorageImages);
    ctk::print_line(1, "maxPerStageDescriptorInputAttachments: %u", DeviceLimits->maxPerStageDescriptorInputAttachments);
    ctk::print_line(1, "maxPerStageResources: %u", DeviceLimits->maxPerStageResources);
    ctk::print_line(1, "maxDescriptorSetSamplers: %u", DeviceLimits->maxDescriptorSetSamplers);
    ctk::print_line(1, "maxDescriptorSetUniformBuffers: %u", DeviceLimits->maxDescriptorSetUniformBuffers);
    ctk::print_line(1, "maxDescriptorSetUniformBuffersDynamic: %u", DeviceLimits->maxDescriptorSetUniformBuffersDynamic);
    ctk::print_line(1, "maxDescriptorSetStorageBuffers: %u", DeviceLimits->maxDescriptorSetStorageBuffers);
    ctk::print_line(1, "maxDescriptorSetStorageBuffersDynamic: %u", DeviceLimits->maxDescriptorSetStorageBuffersDynamic);
    ctk::print_line(1, "maxDescriptorSetSampledImages: %u", DeviceLimits->maxDescriptorSetSampledImages);
    ctk::print_line(1, "maxDescriptorSetStorageImages: %u", DeviceLimits->maxDescriptorSetStorageImages);
    ctk::print_line(1, "maxDescriptorSetInputAttachments: %u", DeviceLimits->maxDescriptorSetInputAttachments);
    ctk::print_line(1, "maxVertexInputAttributes: %u", DeviceLimits->maxVertexInputAttributes);
    ctk::print_line(1, "maxVertexInputBindings: %u", DeviceLimits->maxVertexInputBindings);
    ctk::print_line(1, "maxVertexInputAttributeOffset: %u", DeviceLimits->maxVertexInputAttributeOffset);
    ctk::print_line(1, "maxVertexInputBindingStride: %u", DeviceLimits->maxVertexInputBindingStride);
    ctk::print_line(1, "maxVertexOutputComponents: %u", DeviceLimits->maxVertexOutputComponents);
    ctk::print_line(1, "maxTessellationGenerationLevel: %u", DeviceLimits->maxTessellationGenerationLevel);
    ctk::print_line(1, "maxTessellationPatchSize: %u", DeviceLimits->maxTessellationPatchSize);
    ctk::print_line(1, "maxTessellationControlPerVertexInputComponents: %u", DeviceLimits->maxTessellationControlPerVertexInputComponents);
    ctk::print_line(1, "maxTessellationControlPerVertexOutputComponents: %u", DeviceLimits->maxTessellationControlPerVertexOutputComponents);
    ctk::print_line(1, "maxTessellationControlPerPatchOutputComponents: %u", DeviceLimits->maxTessellationControlPerPatchOutputComponents);
    ctk::print_line(1, "maxTessellationControlTotalOutputComponents: %u", DeviceLimits->maxTessellationControlTotalOutputComponents);
    ctk::print_line(1, "maxTessellationEvaluationInputComponents: %u", DeviceLimits->maxTessellationEvaluationInputComponents);
    ctk::print_line(1, "maxTessellationEvaluationOutputComponents: %u", DeviceLimits->maxTessellationEvaluationOutputComponents);
    ctk::print_line(1, "maxGeometryShaderInvocations: %u", DeviceLimits->maxGeometryShaderInvocations);
    ctk::print_line(1, "maxGeometryInputComponents: %u", DeviceLimits->maxGeometryInputComponents);
    ctk::print_line(1, "maxGeometryOutputComponents: %u", DeviceLimits->maxGeometryOutputComponents);
    ctk::print_line(1, "maxGeometryOutputVertices: %u", DeviceLimits->maxGeometryOutputVertices);
    ctk::print_line(1, "maxGeometryTotalOutputComponents: %u", DeviceLimits->maxGeometryTotalOutputComponents);
    ctk::print_line(1, "maxFragmentInputComponents: %u", DeviceLimits->maxFragmentInputComponents);
    ctk::print_line(1, "maxFragmentOutputAttachments: %u", DeviceLimits->maxFragmentOutputAttachments);
    ctk::print_line(1, "maxFragmentDualSrcAttachments: %u", DeviceLimits->maxFragmentDualSrcAttachments);
    ctk::print_line(1, "maxFragmentCombinedOutputResources: %u", DeviceLimits->maxFragmentCombinedOutputResources);
    ctk::print_line(1, "maxComputeSharedMemorySize: %u", DeviceLimits->maxComputeSharedMemorySize);
    ctk::print_line(1, "maxComputeWorkGroupCount[3]: { %u, %u, %u }", DeviceLimits->maxComputeWorkGroupCount[0], DeviceLimits->maxComputeWorkGroupCount[1], DeviceLimits->maxComputeWorkGroupCount[2]);
    ctk::print_line(1, "maxComputeWorkGroupInvocations: %u", DeviceLimits->maxComputeWorkGroupInvocations);
    ctk::print_line(1, "maxComputeWorkGroupSize[3]: { %u, %u, %u }", DeviceLimits->maxComputeWorkGroupSize[0], DeviceLimits->maxComputeWorkGroupSize[1], DeviceLimits->maxComputeWorkGroupSize[2]);
    ctk::print_line(1, "subPixelPrecisionBits: %u", DeviceLimits->subPixelPrecisionBits);
    ctk::print_line(1, "subTexelPrecisionBits: %u", DeviceLimits->subTexelPrecisionBits);
    ctk::print_line(1, "mipmapPrecisionBits: %u", DeviceLimits->mipmapPrecisionBits);
    ctk::print_line(1, "maxDrawIndexedIndexValue: %u", DeviceLimits->maxDrawIndexedIndexValue);
    ctk::print_line(1, "maxDrawIndirectCount: %u", DeviceLimits->maxDrawIndirectCount);
    ctk::print_line(1, "maxSamplerLodBias: %f", DeviceLimits->maxSamplerLodBias);
    ctk::print_line(1, "maxSamplerAnisotropy: %f", DeviceLimits->maxSamplerAnisotropy);
    ctk::print_line(1, "maxViewports: %u", DeviceLimits->maxViewports);
    ctk::print_line(1, "maxViewportDimensions[2]: { %u, %u }", DeviceLimits->maxViewportDimensions[0], DeviceLimits->maxViewportDimensions[1]);
    ctk::print_line(1, "viewportBoundsRange[2]: { %f, %f }", DeviceLimits->viewportBoundsRange[0], DeviceLimits->viewportBoundsRange[1]);
    ctk::print_line(1, "viewportSubPixelBits: %u", DeviceLimits->viewportSubPixelBits);
    ctk::print_line(1, "minMemoryMapAlignment: %u", DeviceLimits->minMemoryMapAlignment);
    ctk::print_line(1, "minTexelBufferOffsetAlignment: %llu", DeviceLimits->minTexelBufferOffsetAlignment);
    ctk::print_line(1, "minUniformBufferOffsetAlignment: %llu", DeviceLimits->minUniformBufferOffsetAlignment);
    ctk::print_line(1, "minStorageBufferOffsetAlignment: %llu", DeviceLimits->minStorageBufferOffsetAlignment);
    ctk::print_line(1, "minTexelOffset: %i", DeviceLimits->minTexelOffset);
    ctk::print_line(1, "maxTexelOffset: %u", DeviceLimits->maxTexelOffset);
    ctk::print_line(1, "minTexelGatherOffset: %i", DeviceLimits->minTexelGatherOffset);
    ctk::print_line(1, "maxTexelGatherOffset: %u", DeviceLimits->maxTexelGatherOffset);
    ctk::print_line(1, "minInterpolationOffset: %f", DeviceLimits->minInterpolationOffset);
    ctk::print_line(1, "maxInterpolationOffset: %f", DeviceLimits->maxInterpolationOffset);
    ctk::print_line(1, "subPixelInterpolationOffsetBits: %u", DeviceLimits->subPixelInterpolationOffsetBits);
    ctk::print_line(1, "maxFramebufferWidth: %u", DeviceLimits->maxFramebufferWidth);
    ctk::print_line(1, "maxFramebufferHeight: %u", DeviceLimits->maxFramebufferHeight);
    ctk::print_line(1, "maxFramebufferLayers: %u", DeviceLimits->maxFramebufferLayers);
    ctk::print_line(1, "maxColorAttachments: %u", DeviceLimits->maxColorAttachments);
    ctk::print_line(1, "maxSampleMaskWords: %u", DeviceLimits->maxSampleMaskWords);
    ctk::print_line(1, "timestampComputeAndGraphics: %s", DeviceLimits->timestampComputeAndGraphics ? "true" : "false");
    ctk::print_line(1, "timestampPeriod: %f", DeviceLimits->timestampPeriod);
    ctk::print_line(1, "maxClipDistances: %u", DeviceLimits->maxClipDistances);
    ctk::print_line(1, "maxCullDistances: %u", DeviceLimits->maxCullDistances);
    ctk::print_line(1, "maxCombinedClipAndCullDistances: %u", DeviceLimits->maxCombinedClipAndCullDistances);
    ctk::print_line(1, "discreteQueuePriorities: %u", DeviceLimits->discreteQueuePriorities);
    ctk::print_line(1, "pointSizeRange[2]: { %f, %f }", DeviceLimits->pointSizeRange[0], DeviceLimits->pointSizeRange[1]);
    ctk::print_line(1, "lineWidthRange[2]: { %f, %f }", DeviceLimits->lineWidthRange[0], DeviceLimits->lineWidthRange[1]);
    ctk::print_line(1, "pointSizeGranularity: %f", DeviceLimits->pointSizeGranularity);
    ctk::print_line(1, "lineWidthGranularity: %f", DeviceLimits->lineWidthGranularity);
    ctk::print_line(1, "strictLines: %s", DeviceLimits->strictLines ? "true" : "false");
    ctk::print_line(1, "standardSampleLocations: %s", DeviceLimits->standardSampleLocations ? "true" : "false");
    ctk::print_line(1, "optimalBufferCopyOffsetAlignment: %llu", DeviceLimits->optimalBufferCopyOffsetAlignment);
    ctk::print_line(1, "optimalBufferCopyRowPitchAlignment: %llu", DeviceLimits->optimalBufferCopyRowPitchAlignment);
    ctk::print_line(1, "nonCoherentAtomSize: %llu", DeviceLimits->nonCoherentAtomSize);
}

} // vtk
