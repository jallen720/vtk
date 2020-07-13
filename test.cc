#include <windows.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "vtk/vtk.h"
#include "ctk/ctk.h"
#include "ctk/data.h"
#include "ctk/math.h"

////////////////////////////////////////////////////////////
/// Constants
////////////////////////////////////////////////////////////
static const u32 MEGABYTE = 1000000;
static const ctk::vec2<f64> UNSET_MOUSE_POSITION = { -10000.0, -10000.0 };

////////////////////////////////////////////////////////////
/// Data
////////////////////////////////////////////////////////////
struct app_state
{
    b32 KeyDown[GLFW_KEY_LAST + 1];
    b32 MouseButtonDown[GLFW_MOUSE_BUTTON_LAST + 1];
    ctk::vec2<f64> MousePosition = UNSET_MOUSE_POSITION;
    ctk::vec2<f64> MouseDelta;
};

struct vertex
{
    ctk::vec3<f32> Position;
    ctk::vec2<f32> UV;
};

struct entity_ubo
{
    alignas(16) glm::mat4 ModelMatrix;
    alignas(16) glm::mat4 MVPMatrix;
};

struct view_ubo
{
    alignas(16) glm::mat4 ViewMatrix;
    alignas(16) glm::mat4 ViewProjectionMatrix;
};

struct mesh
{
    ctk::sarray<vertex, 24> Vertexes;
    ctk::sarray<u32, 36> Indexes;
    vtk::region VertexRegion;
    vtk::region IndexRegion;
};

struct render_entity
{
    ctk::sarray<vtk::descriptor_set *, 4> DescriptorSets;
    vtk::graphics_pipeline *GraphicsPipeline;
    mesh *Mesh;
};

////////////////////////////////////////////////////////////
/// Internal
////////////////////////////////////////////////////////////
static void
ErrorCallback(s32 Error, cstr Description)
{
    CTK_FATAL("[%d] %s", Error, Description)
}

static void
KeyCallback(GLFWwindow *Window, s32 Key, s32 Scancode, s32 Action, s32 Mods)
{
    auto AppState = (app_state *)glfwGetWindowUserPointer(Window);
    AppState->KeyDown[Key] = Action == GLFW_PRESS || Action == GLFW_REPEAT;
}

static void
MouseButtonCallback(GLFWwindow *Window, s32 button, s32 Action, s32 Mods)
{
    auto AppState = (app_state *)glfwGetWindowUserPointer(Window);
    AppState->MouseButtonDown[button] = Action == GLFW_PRESS || Action == GLFW_REPEAT;
}

////////////////////////////////////////////////////////////
/// Main
////////////////////////////////////////////////////////////
s32
main()
{
    app_state AppState = {};
    ctk::data Config = ctk::LoadData("test_assets/test_config.ctkd");

    ////////////////////////////////////////////////////////////
    /// Window
    ////////////////////////////////////////////////////////////
    glfwSetErrorCallback(ErrorCallback);
    if(glfwInit() != GLFW_TRUE)
    {
        CTK_FATAL("failed to init GLFW")
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    GLFWwindow *Window = glfwCreateWindow(S32(&Config, "window.width"),
                                          S32(&Config, "window.height"),
                                          CStr(&Config, "window.title"),
                                          NULL, NULL);
    if(Window == NULL)
    {
        CTK_FATAL("failed to create window")
    }
    glfwSetWindowPos(Window, S32(&Config, "window.x"), S32(&Config, "window.y"));
    glfwSetWindowUserPointer(Window, (void *)&AppState);
    // glfwSetFramebufferSizeCallback(Window, FramebufferResizeCallback);
    glfwSetKeyCallback(Window, KeyCallback);
    glfwSetMouseButtonCallback(Window, MouseButtonCallback);

    ////////////////////////////////////////////////////////////
    /// Vulkan Setup
    ////////////////////////////////////////////////////////////

    // Instance
    u32 GLFWExtensionCount = 0;
    cstr *GLFWExtensions = glfwGetRequiredInstanceExtensions(&GLFWExtensionCount);
    vtk::instance_info InstanceInfo = {};
    ctk::Push(&InstanceInfo.Extensions, GLFWExtensions, GLFWExtensionCount);
    InstanceInfo.Debug = ctk::B32(&Config, "debug");
    InstanceInfo.AppName = ctk::CStr(&Config, "app_name");
    vtk::instance Instance = vtk::CreateInstance(&InstanceInfo);

    // Platform Surface
    VkSurfaceKHR PlatformSurface = {};
    VkResult Result = glfwCreateWindowSurface(Instance.Handle, Window, NULL, &PlatformSurface);
    vtk::ValidateVkResult(Result, "glfwCreateWindowSurface", "failed to create GLFW surface");

    // Device
    vtk::device_info DeviceInfo = {};
    Push(&DeviceInfo.Extensions, VK_KHR_SWAPCHAIN_EXTENSION_NAME); // Swapchains required for rendering.
    DeviceInfo.Features.geometryShader = VK_TRUE;
    DeviceInfo.Features.samplerAnisotropy = VK_TRUE;
    // DeviceInfo.Features.vertexPipelineStoresAndAtomics = VK_TRUE;
    vtk::device Device = vtk::CreateDevice(Instance.Handle, PlatformSurface, &DeviceInfo);

    // Swapchain
    vtk::swapchain Swapchain = vtk::CreateSwapchain(&Device, PlatformSurface);

    // Graphics Command Pool
    VkCommandPool GraphicsCommandPool = vtk::CreateCommandPool(Device.Logical, Device.QueueFamilyIndexes.Graphics);

    ////////////////////////////////////////////////////////////
    /// Frame State
    ////////////////////////////////////////////////////////////
    vtk::frame_state FrameState = vtk::CreateFrameState(Device.Logical, 2, Swapchain.Images.Count);

    ////////////////////////////////////////////////////////////
    /// Depth Image
    ////////////////////////////////////////////////////////////
    vtk::image_info DepthImageInfo = {};
    DepthImageInfo.Width = Swapchain.Extent.width;
    DepthImageInfo.Height = Swapchain.Extent.height;
    DepthImageInfo.Format = vtk::FindDepthImageFormat(Device.Physical);
    DepthImageInfo.Tiling = VK_IMAGE_TILING_OPTIMAL;
    DepthImageInfo.UsageFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    DepthImageInfo.MemoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    DepthImageInfo.AspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    vtk::image DepthImage = vtk::CreateImage(&Device, &DepthImageInfo);
    vtk::TransitionImageLayout(&Device, GraphicsCommandPool, &DepthImage,
                               VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    ////////////////////////////////////////////////////////////
    /// Render Pass
    ////////////////////////////////////////////////////////////
    vtk::render_pass_info RenderPassInfo = {};

    // Attachments
    u32 ColorAttachmentIndex = RenderPassInfo.Attachments.Count;
    vtk::attachment *ColorAttachment = ctk::Push(&RenderPassInfo.Attachments);
    ColorAttachment->Description.format = Swapchain.ImageFormat;
    ColorAttachment->Description.samples = VK_SAMPLE_COUNT_1_BIT;
    ColorAttachment->Description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // Clear color attachment before drawing.
    ColorAttachment->Description.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // Store rendered contents in memory.
    ColorAttachment->Description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // Not currently relevant.
    ColorAttachment->Description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // Not currently relevant.
    ColorAttachment->Description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // Image layout before render pass.
    ColorAttachment->Description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    ColorAttachment->ClearValue = { 0.04f, 0.04f, 0.04f, 1.0f };

    u32 DepthAttachmentIndex = RenderPassInfo.Attachments.Count;
    vtk::attachment *DepthAttachment = ctk::Push(&RenderPassInfo.Attachments);
    DepthAttachment->Description.format = DepthImage.Format;
    DepthAttachment->Description.samples = VK_SAMPLE_COUNT_1_BIT;
    DepthAttachment->Description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // Clear color attachment before drawing.
    DepthAttachment->Description.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // Store rendered contents in memory.
    DepthAttachment->Description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // Not currently relevant.
    DepthAttachment->Description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // Not currently relevant.
    DepthAttachment->Description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // Image layout before render pass.
    DepthAttachment->Description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    DepthAttachment->ClearValue = { 1.0f, 0.0f };

    // Subpasses
    vtk::subpass *Subpass = ctk::Push(&RenderPassInfo.Subpasses);

    VkAttachmentReference *ColorAttachmentReference = ctk::Push(&Subpass->ColorAttachmentReferences);
    ColorAttachmentReference->attachment = ColorAttachmentIndex;
    ColorAttachmentReference->layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    Subpass->DepthAttachmentReference.Set = true;
    Subpass->DepthAttachmentReference.Value.attachment = DepthAttachmentIndex;
    Subpass->DepthAttachmentReference.Value.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // Creation
    vtk::render_pass RenderPass = vtk::CreateRenderPass(Device.Logical, &RenderPassInfo);

    // Framebuffers
    ctk::sarray<VkFramebuffer, 4> Framebuffers = {};
    for(u32 FramebufferIndex = 0; FramebufferIndex < Swapchain.Images.Count; ++FramebufferIndex)
    {
        vtk::framebuffer_info FramebufferInfo = {};
        ctk::Push(&FramebufferInfo.Attachments, Swapchain.Images[FramebufferIndex].View);
        ctk::Push(&FramebufferInfo.Attachments, DepthImage.View);
        FramebufferInfo.Extent = Swapchain.Extent;
        FramebufferInfo.Layers = 1;
        ctk::Push(&Framebuffers, vtk::CreateFramebuffer(Device.Logical, RenderPass.Handle, &FramebufferInfo));
    }

    // Command Buffers
    ctk::sarray<VkCommandBuffer, 4> CommandBuffers = {};
    vtk::AllocateCommandBuffers(Device.Logical, GraphicsCommandPool, Swapchain.Images.Count, CommandBuffers.Data);

    ////////////////////////////////////////////////////////////
    /// Shader Modules
    ////////////////////////////////////////////////////////////
    vtk::shader_module VertexShader =
        vtk::CreateShaderModule(Device.Logical, "test_assets/shaders/shader.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    vtk::shader_module FragmentShader =
        vtk::CreateShaderModule(Device.Logical, "test_assets/shaders/shader.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

    ////////////////////////////////////////////////////////////
    /// Vertex Layout
    ////////////////////////////////////////////////////////////
    vtk::vertex_layout VertexLayout = {};
    u32 VertexPositionIndex = vtk::PushVertexAttribute(&VertexLayout, 3);
    u32 VertexUVIndex = vtk::PushVertexAttribute(&VertexLayout, 2);

    ////////////////////////////////////////////////////////////
    /// Memory
    ////////////////////////////////////////////////////////////

    // Buffers
    vtk::buffer_info HostBufferInfo = {};
    HostBufferInfo.Size = 10 * MEGABYTE;
    HostBufferInfo.UsageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    HostBufferInfo.MemoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    vtk::buffer HostBuffer = vtk::CreateBuffer(&Device, &HostBufferInfo);

    vtk::buffer_info DeviceBufferInfo = {};
    DeviceBufferInfo.Size = 10 * MEGABYTE;
    DeviceBufferInfo.UsageFlags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    DeviceBufferInfo.MemoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    vtk::buffer DeviceBuffer = vtk::CreateBuffer(&Device, &DeviceBufferInfo);

    // Regions
    vtk::region StagingRegion = vtk::AllocateRegion(&HostBuffer, 2 * MEGABYTE);

    ////////////////////////////////////////////////////////////
    /// Descriptor Set Resources
    ////////////////////////////////////////////////////////////

    // Uniform Buffers
    vtk::uniform_buffer EntityUniformBuffer = vtk::CreateUniformBuffer(&HostBuffer, 2, sizeof(entity_ubo), FrameState.Frames.Count);

    // Textures
    vtk::texture_info GrassTextureInfo = {};
    GrassTextureInfo.Filter = VK_FILTER_NEAREST;
    vtk::texture GrassTexture = vtk::LoadTexture(&Device, GraphicsCommandPool, &StagingRegion, "test_assets/textures/grass.jpg",
                                                 &GrassTextureInfo);
    vtk::texture_info DirtTextureInfo = {};
    DirtTextureInfo.Filter = VK_FILTER_NEAREST;
    vtk::texture DirtTexture = vtk::LoadTexture(&Device, GraphicsCommandPool, &StagingRegion, "test_assets/textures/dirt.jpg",
                                                &DirtTextureInfo);

    ////////////////////////////////////////////////////////////
    /// Descriptor Sets
    ////////////////////////////////////////////////////////////

    // Descriptor Infos
    vtk::descriptor_info EntityDescriptorInfo = {};
    EntityDescriptorInfo.Type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    EntityDescriptorInfo.ShaderStageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    EntityDescriptorInfo.Count = 1;
    EntityDescriptorInfo.UniformBuffer = &EntityUniformBuffer;

    vtk::descriptor_info GrassTextureDescriptorInfo = {};
    GrassTextureDescriptorInfo.Type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    GrassTextureDescriptorInfo.ShaderStageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    GrassTextureDescriptorInfo.Count = 1;
    GrassTextureDescriptorInfo.Texture = &GrassTexture;

    vtk::descriptor_info DirtTextureDescriptorInfo = {};
    DirtTextureDescriptorInfo.Type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    DirtTextureDescriptorInfo.ShaderStageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    DirtTextureDescriptorInfo.Count = 1;
    DirtTextureDescriptorInfo.Texture = &DirtTexture;

    // Descriptor Set Info
    ctk::smap<vtk::descriptor_set_info, 4> DescriptorSetInfos = {};

    vtk::descriptor_set_info *EntityDescriptorSetInfo = ctk::Push(&DescriptorSetInfos, "entity");
    EntityDescriptorSetInfo->InstanceCount = FrameState.Frames.Count;
    ctk::Push(&EntityDescriptorSetInfo->DescriptorBindings, { 0, &EntityDescriptorInfo });

    vtk::descriptor_set_info *GrassTextureDescriptorSetInfo = ctk::Push(&DescriptorSetInfos, "grass_texture");
    GrassTextureDescriptorSetInfo->InstanceCount = 1;
    ctk::Push(&GrassTextureDescriptorSetInfo->DescriptorBindings, { 0, &GrassTextureDescriptorInfo });

    vtk::descriptor_set_info *DirtTextureDescriptorSetInfo = ctk::Push(&DescriptorSetInfos, "dirt_texture");
    DirtTextureDescriptorSetInfo->InstanceCount = 1;
    ctk::Push(&DirtTextureDescriptorSetInfo->DescriptorBindings, { 0, &DirtTextureDescriptorInfo });

    // Pool
    VkDescriptorPool DescriptorPool = vtk::CreateDescriptorPool(Device.Logical, DescriptorSetInfos.Values, DescriptorSetInfos.Count);

    // Mirror descriptor set infos map to store descriptor sets.
    ctk::smap<vtk::descriptor_set, 4> DescriptorSets = {};
    for(u32 DescriptorSetIndex = 0; DescriptorSetIndex < DescriptorSetInfos.Count; ++DescriptorSetIndex)
    {
        ctk::Push(&DescriptorSets, DescriptorSetInfos.Keys[DescriptorSetIndex]);
    }
    vtk::CreateDescriptorSets(Device.Logical, DescriptorPool, DescriptorSetInfos.Values, DescriptorSetInfos.Count, DescriptorSets.Values);

    ////////////////////////////////////////////////////////////
    /// Graphics Pipelines
    ////////////////////////////////////////////////////////////
    vtk::graphics_pipeline_info GraphicsPipelineInfo = {};
    ctk::Push(&GraphicsPipelineInfo.ShaderModules, &VertexShader);
    ctk::Push(&GraphicsPipelineInfo.ShaderModules, &FragmentShader);
    ctk::Push(&GraphicsPipelineInfo.VertexInputs, { 0, 0, VertexPositionIndex });
    ctk::Push(&GraphicsPipelineInfo.VertexInputs, { 1, 0, VertexUVIndex });
    ctk::Push(&GraphicsPipelineInfo.DescriptorSetLayouts, At(&DescriptorSets, "entity")->Layout);
    ctk::Push(&GraphicsPipelineInfo.DescriptorSetLayouts, At(&DescriptorSets, "grass_texture")->Layout);
    GraphicsPipelineInfo.VertexLayout = &VertexLayout;
    GraphicsPipelineInfo.ViewportExtent = Swapchain.Extent;
    GraphicsPipelineInfo.PrimitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    GraphicsPipelineInfo.DepthTesting = VK_TRUE;
    vtk::graphics_pipeline GraphicsPipeline = vtk::CreateGraphicsPipeline(Device.Logical, RenderPass.Handle, &GraphicsPipelineInfo);

    ////////////////////////////////////////////////////////////
    /// Data
    ////////////////////////////////////////////////////////////

    // Meshes
    mesh Meshes[2] = {};
    mesh *QuadMesh = Meshes + 0;
    mesh *CubeMesh = Meshes + 1;
    u32 QuadIndexes[] = { 0, 1, 2, 0, 2, 3 };
    u32 CubeIndexes[] =
    {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
    };
    ctk::Push(&QuadMesh->Vertexes, { { 0.0f,  0.0f, 0.0f }, { 0.0f, 1.0f } });
    ctk::Push(&QuadMesh->Vertexes, { { 1.0f,  0.0f, 0.0f }, { 1.0f, 1.0f } });
    ctk::Push(&QuadMesh->Vertexes, { { 1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } });
    ctk::Push(&QuadMesh->Vertexes, { { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } });
    ctk::Push(&QuadMesh->Indexes, QuadIndexes, CTK_ARRAY_COUNT(QuadIndexes));
    ctk::Push(&CubeMesh->Vertexes, { { 0.0f,  0.0f, 0.0f }, { 0.0f, 1.0f } });
    ctk::Push(&CubeMesh->Vertexes, { { 1.0f,  0.0f, 0.0f }, { 1.0f, 1.0f } });
    ctk::Push(&CubeMesh->Vertexes, { { 1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } });
    ctk::Push(&CubeMesh->Vertexes, { { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } });
    ctk::Push(&CubeMesh->Vertexes, { { 0.0f,  0.0f, 1.0f }, { 0.0f, 1.0f } });
    ctk::Push(&CubeMesh->Vertexes, { { 0.0f,  0.0f, 0.0f }, { 1.0f, 1.0f } });
    ctk::Push(&CubeMesh->Vertexes, { { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } });
    ctk::Push(&CubeMesh->Vertexes, { { 0.0f, -1.0f, 1.0f }, { 0.0f, 0.0f } });
    ctk::Push(&CubeMesh->Indexes, CubeIndexes, CTK_ARRAY_COUNT(CubeIndexes));
    for(u32 MeshIndex = 0; MeshIndex < CTK_ARRAY_COUNT(Meshes); ++MeshIndex)
    {
        mesh *Mesh = Meshes + MeshIndex;
        u32 VertexByteCount = ctk::ByteCount(&Mesh->Vertexes);
        u32 IndexByteCount = ctk::ByteCount(&Mesh->Indexes);
        Mesh->VertexRegion = vtk::AllocateRegion(&DeviceBuffer, VertexByteCount);
        Mesh->IndexRegion = vtk::AllocateRegion(&DeviceBuffer, IndexByteCount);
        vtk::WriteToDeviceRegion(&Device, GraphicsCommandPool, &StagingRegion, &Mesh->VertexRegion,
                                 Mesh->Vertexes.Data, VertexByteCount, 0);
        vtk::WriteToDeviceRegion(&Device, GraphicsCommandPool, &StagingRegion, &Mesh->IndexRegion,
                                 Mesh->Indexes.Data, IndexByteCount, 0);
    }

    ////////////////////////////////////////////////////////////
    /// Scene
    ////////////////////////////////////////////////////////////
    ctk::sarray<render_entity, 4> RenderEntities = {};
    render_entity *QuadEntity = ctk::Push(&RenderEntities);
    ctk::Push(&QuadEntity->DescriptorSets, At(&DescriptorSets, "entity"));
    ctk::Push(&QuadEntity->DescriptorSets, At(&DescriptorSets, "grass_texture"));
    QuadEntity->GraphicsPipeline = &GraphicsPipeline;
    QuadEntity->Mesh = QuadMesh;

    render_entity *CubeEntity = ctk::Push(&RenderEntities);
    ctk::Push(&CubeEntity->DescriptorSets, At(&DescriptorSets, "entity"));
    ctk::Push(&CubeEntity->DescriptorSets, At(&DescriptorSets, "dirt_texture"));
    CubeEntity->GraphicsPipeline = &GraphicsPipeline;
    CubeEntity->Mesh = CubeMesh;

    ////////////////////////////////////////////////////////////
    /// Record render pass.
    ////////////////////////////////////////////////////////////
    VkRect2D RenderArea = {};
    RenderArea.offset.x = 0;
    RenderArea.offset.y = 0;
    RenderArea.extent = Swapchain.Extent;

    VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
    CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    CommandBufferBeginInfo.flags = 0;
    CommandBufferBeginInfo.pInheritanceInfo = NULL;

    for(u32 FrameIndex = 0; FrameIndex < FrameState.Frames.Count; ++FrameIndex)
    {
        VkCommandBuffer CommandBuffer = CommandBuffers[FrameIndex];
        vtk::ValidateVkResult(vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo),
                              "vkBeginCommandBuffer", "failed to begin recording command buffer");
        VkRenderPassBeginInfo RenderPassBeginInfo = {};
        RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        RenderPassBeginInfo.renderPass = RenderPass.Handle;
        RenderPassBeginInfo.framebuffer = Framebuffers[FrameIndex];
        RenderPassBeginInfo.renderArea = RenderArea;
        RenderPassBeginInfo.clearValueCount = RenderPass.ClearValues.Count;
        RenderPassBeginInfo.pClearValues = RenderPass.ClearValues.Data;

        // Begin
        vkCmdBeginRenderPass(CommandBuffer, &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Render Commands
        for(u32 EntityIndex = 0; EntityIndex < RenderEntities.Count; ++EntityIndex)
        {
            render_entity *RenderEntity = RenderEntities + EntityIndex;
            mesh *Mesh = RenderEntity->Mesh;

            // Graphics Pipeline
            vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, RenderEntity->GraphicsPipeline->Handle);

            // Descriptor Sets
            vtk::BindDescriptorSets(CommandBuffer, RenderEntity->GraphicsPipeline->Layout,
                                    RenderEntity->DescriptorSets.Data, RenderEntity->DescriptorSets.Count,
                                    FrameIndex, EntityIndex);

            // Vertex/Index Buffers
            vkCmdBindVertexBuffers(CommandBuffer, 0, 1, &Mesh->VertexRegion.Buffer->Handle, &Mesh->VertexRegion.Offset);
            vkCmdBindIndexBuffer(CommandBuffer, Mesh->IndexRegion.Buffer->Handle, Mesh->IndexRegion.Offset, VK_INDEX_TYPE_UINT32);

            // Draw
            vkCmdDrawIndexed(CommandBuffer, Mesh->Indexes.Count, 1, 0, 0, 0);
        }

        // End
        vkCmdEndRenderPass(CommandBuffer);
        vtk::ValidateVkResult(vkEndCommandBuffer(CommandBuffer), "vkEndCommandBuffer", "error during render pass command recording");
    }

    ////////////////////////////////////////////////////////////
    /// Main Loop
    ////////////////////////////////////////////////////////////
    b32 Close = false;
    glm::vec3 CameraPosition = { 0.0f, 0.0f, -1.0f };
    glm::vec3 CameraRotation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 EntityPositions[2] =
    {
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 3.0f },
    };
    while(!glfwWindowShouldClose(Window) && !Close)
    {
        ////////////////////////////////////////////////////////////
        /// Input
        ////////////////////////////////////////////////////////////
        glfwPollEvents();
        if(AppState.KeyDown[GLFW_KEY_ESCAPE])
        {
            Close = true;
        }

        // Mouse Delta
        ctk::vec2 PreviousMousePosition = AppState.MousePosition;
        f64 CurrentMouseX = 0.0;
        f64 CurrentMouseY = 0.0;
        glfwGetCursorPos(Window, &CurrentMouseX, &CurrentMouseY);
        AppState.MousePosition = { CurrentMouseX, CurrentMouseY };

        // Calculate delta if previous position was not unset.
        if(PreviousMousePosition != UNSET_MOUSE_POSITION)
        {
            AppState.MouseDelta = AppState.MousePosition - PreviousMousePosition;
        }

        ////////////////////////////////////////////////////////////
        /// Camera Controls
        ////////////////////////////////////////////////////////////

        // Rotation
        if(AppState.MouseButtonDown[GLFW_MOUSE_BUTTON_2])
        {
            static const f32 SENS = 0.4f;
            CameraRotation.x += AppState.MouseDelta.Y * SENS;
            CameraRotation.y -= AppState.MouseDelta.X * SENS;
            CameraRotation.x = ctk::Clamp(CameraRotation.x, -80.0f, 80.0f);
        }

        // Translation
        glm::vec3 Translation = {};
        f32 Modifier = AppState.KeyDown[GLFW_KEY_LEFT_SHIFT] ? 4 :
                       AppState.KeyDown[GLFW_KEY_LEFT_CONTROL] ? 1 :
                       2;
        if(AppState.KeyDown[GLFW_KEY_W]) Translation.z += 0.01f * Modifier;
        if(AppState.KeyDown[GLFW_KEY_S]) Translation.z -= 0.01f * Modifier;
        if(AppState.KeyDown[GLFW_KEY_D]) Translation.x += 0.01f * Modifier;
        if(AppState.KeyDown[GLFW_KEY_A]) Translation.x -= 0.01f * Modifier;
        if(AppState.KeyDown[GLFW_KEY_E]) Translation.y -= 0.01f * Modifier;
        if(AppState.KeyDown[GLFW_KEY_Q]) Translation.y += 0.01f * Modifier;

        glm::mat4 CameraWorldMatrix(1.0f);
        CameraWorldMatrix = glm::rotate(CameraWorldMatrix, glm::radians(CameraRotation.x), { 1.0f, 0.0f, 0.0f });
        CameraWorldMatrix = glm::rotate(CameraWorldMatrix, glm::radians(CameraRotation.y), { 0.0f, 1.0f, 0.0f });
        CameraWorldMatrix = glm::rotate(CameraWorldMatrix, glm::radians(CameraRotation.z), { 0.0f, 0.0f, 1.0f });
        CameraWorldMatrix = glm::translate(CameraWorldMatrix, { CameraPosition.x, CameraPosition.y, CameraPosition.z });

        glm::vec3 Right = {};
        Right.x = CameraWorldMatrix[0][0];
        Right.y = CameraWorldMatrix[1][0];
        Right.z = CameraWorldMatrix[2][0];

        glm::vec3 Up = {};
        Up.x = CameraWorldMatrix[0][1];
        Up.y = CameraWorldMatrix[1][1];
        Up.z = CameraWorldMatrix[2][1];

        glm::vec3 Forward = {};
        Forward.x = CameraWorldMatrix[0][2];
        Forward.y = CameraWorldMatrix[1][2];
        Forward.z = CameraWorldMatrix[2][2];

        glm::vec3 NewPosition = CameraPosition;
        NewPosition = NewPosition + (Right * Translation.x);
        NewPosition = NewPosition + (Up * Translation.y);
        CameraPosition = NewPosition + (Forward * Translation.z);

        ////////////////////////////////////////////////////////////
        /// Update Uniform Data
        ////////////////////////////////////////////////////////////
        entity_ubo EntityUBOs[2] = {};

        // View Matrix
        glm::mat4 CameraMatrix(1.0f);
        CameraMatrix = glm::rotate(CameraMatrix, glm::radians(CameraRotation.x), { 1.0f, 0.0f, 0.0f });
        CameraMatrix = glm::rotate(CameraMatrix, glm::radians(CameraRotation.y), { 0.0f, 1.0f, 0.0f });
        CameraMatrix = glm::rotate(CameraMatrix, glm::radians(CameraRotation.z), { 0.0f, 0.0f, 1.0f });
        CameraMatrix = glm::translate(CameraMatrix, CameraPosition);
        glm::vec3 CameraForward = { CameraMatrix[0][2], CameraMatrix[1][2], CameraMatrix[2][2] };
        glm::mat4 ViewMatrix = glm::lookAt(CameraPosition, CameraPosition + CameraForward, { 0.0f, -1.0f, 0.0f });

        // Projection Matrix
        f32 Aspect = Swapchain.Extent.width / (f32)Swapchain.Extent.height;
        glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(90.0f), Aspect, 0.1f, 1000.0f);
        ProjectionMatrix[1][1] *= -1; // Flip y value for scale (glm is designed for OpenGL).

        // Entity Model Matrixes
        for(u32 EntityIndex = 0; EntityIndex < 2; ++EntityIndex)
        {
            glm::mat4 ModelMatrix(1.0f);
            ModelMatrix = glm::translate(ModelMatrix, EntityPositions[EntityIndex]);
            ModelMatrix = glm::rotate(ModelMatrix, glm::radians(0.0f), { 1.0f, 0.0f, 0.0f });
            ModelMatrix = glm::rotate(ModelMatrix, glm::radians(0.0f), { 0.0f, 1.0f, 0.0f });
            ModelMatrix = glm::rotate(ModelMatrix, glm::radians(0.0f), { 0.0f, 0.0f, 1.0f });
            ModelMatrix = glm::scale(ModelMatrix, { 1.0f, 1.0f, 1.0f });
            EntityUBOs[EntityIndex].ModelMatrix = ModelMatrix;
            EntityUBOs[EntityIndex].MVPMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix;
        }
        vtk::WriteToHostRegion(Device.Logical, EntityUniformBuffer.Regions + FrameState.CurrentFrameIndex, EntityUBOs, sizeof(EntityUBOs), 0);

        ////////////////////////////////////////////////////////////
        /// Rendering
        ////////////////////////////////////////////////////////////
        vtk::frame *CurrentFrame = FrameState.Frames + FrameState.CurrentFrameIndex;

        // Wait on current frame's fence if still unsignaled.
        vkWaitForFences(Device.Logical, 1, &CurrentFrame->InFlightFence, VK_TRUE, UINT64_MAX);

        // Aquire next swapchain image index, using a semaphore to signal when image is available for rendering.
        u32 SwapchainImageIndex = VTK_UNSET_INDEX;
        {
            VkResult Result = vkAcquireNextImageKHR(Device.Logical, Swapchain.Handle, UINT64_MAX, CurrentFrame->ImageAquiredSemaphore,
                                                    VK_NULL_HANDLE, &SwapchainImageIndex);
            vtk::ValidateVkResult(Result, "vkAcquireNextImageKHR", "failed to aquire next swapchain image");
        }

        // Wait on swapchain images previously associated frame fence before rendering.
        VkFence *PreviousFrameInFlightFence = FrameState.PreviousFrameInFlightFences + SwapchainImageIndex;
        if(*PreviousFrameInFlightFence != VK_NULL_HANDLE)
        {
            vkWaitForFences(Device.Logical, 1, PreviousFrameInFlightFence, VK_TRUE, UINT64_MAX);
        }
        vkResetFences(Device.Logical, 1, &CurrentFrame->InFlightFence);
        *PreviousFrameInFlightFence = CurrentFrame->InFlightFence;

        ////////////////////////////////////////////////////////////
        /// Submit Render Pass Command Buffer
        ////////////////////////////////////////////////////////////
        VkSemaphore QueueSubmitWaitSemaphores[] = { CurrentFrame->ImageAquiredSemaphore };
        VkPipelineStageFlags QueueSubmitWaitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        VkSemaphore QueueSubmitSignalSemaphores[] = { CurrentFrame->RenderFinishedSemaphore };
        VkCommandBuffer QueueSubmitCommandBuffers[] = { CommandBuffers[SwapchainImageIndex] };

        VkSubmitInfo SubmitInfos[1] = {};
        SubmitInfos[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        SubmitInfos[0].waitSemaphoreCount = CTK_ARRAY_COUNT(QueueSubmitWaitSemaphores);
        SubmitInfos[0].pWaitSemaphores = QueueSubmitWaitSemaphores;
        SubmitInfos[0].pWaitDstStageMask = QueueSubmitWaitStages;
        SubmitInfos[0].commandBufferCount = CTK_ARRAY_COUNT(QueueSubmitCommandBuffers);
        SubmitInfos[0].pCommandBuffers = QueueSubmitCommandBuffers;
        SubmitInfos[0].signalSemaphoreCount = CTK_ARRAY_COUNT(QueueSubmitSignalSemaphores);
        SubmitInfos[0].pSignalSemaphores = QueueSubmitSignalSemaphores;
        {
            // Submit render pass commands to graphics queue for rendering.
            // Signal current frame's in flight flence when commands have finished executing.
            VkResult Result = vkQueueSubmit(Device.GraphicsQueue, CTK_ARRAY_COUNT(SubmitInfos), SubmitInfos, CurrentFrame->InFlightFence);
            vtk::ValidateVkResult(Result, "vkQueueSubmit", "failed to submit command buffer to graphics queue");
        }

        ////////////////////////////////////////////////////////////
        /// Presentation
        ////////////////////////////////////////////////////////////

        // These are parallel; provide 1:1 index per swapchain.
        VkSwapchainKHR Swapchains[] = { Swapchain.Handle };
        u32 SwapchainImageIndexes[] = { SwapchainImageIndex };

        VkPresentInfoKHR PresentInfo = {};
        PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        PresentInfo.waitSemaphoreCount = CTK_ARRAY_COUNT(QueueSubmitSignalSemaphores);
        PresentInfo.pWaitSemaphores = QueueSubmitSignalSemaphores;
        PresentInfo.swapchainCount = CTK_ARRAY_COUNT(Swapchains);
        PresentInfo.pSwapchains = Swapchains;
        PresentInfo.pImageIndices = SwapchainImageIndexes;
        PresentInfo.pResults = NULL;
        {
            // Submit Swapchains to present queue for presentation once rendering is complete.
            VkResult Result = vkQueuePresentKHR(Device.PresentQueue, &PresentInfo);
            vtk::ValidateVkResult(Result, "vkQueuePresentKHR", "failed to queue image for presentation");
        }

        // Cycle frame.
        FrameState.CurrentFrameIndex = (FrameState.CurrentFrameIndex + 1) % FrameState.Frames.Count;

        Sleep(1);
    }
}
