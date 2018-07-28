#include <chrono>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>
#include <Windows.h>
#include "Graphics/Object3D.h"
#include "Graphics/Renderer.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/Triangle.h"
#include "Windowing/Win32Window.h"

// GLOBALS.
// Global to provide access to them within the window procedure.
/// The window for the application.
static std::unique_ptr<WINDOWING::Win32Window> g_window = nullptr;
/// The renderer.
static std::unique_ptr<GRAPHICS::Renderer> g_renderer = nullptr;

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
            const float move_speed = 0.1f;
            int virtual_key_code = static_cast<int>(w_param);
            switch (virtual_key_code)
            {
                case VK_UP:
                    g_renderer->Camera.WorldPosition.Y += move_speed;
                    g_renderer->Camera.LookAtWorldPosition.Y += move_speed;
                    break;
                case VK_DOWN:
                    g_renderer->Camera.WorldPosition.Y -= move_speed;
                    g_renderer->Camera.LookAtWorldPosition.Y -= move_speed;
                    break;
                case VK_LEFT:
                    g_renderer->Camera.WorldPosition.X -= move_speed;
                    g_renderer->Camera.LookAtWorldPosition.X -= move_speed;
                    break;
                case VK_RIGHT:
                    g_renderer->Camera.WorldPosition.X += move_speed;
                    g_renderer->Camera.LookAtWorldPosition.X += move_speed;
                    break;
                case 0x57: // W
                    g_renderer->Camera.WorldPosition.Z -= move_speed;
                    g_renderer->Camera.LookAtWorldPosition.Z -= move_speed;
                    break;
                case 0x53: // S
                    g_renderer->Camera.WorldPosition.Z += move_speed;
                    g_renderer->Camera.LookAtWorldPosition.Z += move_speed;
                    break;
            }

            std::string camera_position;
            camera_position += std::to_string(g_renderer->Camera.WorldPosition.X) + ",";
            camera_position += std::to_string(g_renderer->Camera.WorldPosition.Y) + ",";
            camera_position += std::to_string(g_renderer->Camera.WorldPosition.Z) + "\n";
            OutputDebugString(camera_position.c_str());
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

    // CREATE THE RENDERER.
    g_renderer = std::make_unique<GRAPHICS::Renderer>();

    g_renderer->Camera.WorldPosition = MATH::Vector3f(0.0f, 0.0f, 1.0f);
    g_renderer->Camera.LookAtWorldPosition = MATH::Vector3f(0.0f, 0.0f, 0.0f);
    g_renderer->Camera.UpDirection = MATH::Vector3f(0.0f, 1.0f, 0.0f);

    // CREATE THE TRIANGLE TO RENDER.
    GRAPHICS::Triangle triangle = GRAPHICS::Triangle::CreateEquilateral(GRAPHICS::Color::GREEN);
    GRAPHICS::Object3D object_3D;
    object_3D.Triangles = { triangle };
    object_3D.WorldPosition = MATH::Vector3f(0.0f, 0.0f, 0.0f);
    //object_3D.Scale = MATH::Vector3f(50.0f, 50.0f, 50.0f);

    // RUN A MESSAGE LOOP.
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
        object_3D.RotationInRadians.X = MATH::Angle<float>::Radians(angle_in_radians);
        object_3D.RotationInRadians.Y = MATH::Angle<float>::Radians(angle_in_radians);
        object_3D.RotationInRadians.Z = MATH::Angle<float>::Radians(angle_in_radians);
#endif

        /// @todo Render to render target.
        render_target.FillPixels(GRAPHICS::Color(1.0, 0.0, 0.0, 1.0f));

        g_renderer->Render(object_3D, render_target);

        /// @todo Copy render target to window.
        g_window->Display(render_target);

#if 1
        auto frame_end_time = std::chrono::high_resolution_clock::now();
        //float total_elapsed_time = std::chrono::duration_cast<std::chrono::duration<float>>(current_time - frame_start_time).count();
        auto frame_elapsed_time = frame_end_time - frame_start_time;
        auto frame_elapsed_time_milliseconds = std::chrono::duration_cast<
            std::chrono::duration<float, std::chrono::milliseconds::period> >(frame_elapsed_time);
        std::string frame_elapsed_time_milliseconds_string = std::to_string(frame_elapsed_time_milliseconds.count()) + " ms\t";
        OutputDebugStringA(frame_elapsed_time_milliseconds_string.c_str());
        auto frame_elapsed_time_seconds = std::chrono::duration_cast<
            std::chrono::duration<float, std::chrono::seconds::period> >(frame_elapsed_time);
        std::string frame_elapsed_time_seconds_string = std::to_string(frame_elapsed_time_seconds.count()) + " s\t";
        OutputDebugStringA(frame_elapsed_time_seconds_string.c_str());
        float frames_per_second = 1.0f / frame_elapsed_time_seconds.count();
        std::string frames_per_second_string = std::to_string(frames_per_second) + " fps\n";
        OutputDebugStringA(frames_per_second_string.c_str());
#endif
    }

    return EXIT_SUCCESS;
}
