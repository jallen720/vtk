#include <windows.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vtk/vtk.h"
#include "ctk/ctk.h"
#include "ctk/data.h"
#include "ctk/math.h"

////////////////////////////////////////////////////////////
/// Data
////////////////////////////////////////////////////////////
struct app_state
{
    b32            KeyDown[GLFW_KEY_LAST + 1];
    b32            MouseButtonDown[GLFW_MOUSE_BUTTON_LAST + 1];
    ctk::vec2<f64> MousePosition;
    ctk::vec2<f64> MouseDelta;
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
    ctk::data Config = ctk::LoadData("assets/test_config.ctkd");

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
    vtk::instance_config InstanceConfig = {};
    ctk::Push(&InstanceConfig.Extensions, GLFWExtensions, GLFWExtensionCount);
    InstanceConfig.Debug = ctk::B32(&Config, "debug");
    InstanceConfig.AppName = ctk::CStr(&Config, "app_name");
    vtk::instance Instance = vtk::CreateInstance(&InstanceConfig);

    // Platform Surface
    VkSurfaceKHR PlatformSurface = {};
    VkResult Result = glfwCreateWindowSurface(Instance.Instance, Window, NULL, &PlatformSurface);
    vtk::ValidateVkResult(Result, "glfwCreateWindowSurface", "failed to create GLFW surface");

    // Device
    vtk::device_config DeviceConfig = {};
    Push(&DeviceConfig.Extensions, VK_KHR_SWAPCHAIN_EXTENSION_NAME); // Swapchains required for rendering.
    DeviceConfig.Features.geometryShader = VK_TRUE;
    DeviceConfig.Features.samplerAnisotropy = VK_TRUE;
    // DeviceConfig.Features.vertexPipelineStoresAndAtomics = VK_TRUE;
    vtk::device Device = vtk::CreateDevice(Instance.Instance, PlatformSurface, &DeviceConfig);

    // Swapchain
    vtk::swapchain Swapchain = vtk::CreateSwapchain(&Device, PlatformSurface);

    // Graphics Command Pool
    VkCommandPool GraphicsCommandPool = vtk::CreateCommandPool(Device.Logical, Device.QueueFamilyIndexes.Graphics);

    ////////////////////////////////////////////////////////////
    /// Data
    ////////////////////////////////////////////////////////////
    struct vertex
    {
        ctk::vec3<f32> Position;
        ctk::vec4<f32> Color;
    };
    vertex Vertexes[] =
    {
        { {  0.75f,  0.75f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        { {  0.0f,  -0.75f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { -0.75f,  0.75f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
    };
    vtk::buffer VertexBuffer = vtk::CreateBuffer(&Device, sizeof(Vertexes), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    vtk::WriteToHostCoherentBuffer(Device.Logical, &VertexBuffer, Vertexes, sizeof(Vertexes), 0);

    ////////////////////////////////////////////////////////////
    /// Render Pass
    ////////////////////////////////////////////////////////////
    vtk::render_pass_config RenderPassConfig = {};

    // Attachments
    u32 ColorAttachmentIndex = RenderPassConfig.Attachments.Count;
    vtk::attachment *ColorAttachment = ctk::Push(&RenderPassConfig.Attachments);
    ColorAttachment->Format     = Swapchain.ImageFormat;
    ColorAttachment->LoadOp     = VK_ATTACHMENT_LOAD_OP_CLEAR;
    ColorAttachment->StoreOp    = VK_ATTACHMENT_STORE_OP_STORE;
    ColorAttachment->ClearValue = { 0.04f, 0.04f, 0.04f, 1.0f };

    // Subpasses
    vtk::subpass *Subpass = ctk::Push(&RenderPassConfig.Subpasses);

    VkAttachmentReference *ColorAttachmentReference = ctk::Push(&Subpass->ColorAttachmentReferences);
    ColorAttachmentReference->attachment = ColorAttachmentIndex;
    ColorAttachmentReference->layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // Creation
    vtk::render_pass RenderPass = vtk::CreateRenderPass(Device.Logical, &RenderPassConfig);

    // Framebuffers
    ctk::static_array<VkFramebuffer, 4> Framebuffers = {};
    for(u32 FramebufferIndex = 0; FramebufferIndex < Swapchain.Images.Count; ++FramebufferIndex)
    {
        vtk::framebuffer_config FramebufferConfig = {};
        ctk::Push(&FramebufferConfig.Attachments, Swapchain.Images[FramebufferIndex].View);
        FramebufferConfig.Extent = Swapchain.Extent;
        FramebufferConfig.Layers = 1;
        ctk::Push(&Framebuffers, vtk::CreateFramebuffer(Device.Logical, RenderPass.RenderPass, &FramebufferConfig));
    }

    // Command Buffers
    ctk::static_array<VkCommandBuffer, 4> CommandBuffers = {};
    vtk::AllocateCommandBuffers(Device.Logical, GraphicsCommandPool, Swapchain.Images.Count, CommandBuffers.Data);

    ////////////////////////////////////////////////////////////
    /// Shader Modules
    ////////////////////////////////////////////////////////////
    vtk::shader_module VertexShader =
        vtk::CreateShaderModule(Device.Logical, "assets/shaders/shader.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    vtk::shader_module FragmentShader =
        vtk::CreateShaderModule(Device.Logical, "assets/shaders/shader.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

    ////////////////////////////////////////////////////////////
    /// Vertex Layout
    ////////////////////////////////////////////////////////////
    vtk::vertex_layout VertexLayout = {};
    u32 VertexPositionIndex = vtk::PushVertexAttribute(&VertexLayout, 3);
    u32 VertexColorIndex = vtk::PushVertexAttribute(&VertexLayout, 4);

    ////////////////////////////////////////////////////////////
    /// Graphics Pipelines
    ////////////////////////////////////////////////////////////
    vtk::graphics_pipeline_config GraphicsPipelineConfig = {};
    ctk::Push(&GraphicsPipelineConfig.ShaderModules, &VertexShader);
    ctk::Push(&GraphicsPipelineConfig.ShaderModules, &FragmentShader);
    ctk::Push(&GraphicsPipelineConfig.VertexInputs, { 0, 0, VertexPositionIndex });
    ctk::Push(&GraphicsPipelineConfig.VertexInputs, { 1, 0, VertexColorIndex });
    GraphicsPipelineConfig.VertexLayout      = &VertexLayout;
    GraphicsPipelineConfig.ViewportExtent    = Swapchain.Extent;
    GraphicsPipelineConfig.PrimitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    GraphicsPipelineConfig.DepthTesting      = VK_TRUE;

    vtk::graphics_pipeline GraphicsPipeline = vtk::CreateGraphicsPipeline(Device.Logical, RenderPass.RenderPass, &GraphicsPipelineConfig);

    ////////////////////////////////////////////////////////////
    /// Record render pass.
    ////////////////////////////////////////////////////////////
    VkRect2D RenderArea = {};
    RenderArea.offset.x = 0;
    RenderArea.offset.y = 0;
    RenderArea.extent   = Swapchain.Extent;

    VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
    CommandBufferBeginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    CommandBufferBeginInfo.flags            = 0;
    CommandBufferBeginInfo.pInheritanceInfo = NULL;

    VkBuffer VertexBuffers[] = { VertexBuffer.Buffer };
    VkDeviceSize VertexBufferOffsets[] = { 0 };
    for(u32 FrameIndex = 0; FrameIndex < Swapchain.Images.Count; ++FrameIndex)
    {
        VkCommandBuffer CommandBuffer = CommandBuffers[FrameIndex];
        {
            VkResult Result = vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo);
            vtk::ValidateVkResult(Result, "vkBeginCommandBuffer", "failed to begin recording command buffer");
        }
        VkRenderPassBeginInfo RenderPassBeginInfo = {};
        RenderPassBeginInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        RenderPassBeginInfo.renderPass      = RenderPass.RenderPass;
        RenderPassBeginInfo.framebuffer     = Framebuffers[FrameIndex];
        RenderPassBeginInfo.renderArea      = RenderArea;
        RenderPassBeginInfo.clearValueCount = RenderPass.ClearValues.Count;
        RenderPassBeginInfo.pClearValues    = RenderPass.ClearValues.Data;

        // Begin
        vkCmdBeginRenderPass(CommandBuffer, &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Render Commands
        vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipeline.Pipeline);
        vkCmdBindVertexBuffers(CommandBuffer,
                               0, // First Binding
                               1, // Binding Count
                               VertexBuffers,
                               VertexBufferOffsets);
        vkCmdDraw(CommandBuffer,
                  CTK_ARRAY_COUNT(Vertexes),
                  1, // Instance Count (instanced rendering only)
                  0, // First Vertex
                  0); // First instance index (instanced rendering only)

        // End
        vkCmdEndRenderPass(CommandBuffer);
        {
            VkResult Result = vkEndCommandBuffer(CommandBuffer);
            vtk::ValidateVkResult(Result, "vkEndCommandBuffer", "error during render pass command recording");
        }
    }

    ////////////////////////////////////////////////////////////
    /// Frame State
    ////////////////////////////////////////////////////////////
    vtk::frame_state FrameState = vtk::CreateFrameState(Device.Logical, 2, Swapchain.Images.Count);

    ////////////////////////////////////////////////////////////
    /// Main Loop
    ////////////////////////////////////////////////////////////
    b32 Close = false;
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

        ////////////////////////////////////////////////////////////
        /// Rendering
        ////////////////////////////////////////////////////////////
        vtk::frame *CurrentFrame = FrameState.Frames + FrameState.CurrentFrameIndex;

        // Wait on current frame's fence if still unsignaled.
        vkWaitForFences(Device.Logical, 1, &CurrentFrame->InFlightFence, VK_TRUE, UINT64_MAX);

        // Aquire next swapchain image index, using a semaphore to signal when image is available for rendering.
        u32 SwapchainImageIndex = VTK_UNSET_INDEX;
        {
            VkResult Result = vkAcquireNextImageKHR(Device.Logical, Swapchain.Swapchain, UINT64_MAX, CurrentFrame->ImageAquiredSemaphore,
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
        VkSemaphore          QueueSubmitWaitSemaphores[]   = { CurrentFrame->ImageAquiredSemaphore           };
        VkPipelineStageFlags QueueSubmitWaitStages[]       = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        VkSemaphore          QueueSubmitSignalSemaphores[] = { CurrentFrame->RenderFinishedSemaphore         };
        VkCommandBuffer      QueueSubmitCommandBuffers[]   = { CommandBuffers[SwapchainImageIndex]           };

        VkSubmitInfo SubmitInfos[1] = {};
        SubmitInfos[0].sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        SubmitInfos[0].waitSemaphoreCount   = CTK_ARRAY_COUNT(QueueSubmitWaitSemaphores);
        SubmitInfos[0].pWaitSemaphores      = QueueSubmitWaitSemaphores;
        SubmitInfos[0].pWaitDstStageMask    = QueueSubmitWaitStages;
        SubmitInfos[0].commandBufferCount   = CTK_ARRAY_COUNT(QueueSubmitCommandBuffers);
        SubmitInfos[0].pCommandBuffers      = QueueSubmitCommandBuffers;
        SubmitInfos[0].signalSemaphoreCount = CTK_ARRAY_COUNT(QueueSubmitSignalSemaphores);
        SubmitInfos[0].pSignalSemaphores    = QueueSubmitSignalSemaphores;
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
        VkSwapchainKHR Swapchains[] = { Swapchain.Swapchain };
        u32 SwapchainImageIndexes[] = { SwapchainImageIndex };

        VkPresentInfoKHR PresentInfo = {};
        PresentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        PresentInfo.waitSemaphoreCount = CTK_ARRAY_COUNT(QueueSubmitSignalSemaphores);
        PresentInfo.pWaitSemaphores    = QueueSubmitSignalSemaphores;
        PresentInfo.swapchainCount     = CTK_ARRAY_COUNT(Swapchains);
        PresentInfo.pSwapchains        = Swapchains;
        PresentInfo.pImageIndices      = SwapchainImageIndexes;
        PresentInfo.pResults           = NULL;
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
