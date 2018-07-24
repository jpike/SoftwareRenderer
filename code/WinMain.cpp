#include <chrono>
#include <cstdlib>
#include <memory>
#include <string>
#include <Windows.h>
#include "Graphics/RenderTarget.h"
#include "Windowing/Win32Window.h"

// GLOBALS.
// Global to provide access to them within the window procedure.
/// The window for the application.
static std::unique_ptr<WINDOWING::Win32Window> g_window = nullptr;

void CopyToWindow(const GRAPHICS::RenderTarget& render_target, const WINDOWING::Win32Window& window)
{
    // GET THE DEVICE CONTEXT.
    HDC device_context = GetDC(window.WindowHandle);
    bool device_context_retrieved = (nullptr != device_context);
    if (!device_context_retrieved)
    {
        // The render target can't be copied without a valid device context.
        return;
    }

    int render_target_width = render_target.GetWidthInPixels();
    int render_target_height = render_target.GetHeightInPixels();

    /// @todo   Bitmap info.
    // POPULATE THE BITMAP INFO DESCRIBING THE RENDER TARGET.
    BITMAPINFO bitmap_info = {};
    bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
    bitmap_info.bmiHeader.biWidth = static_cast<LONG>(render_target_width);
    // To ensure that the bitmap for the render target has an origin
    // at the top-left corner, the height needs to be made negative
    // to ensure the device independent bitmap is top-down.
    bitmap_info.bmiHeader.biHeight = -1 * static_cast<LONG>(render_target_height);
    // The number of planes must always be 1.
    bitmap_info.bmiHeader.biPlanes = 1;
    // The pixel format is a 32-bit RGB format.
    bitmap_info.bmiHeader.biBitCount = 32;
    bitmap_info.bmiHeader.biCompression = BI_RGB;
    // BI_RGB bitmaps can set the image size to 0.
    bitmap_info.bmiHeader.biSizeImage = 0;
    // Meter-based information (used for devices like printing) is not
    // known or needed.
    bitmap_info.bmiHeader.biXPelsPerMeter = 0;
    bitmap_info.bmiHeader.biYPelsPerMeter = 0;
    // Require all colors be supported by the bitmap.
    bitmap_info.bmiHeader.biClrUsed = 0;
    bitmap_info.bmiHeader.biClrImportant = 0;

    // COPY AND SCALE THE RENDER TARGET DATA TO FILL THE ENTIRE CLIENT AREA OF THE WINDOW.
    RECT client_rectangle = {};
    GetClientRect(window.WindowHandle, &client_rectangle);
    const int RENDER_TARGET_LEFT_X_POSITION = 0;
    const int RENDER_TARGET_TOP_Y_POSITION = 0;
    const uint32_t* pixel_data = render_target.GetRawData();
    // The return value is ignored since there isn't much that can be meaningfully done
    // if copying fails.
    StretchDIBits(
        device_context,
        /// @todo   Perform proper scaling later.  Right now, the pixels
        /// are rendered to scale to make debugging easier.
        /*clientRectangle.Left,
        clientRectangle.Top,
        clientRectangle.Right,
        clientRectangle.Bottom,*/
        0,
        0,
        render_target_width,
        render_target_height,
        RENDER_TARGET_LEFT_X_POSITION,
        RENDER_TARGET_TOP_Y_POSITION,
        render_target_width,
        render_target_height,
        pixel_data,
        &bitmap_info,
        DIB_RGB_COLORS,
        SRCCOPY);

    // RELEASE THE DEVICE CONTEXT.
    ReleaseDC(window.WindowHandle, device_context);
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

    // RUN A MESSAGE LOOP.
    auto start_time = std::chrono::high_resolution_clock::now();
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

#if 1
        auto current_time = std::chrono::high_resolution_clock::now();
        float total_elapsed_time = std::chrono::duration_cast<std::chrono::duration<float>>(current_time - start_time).count();
        std::string total_elapsed_time_string = std::to_string(total_elapsed_time) + "\n";
        OutputDebugStringA(total_elapsed_time_string.c_str());
#endif

        /// @todo Render to render target.
        /// @todo Fix colors - they're backwards!
        render_target.FillPixels(GRAPHICS::Color(1.0, 0.0, 0.0, 1.0f));

        /// @todo Copy render target to window.
        CopyToWindow(render_target, *g_window);
    }

    return EXIT_SUCCESS;
}
