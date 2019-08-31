#include <chrono>
#include <cstdlib>
#include <memory>
#include <random>
#include <string>
#include <thread>
#include <vector>
#include <Windows.h>
#include "Graphics/Camera.h"
#include "Graphics/Cube.h"
#include "Graphics/Object3D.h"
#include "Graphics/RayTracing/Material.h"
#include "Graphics/RayTracing/PointLight.h"
#include "Graphics/RayTracing/RayTracingAlgorithm.h"
#include "Graphics/RayTracing/Sphere.h"
#include "Graphics/RayTracing/Triangle.h"
#include "Graphics/Renderer.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/Triangle.h"
#include "Windowing/Win32Window.h"

#define USE_RAY_TRACING 0

// GLOBALS.
// Global to provide access to them within the window procedure.
/// The window for the application.
static std::unique_ptr<WINDOWING::Win32Window> g_window = nullptr;
/// The renderer.
static std::unique_ptr<GRAPHICS::Renderer> g_renderer = nullptr;

/// \todo   temporary?
//static GRAPHICS::RAY_TRACING::Scene* g_scene = nullptr;
static std::unique_ptr<GRAPHICS::RAY_TRACING::Scene> g_scene = nullptr;
static GRAPHICS::RAY_TRACING::RayTracingAlgorithm* g_ray_tracer = nullptr;
static GRAPHICS::RenderTarget* g_render_target = nullptr;

std::unique_ptr<GRAPHICS::RAY_TRACING::Scene> CreateScene(const unsigned int scene_number)
{
    switch (scene_number)
    {
    case 0:
    {
        // BASIC TRIANGLE.
        auto scene = std::make_unique<GRAPHICS::RAY_TRACING::Scene>();
        scene->PointLights.push_back(GRAPHICS::RAY_TRACING::PointLight(
            MATH::Vector3f(0.0f, 0.0f, 0.0f),
            GRAPHICS::Color(1.0f, 1.0f, 1.0f, 1.0f)));
        scene->BackgroundColor = GRAPHICS::Color(0.3f, 0.3f, 0.7f, 0.0f);

        GRAPHICS::RAY_TRACING::Material material;
        material.DiffuseColor = GRAPHICS::Color(0.8f, 0.8f, 0.8f, 1.0f);
        material.AmbientColor = GRAPHICS::Color(0.0f, 0.0f, 0.0f, 1.0f);
        material.SpecularColor = GRAPHICS::Color(0.0f, 0.0f, 0.0f, 1.0f);
        material.SpecularPower = 1.0f;
        material.ReflectivityProportion = 0.0f;

        auto triangle = std::make_unique<GRAPHICS::RAY_TRACING::Triangle>();
        triangle->Vertices =
        {
            MATH::Vector3f(-1.0f, -1.0f, -2.0f),
            MATH::Vector3f(1.0f, -1.0f, -2.0f),
            MATH::Vector3f(0.0f, 1.0f, -2.0f),
        };
        triangle->Material = material;
        scene->Objects.push_back(std::move(triangle));
        return scene;
    }
    case 1:
    {
        // MULTIPLE TRIANGLES.
        auto scene = std::make_unique<GRAPHICS::RAY_TRACING::Scene>();
        scene->PointLights.push_back(GRAPHICS::RAY_TRACING::PointLight(
            MATH::Vector3f(4.0f, 4.0f, 8.0f),
            GRAPHICS::Color(0.7f, 0.7f, 0.7f, 1.0f)));
        scene->BackgroundColor = GRAPHICS::Color(0.2f, 0.2f, 1.0f, 0.0f);

        GRAPHICS::RAY_TRACING::Material material;
        material.DiffuseColor = GRAPHICS::Color(0.8f, 0.8f, 0.8f, 1.0f);
        material.AmbientColor = GRAPHICS::Color(0.2f, 0.2f, 0.2f, 1.0f);
        material.SpecularColor = GRAPHICS::Color(0.0f, 0.0f, 0.0f, 1.0f);
        material.SpecularPower = 1.0f;
        material.ReflectivityProportion = 0.0f;

        auto triangle = std::make_unique<GRAPHICS::RAY_TRACING::Triangle>();
        triangle->Vertices =
        {
            MATH::Vector3f(-0.1f, -0.1f, -2.0f),
            MATH::Vector3f(0.1f, -0.1f, -2.0f),
            MATH::Vector3f(0.0f, 0.1f, -2.0f),
        };
        triangle->Material = material;
        scene->Objects.push_back(std::move(triangle));

        triangle = std::make_unique<GRAPHICS::RAY_TRACING::Triangle>();
        triangle->Vertices =
        {
            MATH::Vector3f(-0.9f, -0.9f, -2.0f),
            MATH::Vector3f(0.7f, -0.7f, -2.0f),
            MATH::Vector3f(-0.7f, -0.6f, -2.0f),
        };
        triangle->Material = material;
        scene->Objects.push_back(std::move(triangle));

        triangle = std::make_unique<GRAPHICS::RAY_TRACING::Triangle>();
        triangle->Vertices =
        {
            MATH::Vector3f(0.5f, -0.5f, -2.0f),
            MATH::Vector3f(0.8f, -0.7f, -2.0f),
            MATH::Vector3f(0.6f, 0.6f, -2.0f),
        };
        triangle->Material = material;
        scene->Objects.push_back(std::move(triangle));
        
        return scene;
    }
    case 2:
    {
        auto scene = std::make_unique<GRAPHICS::RAY_TRACING::Scene>();
        scene->PointLights.push_back(GRAPHICS::RAY_TRACING::PointLight(
            MATH::Vector3f(4.0f, 4.0f, 8.0f),
            GRAPHICS::Color(0.7f, 0.7f, 0.7f, 1.0f)));
        scene->BackgroundColor = GRAPHICS::Color(0.2f, 0.2f, 1.0f, 0.0f);

        // TRIANLGE + SPHERE.
        GRAPHICS::RAY_TRACING::Material material;
        material.DiffuseColor = GRAPHICS::Color(0.8f, 0.8f, 0.8f, 1.0f);
        material.AmbientColor = GRAPHICS::Color(0.2f, 0.2f, 0.2f, 1.0f);
        material.SpecularColor = GRAPHICS::Color(0.0f, 0.0f, 0.0f, 1.0f);
        material.SpecularPower = 1.0f;
        material.ReflectivityProportion = 0.0f;

        auto triangle = std::make_unique<GRAPHICS::RAY_TRACING::Triangle>();
        triangle->Vertices =
        {
            MATH::Vector3f(-1.0f, -1.0f, -3.0f),
            MATH::Vector3f(1.0f, -1.0f, -3.0f),
            MATH::Vector3f(0.0f, 1.0f, -3.0f),
        };
        triangle->Material = material;
        scene->Objects.push_back(std::move(triangle));

        // SPHERE.
        GRAPHICS::RAY_TRACING::Material sphere_material;
        sphere_material.DiffuseColor = GRAPHICS::Color(0.8f, 0.2f, 0.7f, 1.0f);
        sphere_material.AmbientColor = GRAPHICS::Color(0.3f, 0.1f, 0.6f, 1.0f);
        sphere_material.SpecularColor = GRAPHICS::Color(0.0f, 0.0f, 0.0f, 1.0f);
        sphere_material.SpecularPower = 1.0f;
        sphere_material.ReflectivityProportion = 0.0f;

        auto sphere = std::make_unique<GRAPHICS::RAY_TRACING::Sphere>();
        sphere->CenterPosition = MATH::Vector3f(0.0f, 0.0f, -3.0f);
        sphere->Radius = 0.87f;
        sphere->Material = sphere_material;

        scene->Objects.push_back(std::move(sphere));

        return scene;
    }
    case 3:
    {
        // MULTIPLE LIGHTS.
        auto scene = std::make_unique<GRAPHICS::RAY_TRACING::Scene>();
        scene->PointLights.push_back(GRAPHICS::RAY_TRACING::PointLight(
            MATH::Vector3f(3.0f, 4.0f, 0.0f),
            GRAPHICS::Color(0.9f, 0.0f, 0.0f, 1.0f)));
        scene->PointLights.push_back(GRAPHICS::RAY_TRACING::PointLight(
            MATH::Vector3f(-4.0f, 5.0f, 0.0f),
            GRAPHICS::Color(0.0f, 0.9f, 0.0f, 1.0f)));
        scene->PointLights.push_back(GRAPHICS::RAY_TRACING::PointLight(
            MATH::Vector3f(0.0f, 4.0f, -5.0f),
            GRAPHICS::Color(0.0f, 0.0f, 0.9f, 1.0f)));
        scene->BackgroundColor = GRAPHICS::Color(0.2f, 0.2f, 1.0f, 0.0f);

        // Ground plane.
        GRAPHICS::RAY_TRACING::Material material;
        material.DiffuseColor = GRAPHICS::Color(0.8f, 0.8f, 0.8f, 1.0f);
        material.AmbientColor = GRAPHICS::Color(0.2f, 0.2f, 0.2f, 1.0f);
        material.SpecularColor = GRAPHICS::Color(0.1f, 0.1f, 0.1f, 1.0f);
        material.SpecularPower = 1.0f;
        material.ReflectivityProportion = 0.5f;

        auto triangle = std::make_unique<GRAPHICS::RAY_TRACING::Triangle>();
        triangle->Vertices =
        {
            MATH::Vector3f(-100.0f, -1.0f, -100.0f),
            MATH::Vector3f(100.0f, -1.0f, 100.0f),
            MATH::Vector3f(100.0f, -1.0f, -100.0f)
        };
        triangle->Material = material;
        scene->Objects.push_back(std::move(triangle));

        triangle = std::make_unique<GRAPHICS::RAY_TRACING::Triangle>();
        triangle->Vertices =
        {
            MATH::Vector3f(100.0f, -1.0f, 100.0f),
            MATH::Vector3f(-100.0f, -1.0f, -100.0f),
            MATH::Vector3f(-100.0f, -1.0f, 100.0f)
        };
        triangle->Material = material;
        scene->Objects.push_back(std::move(triangle));

        // SPHERE.
        GRAPHICS::RAY_TRACING::Material sphere_material;
        sphere_material.DiffuseColor = GRAPHICS::Color(0.8f, 0.8f, 0.8f, 1.0f);
        sphere_material.AmbientColor = GRAPHICS::Color(0.2f, 0.2f, 0.2f, 1.0f);
        sphere_material.SpecularColor = GRAPHICS::Color(0.1f, 0.1f, 0.1f, 1.0f);
        sphere_material.SpecularPower = 1.0f;
        sphere_material.ReflectivityProportion = 0.5f;

        auto sphere = std::make_unique<GRAPHICS::RAY_TRACING::Sphere>();
        sphere->CenterPosition = MATH::Vector3f(0.0f, 0.5f, -3.0f);
        sphere->Radius = 0.7f;
        sphere->Material = sphere_material;

        scene->Objects.push_back(std::move(sphere));

        return scene;
    }
    case 4:
    {
        // MULTIPLE REFLECTIVE SPHERES.
        auto scene = std::make_unique<GRAPHICS::RAY_TRACING::Scene>();
        scene->PointLights.push_back(GRAPHICS::RAY_TRACING::PointLight(
            MATH::Vector3f(8.0f, 8.0f, 3.0f),
            GRAPHICS::Color(0.7f, 0.7f, 0.7f, 1.0f)));
        scene->PointLights.push_back(GRAPHICS::RAY_TRACING::PointLight(
            MATH::Vector3f(-4.0f, 2.0f, 0.0f),
            GRAPHICS::Color(0.3f, 0.3f, 0.3f, 1.0f)));
        scene->BackgroundColor = GRAPHICS::Color(0.2f, 0.2f, 1.0f, 0.0f);

        // Ground plane.
        GRAPHICS::RAY_TRACING::Material material;
        material.DiffuseColor = GRAPHICS::Color(0.8f, 0.8f, 0.8f, 1.0f);
        material.AmbientColor = GRAPHICS::Color(0.2f, 0.2f, 0.2f, 1.0f);
        material.SpecularColor = GRAPHICS::Color(0.0f, 0.0f, 0.0f, 1.0f);
        material.SpecularPower = 1.0f;
        material.ReflectivityProportion = 0.5f;

        auto triangle = std::make_unique<GRAPHICS::RAY_TRACING::Triangle>();
        triangle->Vertices =
        {
            MATH::Vector3f(-100.0f, -1.0f, -100.0f),
            MATH::Vector3f(100.0f, -1.0f, 100.0f),
            MATH::Vector3f(100.0f, -1.0f, -100.0f)
        };
        triangle->Material = material;
        scene->Objects.push_back(std::move(triangle));

        triangle = std::make_unique<GRAPHICS::RAY_TRACING::Triangle>();
        triangle->Vertices =
        {
            MATH::Vector3f(100.0f, -1.0f, 100.0f),
            MATH::Vector3f(-100.0f, -1.0f, -100.0f),
            MATH::Vector3f(-100.0f, -1.0f, 100.0f)
        };
        triangle->Material = material;
        scene->Objects.push_back(std::move(triangle));

        // SPHERE.
        GRAPHICS::RAY_TRACING::Material sphere_material;
        sphere_material.DiffuseColor = GRAPHICS::Color(0.6f, 0.6f, 0.6f, 1.0f);
        sphere_material.AmbientColor = GRAPHICS::Color(0.2f, 0.2f, 0.2f, 1.0f);
        sphere_material.SpecularColor = GRAPHICS::Color(0.7f, 0.7f, 0.7f, 1.0f);
        sphere_material.SpecularPower = 20.0f;
        sphere_material.ReflectivityProportion = 0.7f;

        auto sphere = std::make_unique<GRAPHICS::RAY_TRACING::Sphere>();
        sphere->CenterPosition = MATH::Vector3f(1.2f, 0.0f, -5.0f);
        sphere->Radius = 1.0f;
        sphere_material.AmbientColor = GRAPHICS::Color(1.0f, 0.0f, 0.0f, 1.0f);
        sphere_material.DiffuseColor = sphere_material.AmbientColor;
        sphere->Material = sphere_material;
        scene->Objects.push_back(std::move(sphere));

        sphere = std::make_unique<GRAPHICS::RAY_TRACING::Sphere>();
        sphere->CenterPosition = MATH::Vector3f(0.0f, 0.3f, -1.5f);
        sphere->Radius = 0.2f;
        sphere_material.AmbientColor = GRAPHICS::Color(0.0f, 1.0f, 0.0f, 1.0f);
        sphere_material.DiffuseColor = sphere_material.AmbientColor;
        sphere->Material = sphere_material;
        scene->Objects.push_back(std::move(sphere));

        sphere = std::make_unique<GRAPHICS::RAY_TRACING::Sphere>();
        sphere->CenterPosition = MATH::Vector3f(-1.0f, -0.5f, -3.0f);
        sphere->Radius = 0.5f;
        sphere_material.AmbientColor = GRAPHICS::Color(0.0f, 0.0f, 1.0f, 1.0f);
        sphere_material.DiffuseColor = sphere_material.AmbientColor;
        sphere->Material = sphere_material;
        scene->Objects.push_back(std::move(sphere));

        return scene;
    }
    case 5:
    {
        auto scene = std::make_unique<GRAPHICS::RAY_TRACING::Scene>();
        scene->PointLights.push_back(GRAPHICS::RAY_TRACING::PointLight(
            MATH::Vector3f(-5.0f, 2.0f, 5.0f),
            GRAPHICS::Color(1.0f, 1.0f, 1.0f, 1.0f)));
        scene->BackgroundColor = GRAPHICS::Color::BLACK;

        // SPHERE 1.
        GRAPHICS::RAY_TRACING::Material sphere_material;
        sphere_material.DiffuseColor = GRAPHICS::Color(0.5f, 0.0f, 0.5f, 1.0f);
        sphere_material.AmbientColor = GRAPHICS::Color(0.2f, 0.0f, 0.2f, 1.0f);
        sphere_material.SpecularColor = GRAPHICS::Color(0.7f, 0.7f, 0.7f, 1.0f);
        sphere_material.SpecularPower = 20.0f;
        sphere_material.ReflectivityProportion = 0.8f;

        auto sphere = std::make_unique<GRAPHICS::RAY_TRACING::Sphere>();
        sphere->CenterPosition = MATH::Vector3f(0.0f, 0.0f, -4.0f);
        sphere->Radius = 1.0f;
        sphere->Material = sphere_material;
        scene->Objects.push_back(std::move(sphere));

        // SPHERE 2.
        sphere_material.DiffuseColor = GRAPHICS::Color(0.0f, 0.5f, 0.0f, 1.0f);
        sphere_material.AmbientColor = GRAPHICS::Color(0.0f, 0.2f, 0.0f, 1.0f);
        sphere_material.SpecularColor = GRAPHICS::Color(0.0f, 0.2f, 0.0f, 1.0f);
        sphere_material.SpecularPower = 1.0f;
        sphere_material.ReflectivityProportion = 0.0f;

        sphere = std::make_unique<GRAPHICS::RAY_TRACING::Sphere>();
        sphere->CenterPosition = MATH::Vector3f(1.0f, 0.6f, -3.0f);
        sphere->Radius = 0.3f;
        sphere->Material = sphere_material;
        scene->Objects.push_back(std::move(sphere));

        return scene;
    }
    default:
        return nullptr;
    }
}

/// The main window callback procedure for processing messages sent to the main application window.
/// @param[in]  window - Handle to the window.
/// @param[in]  message - The message.
/// @param[in]  w_param - Additional information depending on the type of message.
/// @param[in]  l_param - Additional information depending on the type of message.
/// @return     The result of processing the message.
LRESULT CALLBACK MainWindowCallback(
    HWND window,
    UINT message,
    WPARAM w_param,
    LPARAM l_param)
{
    LRESULT messageProcessingResult = 0;

    switch (message)
    {
    case WM_CREATE:
        break;
    case WM_SIZE:
        break;
    case WM_DESTROY:
        break;
    case WM_CLOSE:
    {
        PostQuitMessage(EXIT_SUCCESS);
        break;
    }
    case WM_ACTIVATEAPP:
        break;
    case WM_SYSKEYDOWN:
        break;
    case WM_SYSKEYUP:
        break;
    case WM_KEYUP:
        break;
    case WM_KEYDOWN:
    {
        const float move_speed = 1.0f;
        int virtual_key_code = static_cast<int>(w_param);
        switch (virtual_key_code)
        {
#if USE_RAY_TRACING
        case VK_UP:
            g_ray_tracer->Camera.WorldPosition.Y += 0.1f;
            break;
        case VK_DOWN:
            g_ray_tracer->Camera.WorldPosition.Y -= 0.1f;
            break;
        case VK_LEFT:
            g_ray_tracer->Camera.WorldPosition.X -= 0.1f;
            break;
        case VK_RIGHT:
            g_ray_tracer->Camera.WorldPosition.X += 0.1f;
            break;
        case 0x5A: // Z
            g_ray_tracer->Camera.WorldPosition.Z += 0.1f;
            break;
        case 0x58: // X
            g_ray_tracer->Camera.WorldPosition.Z -= 0.1f;
            break;
        case 0x31: // 1
            g_ray_tracer->Camera.ViewingPlane.FocalLength += 0.1f;
            break;
        case 0x32: // 2
            g_ray_tracer->Camera.ViewingPlane.FocalLength -= 0.1f;
            break;
        case 0x30: // 0
            if (GRAPHICS::ProjectionType::ORTHOGRAPHIC == g_ray_tracer->Camera.Projection)
            {
                g_ray_tracer->Camera.Projection = GRAPHICS::ProjectionType::PERSPECTIVE;
            }
            else
            {
                g_ray_tracer->Camera.Projection = GRAPHICS::ProjectionType::ORTHOGRAPHIC;
            }
            break;
        case 0x51: // Q
            g_scene = CreateScene(0);
            break;
        case 0x57: // W
            g_scene = CreateScene(1);
            break;
        case 0x45: // E
            g_scene = CreateScene(2);
            break;
        case 0x52: // R
            g_scene = CreateScene(3);
            break;
        case 0x54: // T
            g_scene = CreateScene(4);
            break;
        case 0x59: // Y
            g_scene = CreateScene(5);
            break;
        case 0x41: // A
            g_ray_tracer->Ambient = !g_ray_tracer->Ambient;
            break;
        case 0x53: // S
            g_ray_tracer->Shadows = !g_ray_tracer->Shadows;
            break;
        case 0x44: // D
            g_ray_tracer->Diffuse = !g_ray_tracer->Diffuse;
            break;
        case 0x46: // F
            g_ray_tracer->Specular = !g_ray_tracer->Specular;
            break;
        case 0x47: // G
            g_ray_tracer->Reflections = !g_ray_tracer->Reflections;
            break;
        case 0x48: // H
            --g_ray_tracer->ReflectionCount;
            break;
        case 0x4A: // J
            ++g_ray_tracer->ReflectionCount;
            break;
        }

        OutputDebugString(std::to_string(g_ray_tracer->Camera.ViewingPlane.FocalLength).c_str());
        OutputDebugString("\n");
        g_ray_tracer->Render(*g_scene, *g_render_target);
#else
        case VK_UP:
            g_renderer->Camera.WorldPosition.Y += move_speed;
            break;
        case VK_DOWN:
            g_renderer->Camera.WorldPosition.Y -= move_speed;
            break;
        case VK_LEFT:
            g_renderer->Camera.WorldPosition.X -= move_speed;
            break;
        case VK_RIGHT:
            g_renderer->Camera.WorldPosition.X += move_speed;
            break;
        case 0x57: // W
            g_renderer->Camera.WorldPosition.Z -= move_speed;
            break;
        case 0x53: // S
            g_renderer->Camera.WorldPosition.Z += move_speed;
            break;
    }

        std::string camera_position;
        camera_position += std::to_string(g_renderer->Camera.WorldPosition.X) + ",";
        camera_position += std::to_string(g_renderer->Camera.WorldPosition.Y) + ",";
        camera_position += std::to_string(g_renderer->Camera.WorldPosition.Z) + "\n";
        OutputDebugString(camera_position.c_str());
#endif
        break;
    }
    /// @todo case WM_SETCURSOR:
    /// @todo break;
    case WM_PAINT:
    {
        PAINTSTRUCT paint;
        /// @todo HDC device_context = BeginPaint(window, &paint);
        BeginPaint(window, &paint);

        /// @todo   Copy rendered stuff to window here!

        EndPaint(window, &paint);
        break;
    }
    default:
        messageProcessingResult = DefWindowProcA(window, message, w_param, l_param);
        break;
}

    return messageProcessingResult;
}

/// The entry point to the application.
/// @param[in]  application_instance - A handle to the current instance of the application.
/// @param[in]  previous_application_instance - Always NULL.
/// @param[in]  command_line_string - The command line parameters for the application.
/// @param[in]  window_show_code - Controls how the window is to be shown.
/// @return     An exit code.  0 for success.
int CALLBACK WinMain(
    HINSTANCE application_instance,
    HINSTANCE previous_application_instance,
    LPSTR command_line_string,
    int window_show_code)
{
    // REFERENCE UNUSED PARAMETERS TO PREVENT COMPILER WARNINGS.
    previous_application_instance;
    command_line_string;
    window_show_code;

    // DEFINE PARAMETERS FOR THE WINDOW TO BE CREATED.
    // The structure is zeroed-out initially since it isn't necessary to set all fields.
    WNDCLASSEX window_class = {};

    window_class.cbSize = sizeof(window_class);

    // Ensure that the window gets re-drawn when the client area is moved or re-sized.
    // A unique device context is allocated for each window in the class which can
    // potentially provide performance improvements by Windows not having to re-obtain
    // or re-allocate a device context.
    window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

    window_class.lpfnWndProc = MainWindowCallback;
    window_class.hInstance = application_instance;
    window_class.lpszClassName = "WindowClass";

    // CREATE THE WINDOW.
    constexpr unsigned int SCREEN_WIDTH_IN_PIXELS = 400;
    constexpr unsigned int SCREEN_HEIGHT_IN_PIXELS = 400;
    g_window = WINDOWING::Win32Window::Create(
        window_class,
        "Window Title",
        static_cast<int>(SCREEN_WIDTH_IN_PIXELS),
        static_cast<int>(SCREEN_HEIGHT_IN_PIXELS));
    bool window_created = (nullptr != g_window);
    if (!window_created)
    {
        OutputDebugString("Failed to create window.");
        return EXIT_FAILURE;
    }

    // GET THE DEVICE CONTEXT OF THE WINDOW.
    HDC device_context = GetDC(g_window->WindowHandle);
    bool device_context_retrieved = (NULL != device_context);
    if (!device_context_retrieved)
    {
        OutputDebugString("Failed to get window device context.");
        return EXIT_FAILURE;
    }

    // CREATE THE MAIN RENDER TARGET.
    GRAPHICS::RenderTarget render_target(SCREEN_WIDTH_IN_PIXELS, SCREEN_HEIGHT_IN_PIXELS, GRAPHICS::ColorFormat::ARGB);

    // CREATE A SCENE.
    g_scene = CreateScene(0);

    // PERFORM RAY TRACING.
    GRAPHICS::RAY_TRACING::RayTracingAlgorithm ray_tracer;

    ray_tracer.Camera = GRAPHICS::Camera::LookAtFrom(MATH::Vector3f(0.0f, 0.0f, 0.0f), MATH::Vector3f(0.0f, 0.0f, 1.0f));

    ray_tracer.Render(*g_scene, render_target);

    g_render_target = &render_target;
    g_ray_tracer = &ray_tracer;

#if USE_RAY_TRACING
    bool running = true;
    while (running)
    {
        // PROCESS ANY MESSAGES FOR THE APPLICATION WINDOW.
        MSG message;
        auto message_received = [&]()
        {
            const HWND ALL_MESSAGES_FOR_CURRENT_THREAD = NULL;
            const UINT NO_MIN_MESSAGE_RESTRICTION = 0;
            const UINT NO_MAX_MESSAGE_RESTRICTION = 0;

            return PeekMessageA(
                &message,
                ALL_MESSAGES_FOR_CURRENT_THREAD,
                NO_MIN_MESSAGE_RESTRICTION,
                NO_MAX_MESSAGE_RESTRICTION,
                PM_REMOVE);
        };
        while (message_received())
        {
            // STOP RUNNING THE APPLICATION IF THE USER DECIDED TO QUIT.
            if (message.message == WM_QUIT)
            {
                running = false;
                break;
            }

            // TRANSLATE VIRTUAL KEY MESSAGES TO CHARACTER MESSAGES.
            // The return value is not checked since failure is not problematic
            // and the only thing that could be done is logging.
            TranslateMessage(&message);

            // DISPATCH THE MESSAGE TO THE WINDOW PROCEDURE.
            // The return value is simply the return value from the window procedure.
            // Nothing value could be done with it besides logging, so it is ignored.
            DispatchMessage(&message);
        }

        g_window->Display(render_target);
    }
#else
    // CREATE THE RENDERER.
    g_renderer = std::make_unique<GRAPHICS::Renderer>();

    g_renderer->Camera = GRAPHICS::Camera::LookAtFrom(MATH::Vector3f(0.0f, 0.0f, 0.0f), MATH::Vector3f(0.0f, 0.0f, 100.0f));

    // CREATE THE TRIANGLES TO RENDER.
    std::random_device random_number_generator;
    std::vector<GRAPHICS::Object3D> objects;
    GRAPHICS::Triangle triangle = GRAPHICS::Triangle::CreateEquilateral(GRAPHICS::Color::GREEN);
    // Can go up to about 200 triangles with some 30 FPS and some dips.
    // 150 triangles is about the cap in which we mostly get 30 FPS,
    // although there are still some occasional dips.
    // At around 300 triangles, we can usually keep above 20 FPS, although there are some dips.
    // This is all when running a debug build through Visual Studio, however.
    constexpr std::size_t OBJECT_COUNT = 300;
    constexpr float MIN_X_POSITION = -50.0f;
    constexpr float MAX_X_POSITION = 50.0f;
    constexpr float MIN_Y_POSITION = -50.0f;
    constexpr float MAX_Y_POSITION = 50.0f;
    while (objects.size() < OBJECT_COUNT)
    {
        GRAPHICS::Object3D current_object_3D;
        current_object_3D.Triangles = { triangle };
        constexpr float OBJECT_SCALE = 30.0f;
        current_object_3D.Scale = MATH::Vector3f(OBJECT_SCALE, OBJECT_SCALE, OBJECT_SCALE);
        float x_position = static_cast<float>(random_number_generator() % 150) - 75.0f;
        float y_position = static_cast<float>(random_number_generator() % 150) - 75.0f;
        current_object_3D.WorldPosition = MATH::Vector3f(x_position, y_position, -100.0f);
        objects.push_back(current_object_3D);
    }

#define SINGLE_TRIANGLE 1
#if SINGLE_TRIANGLE
    objects.clear();
    GRAPHICS::Object3D test_object_3D;
    test_object_3D.Triangles = { triangle };
    constexpr float OBJECT_SCALE = 50.0f;
    test_object_3D.Scale = MATH::Vector3f(OBJECT_SCALE, OBJECT_SCALE, OBJECT_SCALE);
    test_object_3D.WorldPosition = MATH::Vector3f(0.0f, 0.0f, -2.0f);
    objects.push_back(test_object_3D);
#endif

#define CUBE 0
#if CUBE
    objects.clear();

    GRAPHICS::Object3D cube = GRAPHICS::Cube::Create(GRAPHICS::Color::GREEN);
    cube.Scale = MATH::Vector3f(10.0f, 10.0f, 10.0f);
    cube.WorldPosition = MATH::Vector3f(0.0f, 0.0f, -2.0f);
    objects.push_back(cube);
#endif

    // RUN A MESSAGE LOOP.
    constexpr float TARGET_FRAMES_PER_SECOND = 60.0f;
    constexpr std::chrono::duration<float, std::chrono::seconds::period> TARGET_SECONDS_PER_FRAME(1.0f / TARGET_FRAMES_PER_SECOND);

#define ROTATE_TRIANGLE 1
#if ROTATE_TRIANGLE
    float angle_in_radians = 0.0f;
    auto start_time = std::chrono::high_resolution_clock::now();
#endif
    bool running = true;
    while (running)
    {
        auto frame_start_time = std::chrono::high_resolution_clock::now();

        // PROCESS ANY MESSAGES FOR THE APPLICATION WINDOW.
        MSG message;
        auto message_received = [&]()
        {
            const HWND ALL_MESSAGES_FOR_CURRENT_THREAD = NULL;
            const UINT NO_MIN_MESSAGE_RESTRICTION = 0;
            const UINT NO_MAX_MESSAGE_RESTRICTION = 0;

            return PeekMessageA(
                &message,
                ALL_MESSAGES_FOR_CURRENT_THREAD,
                NO_MIN_MESSAGE_RESTRICTION,
                NO_MAX_MESSAGE_RESTRICTION,
                PM_REMOVE);
        };
        while (message_received())
        {
            // STOP RUNNING THE APPLICATION IF THE USER DECIDED TO QUIT.
            if (message.message == WM_QUIT)
            {
                running = false;
                break;
            }

            // TRANSLATE VIRTUAL KEY MESSAGES TO CHARACTER MESSAGES.
            // The return value is not checked since failure is not problematic
            // and the only thing that could be done is logging.
            TranslateMessage(&message);

            // DISPATCH THE MESSAGE TO THE WINDOW PROCEDURE.
            // The return value is simply the return value from the window procedure.
            // Nothing value could be done with it besides logging, so it is ignored.
            DispatchMessage(&message);
        }

#if ROTATE_TRIANGLE
        auto current_time = std::chrono::high_resolution_clock::now();
        auto total_elapsed_time = std::chrono::duration_cast<std::chrono::duration<float>>(current_time - start_time).count();
        angle_in_radians = 0.5f * total_elapsed_time;
        for (auto& object_3D : objects)
        {
            object_3D.RotationInRadians.X = MATH::Angle<float>::Radians(angle_in_radians);
            object_3D.RotationInRadians.Y = MATH::Angle<float>::Radians(angle_in_radians);
            object_3D.RotationInRadians.Z = MATH::Angle<float>::Radians(angle_in_radians);
        }
#endif

        render_target.FillPixels(GRAPHICS::Color::BLACK);

        for (auto object_3D : objects)
        {
            g_renderer->Render(object_3D, render_target);
        }

        g_window->Display(render_target);

#if 1
        auto frame_end_time = std::chrono::high_resolution_clock::now();
        //float total_elapsed_time = std::chrono::duration_cast<std::chrono::duration<float>>(current_time - frame_start_time).count();
        auto frame_elapsed_time = frame_end_time - frame_start_time;

        auto frame_elapsed_time_milliseconds = std::chrono::duration_cast<
            std::chrono::duration<float, std::chrono::milliseconds::period>>(frame_elapsed_time);
        std::string frame_elapsed_time_milliseconds_string = std::to_string(frame_elapsed_time_milliseconds.count()) + " ms\t";
        //OutputDebugStringA(frame_elapsed_time_milliseconds_string.c_str());
        auto frame_elapsed_time_seconds = std::chrono::duration_cast<
            std::chrono::duration<float, std::chrono::seconds::period>>(frame_elapsed_time);
        std::string frame_elapsed_time_seconds_string = std::to_string(frame_elapsed_time_seconds.count()) + " s\t";
        //OutputDebugStringA(frame_elapsed_time_seconds_string.c_str());
        float frames_per_second = 1.0f / frame_elapsed_time_seconds.count();
        std::string frames_per_second_string = std::to_string(frames_per_second) + " fps\n";
        //OutputDebugStringA(frames_per_second_string.c_str());

        /// @todo   Add some buffer time to keep frame rate from dipping too low?
        bool frame_finished_early = frame_elapsed_time_seconds < TARGET_SECONDS_PER_FRAME;
        if (frame_finished_early)
        {
            auto remaining_time_for_frame = TARGET_SECONDS_PER_FRAME - frame_elapsed_time_seconds;
            std::this_thread::sleep_for(remaining_time_for_frame);
        }

        // Re-compute frame time to determine accuracy.
        frame_end_time = std::chrono::high_resolution_clock::now();
        //float total_elapsed_time = std::chrono::duration_cast<std::chrono::duration<float>>(current_time - frame_start_time).count();
        frame_elapsed_time = frame_end_time - frame_start_time;

        frame_elapsed_time_milliseconds = std::chrono::duration_cast<
            std::chrono::duration<float, std::chrono::milliseconds::period>>(frame_elapsed_time);
        frame_elapsed_time_milliseconds_string = std::to_string(frame_elapsed_time_milliseconds.count()) + " ms\t";
        OutputDebugStringA(frame_elapsed_time_milliseconds_string.c_str());
        frame_elapsed_time_seconds = std::chrono::duration_cast<
            std::chrono::duration<float, std::chrono::seconds::period>>(frame_elapsed_time);
        frame_elapsed_time_seconds_string = std::to_string(frame_elapsed_time_seconds.count()) + " s\t";
        OutputDebugStringA(frame_elapsed_time_seconds_string.c_str());
        frames_per_second = 1.0f / frame_elapsed_time_seconds.count();
        frames_per_second_string = std::to_string(frames_per_second) + " fps\n";
        OutputDebugStringA(frames_per_second_string.c_str());
#endif
    }
#endif

    return EXIT_SUCCESS;
}
