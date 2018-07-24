#include "Windowing/Win32Window.h"

namespace WINDOWING
{
    /// Attempts to create a new window based on the provided description.
    /// The window will be displayed if successfully created.
    /// @param[in]  window_class - The class describing the type of window to create.
    /// @param[in]  window_title - The text to display in the window's title bar.
    /// @param[in]  width_in_pixels - The width of the client area within the window, in pixels.
    /// @param[in]  height_in_pixels - The height of the client area within the window, in pixels.
    /// @return     The window if successfully created; null otherwise.
    std::unique_ptr<Win32Window> Win32Window::Create(
        const WNDCLASSEXA& window_class,
        const std::string& window_title,
        const int width_in_pixels,
        const int height_in_pixels)
    {
        // REGISTER THE WINDOW CLASS SO IT CAN BE USED.
        ATOM window_class_id = RegisterClassEx(&window_class);
        const ATOM FAILED_WINDOW_CLASS_ID = 0;
        bool registration_successful = (FAILED_WINDOW_CLASS_ID != window_class_id);
        if (!registration_successful)
        {
            return nullptr;
        }

        // CREATE THE WINDOW.
        const DWORD NO_EXTENDED_STYLE = 0;
        const DWORD WINDOW_STYLE = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
        const int DEFAULT_LEFT_X_POSITION = CW_USEDEFAULT;
        const int DEFAULT_TOP_Y_POSITION = CW_USEDEFAULT;
        const HWND NO_PARENT_WINDOW = NULL;
        const HMENU NO_WINDOW_MENU = NULL;
        const LPVOID NO_ADDITIONAL_CREATE_PARAMETERS = NULL;
        HWND window_handle = CreateWindowEx(
            NO_EXTENDED_STYLE,
            window_class.lpszClassName,
            window_title.c_str(),
            WINDOW_STYLE,
            DEFAULT_LEFT_X_POSITION,
            DEFAULT_TOP_Y_POSITION,
            width_in_pixels,
            height_in_pixels,
            NO_PARENT_WINDOW,
            NO_WINDOW_MENU,
            window_class.hInstance,
            NO_ADDITIONAL_CREATE_PARAMETERS);
        bool window_created = (NULL != window_handle);
        if (!window_created)
        {
            return nullptr;
        }

        /// @todo   Figure out how to guarantee pixel dimensions of window.
        /// Things like AdjustWindowRect, MoveWindow, and SetWindowPos don't seem to be working.

        // WRAP THE WINDOW HANDLE IN THE WINDOW OBJECT.
        std::unique_ptr<Win32Window> window = std::make_unique<Win32Window>(window_handle);
        return window;
    }

    /// Constructor.
    /// @param[in]  window_handle - The operating system handle to the window
    ///     wrapped by this class.
    Win32Window::Win32Window(const HWND window_handle) :
        WindowHandle(window_handle)
    {}
}