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
    b32 KeyDown[GLFW_KEY_LAST + 1];
    b32 MouseButtonDown[GLFW_MOUSE_BUTTON_LAST + 1];
    ctk::vec2<f64> MousePosition;
    ctk::vec2<f64> MouseDelta;
};

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
    /// Vulkan
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
    vtk::swapchain Swapchain = vtk::CreateSwapchain(PlatformSurface, &Device);

    // Graphics Command Pool
    VkCommandPool GraphicsCommandPool = vtk::CreateCommandPool(Device.Logical, Device.GraphicsIndex);

    ////////////////////////////////////////////////////////////
    /// Main Loop
    ////////////////////////////////////////////////////////////
    while(!glfwWindowShouldClose(Window))
    {
        glfwPollEvents();
        Sleep(1);
    }
}
