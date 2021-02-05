#pragma once

#include <vulkan/vulkan.h>
#include "ctk/ctk.h"

////////////////////////////////////////////////////////////
/// Data
////////////////////////////////////////////////////////////
enum {
	VTK_PHYSICAL_DEVICE_FEATURE_robustBufferAccess,
    VTK_PHYSICAL_DEVICE_FEATURE_fullDrawIndexUint32,
    VTK_PHYSICAL_DEVICE_FEATURE_imageCubeArray,
    VTK_PHYSICAL_DEVICE_FEATURE_independentBlend,
    VTK_PHYSICAL_DEVICE_FEATURE_geometryShader,
    VTK_PHYSICAL_DEVICE_FEATURE_tessellationShader,
    VTK_PHYSICAL_DEVICE_FEATURE_sampleRateShading,
    VTK_PHYSICAL_DEVICE_FEATURE_dualSrcBlend,
    VTK_PHYSICAL_DEVICE_FEATURE_logicOp,
    VTK_PHYSICAL_DEVICE_FEATURE_multiDrawIndirect,
    VTK_PHYSICAL_DEVICE_FEATURE_drawIndirectFirstInstance,
    VTK_PHYSICAL_DEVICE_FEATURE_depthClamp,
    VTK_PHYSICAL_DEVICE_FEATURE_depthBiasClamp,
    VTK_PHYSICAL_DEVICE_FEATURE_fillModeNonSolid,
    VTK_PHYSICAL_DEVICE_FEATURE_depthBounds,
    VTK_PHYSICAL_DEVICE_FEATURE_wideLines,
    VTK_PHYSICAL_DEVICE_FEATURE_largePoints,
    VTK_PHYSICAL_DEVICE_FEATURE_alphaToOne,
    VTK_PHYSICAL_DEVICE_FEATURE_multiViewport,
    VTK_PHYSICAL_DEVICE_FEATURE_samplerAnisotropy,
    VTK_PHYSICAL_DEVICE_FEATURE_textureCompressionETC2,
    VTK_PHYSICAL_DEVICE_FEATURE_textureCompressionASTC_LDR,
    VTK_PHYSICAL_DEVICE_FEATURE_textureCompressionBC,
    VTK_PHYSICAL_DEVICE_FEATURE_occlusionQueryPrecise,
    VTK_PHYSICAL_DEVICE_FEATURE_pipelineStatisticsQuery,
    VTK_PHYSICAL_DEVICE_FEATURE_vertexPipelineStoresAndAtomics,
    VTK_PHYSICAL_DEVICE_FEATURE_fragmentStoresAndAtomics,
    VTK_PHYSICAL_DEVICE_FEATURE_shaderTessellationAndGeometryPointSize,
    VTK_PHYSICAL_DEVICE_FEATURE_shaderImageGatherExtended,
    VTK_PHYSICAL_DEVICE_FEATURE_shaderStorageImageExtendedFormats,
    VTK_PHYSICAL_DEVICE_FEATURE_shaderStorageImageMultisample,
    VTK_PHYSICAL_DEVICE_FEATURE_shaderStorageImageReadWithoutFormat,
    VTK_PHYSICAL_DEVICE_FEATURE_shaderStorageImageWriteWithoutFormat,
    VTK_PHYSICAL_DEVICE_FEATURE_shaderUniformBufferArrayDynamicIndexing,
    VTK_PHYSICAL_DEVICE_FEATURE_shaderSampledImageArrayDynamicIndexing,
    VTK_PHYSICAL_DEVICE_FEATURE_shaderStorageBufferArrayDynamicIndexing,
    VTK_PHYSICAL_DEVICE_FEATURE_shaderStorageImageArrayDynamicIndexing,
    VTK_PHYSICAL_DEVICE_FEATURE_shaderClipDistance,
    VTK_PHYSICAL_DEVICE_FEATURE_shaderCullDistance,
    VTK_PHYSICAL_DEVICE_FEATURE_shaderFloat64,
    VTK_PHYSICAL_DEVICE_FEATURE_shaderInt64,
    VTK_PHYSICAL_DEVICE_FEATURE_shaderInt16,
    VTK_PHYSICAL_DEVICE_FEATURE_shaderResourceResidency,
    VTK_PHYSICAL_DEVICE_FEATURE_shaderResourceMinLod,
    VTK_PHYSICAL_DEVICE_FEATURE_sparseBinding,
    VTK_PHYSICAL_DEVICE_FEATURE_sparseResidencyBuffer,
    VTK_PHYSICAL_DEVICE_FEATURE_sparseResidencyImage2D,
    VTK_PHYSICAL_DEVICE_FEATURE_sparseResidencyImage3D,
    VTK_PHYSICAL_DEVICE_FEATURE_sparseResidency2Samples,
    VTK_PHYSICAL_DEVICE_FEATURE_sparseResidency4Samples,
    VTK_PHYSICAL_DEVICE_FEATURE_sparseResidency8Samples,
    VTK_PHYSICAL_DEVICE_FEATURE_sparseResidency16Samples,
    VTK_PHYSICAL_DEVICE_FEATURE_sparseResidencyAliased,
    VTK_PHYSICAL_DEVICE_FEATURE_variableMultisampleRate,
    VTK_PHYSICAL_DEVICE_FEATURE_inheritedQueries,
    VTK_PHYSICAL_DEVICE_FEATURE_COUNT,
};

typedef CTK_StaticArray<s32, VTK_PHYSICAL_DEVICE_FEATURE_COUNT> VTK_PhysicalDeviceFeatures;

static cstr const _VTK_PHYSICAL_DEVICE_FEATURE_NAMES[] = {
    CTK_TO_CSTR(robustBufferAccess),
    CTK_TO_CSTR(fullDrawIndexUint32),
    CTK_TO_CSTR(imageCubeArray),
    CTK_TO_CSTR(independentBlend),
    CTK_TO_CSTR(geometryShader),
    CTK_TO_CSTR(tessellationShader),
    CTK_TO_CSTR(sampleRateShading),
    CTK_TO_CSTR(dualSrcBlend),
    CTK_TO_CSTR(logicOp),
    CTK_TO_CSTR(multiDrawIndirect),
    CTK_TO_CSTR(drawIndirectFirstInstance),
    CTK_TO_CSTR(depthClamp),
    CTK_TO_CSTR(depthBiasClamp),
    CTK_TO_CSTR(fillModeNonSolid),
    CTK_TO_CSTR(depthBounds),
    CTK_TO_CSTR(wideLines),
    CTK_TO_CSTR(largePoints),
    CTK_TO_CSTR(alphaToOne),
    CTK_TO_CSTR(multiViewport),
    CTK_TO_CSTR(samplerAnisotropy),
    CTK_TO_CSTR(textureCompressionETC2),
    CTK_TO_CSTR(textureCompressionASTC_LDR),
    CTK_TO_CSTR(textureCompressionBC),
    CTK_TO_CSTR(occlusionQueryPrecise),
    CTK_TO_CSTR(pipelineStatisticsQuery),
    CTK_TO_CSTR(vertexPipelineStoresAndAtomics),
    CTK_TO_CSTR(fragmentStoresAndAtomics),
    CTK_TO_CSTR(shaderTessellationAndGeometryPointSize),
    CTK_TO_CSTR(shaderImageGatherExtended),
    CTK_TO_CSTR(shaderStorageImageExtendedFormats),
    CTK_TO_CSTR(shaderStorageImageMultisample),
    CTK_TO_CSTR(shaderStorageImageReadWithoutFormat),
    CTK_TO_CSTR(shaderStorageImageWriteWithoutFormat),
    CTK_TO_CSTR(shaderUniformBufferArrayDynamicIndexing),
    CTK_TO_CSTR(shaderSampledImageArrayDynamicIndexing),
    CTK_TO_CSTR(shaderStorageBufferArrayDynamicIndexing),
    CTK_TO_CSTR(shaderStorageImageArrayDynamicIndexing),
    CTK_TO_CSTR(shaderClipDistance),
    CTK_TO_CSTR(shaderCullDistance),
    CTK_TO_CSTR(shaderFloat64),
    CTK_TO_CSTR(shaderInt64),
    CTK_TO_CSTR(shaderInt16),
    CTK_TO_CSTR(shaderResourceResidency),
    CTK_TO_CSTR(shaderResourceMinLod),
    CTK_TO_CSTR(sparseBinding),
    CTK_TO_CSTR(sparseResidencyBuffer),
    CTK_TO_CSTR(sparseResidencyImage2D),
    CTK_TO_CSTR(sparseResidencyImage3D),
    CTK_TO_CSTR(sparseResidency2Samples),
    CTK_TO_CSTR(sparseResidency4Samples),
    CTK_TO_CSTR(sparseResidency8Samples),
    CTK_TO_CSTR(sparseResidency16Samples),
    CTK_TO_CSTR(sparseResidencyAliased),
    CTK_TO_CSTR(variableMultisampleRate),
    CTK_TO_CSTR(inheritedQueries),
};

////////////////////////////////////////////////////////////
/// Interface
////////////////////////////////////////////////////////////
static cstr vtk_physical_device_feature_name(s32 device_feature) {
    CTK_ASSERT(device_feature < VTK_PHYSICAL_DEVICE_FEATURE_COUNT);
    return _VTK_PHYSICAL_DEVICE_FEATURE_NAMES[device_feature];
}

static bool vtk_physical_device_feature_supported(s32 device_feature, VkPhysicalDeviceFeatures *physical_device_features) {
    CTK_ASSERT(device_feature < VTK_PHYSICAL_DEVICE_FEATURE_COUNT);
    return ((VkBool32 *)physical_device_features)[device_feature];
}
