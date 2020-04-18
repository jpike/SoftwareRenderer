#include <limits>
#include "Graphics/Gui/Font.h"

namespace GRAPHICS::GUI
{
    /// Loads the system's default fixed-width font.
    /// @return The system's default fixed-width font, if successfully loaded;
    ///     std::nullopt otherwise.
    std::optional<Font> Font::LoadSystemDefaultFixedFont()
    {
        // GET A DEVICE CONTEXT IN WHICH TO LOAD THE FONT.
        HDC entire_screen_device_context = GetDC(NULL);
        HDC font_device_context = CreateCompatibleDC(entire_screen_device_context);
        /// @todo   Error handling for return values?

        // CREATE A BITMAP FOR RENDERING THE FONT.
        BITMAPINFO bitmap_info = {};
        bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
        bitmap_info.bmiHeader.biWidth = GLYPH_BITMAP_DIMENSION_IN_PIXELS;
        bitmap_info.bmiHeader.biHeight = -static_cast<long>(GLYPH_BITMAP_DIMENSION_IN_PIXELS);
        bitmap_info.bmiHeader.biPlanes = 1;
        bitmap_info.bmiHeader.biBitCount = 32;
        bitmap_info.bmiHeader.biCompression = BI_RGB;
        bitmap_info.bmiHeader.biSizeImage = 0;
        bitmap_info.bmiHeader.biClrUsed = 0;
        bitmap_info.bmiHeader.biClrImportant = 0;
        void* font_bits;
        constexpr HANDLE ALLOCATE_MEMORY_RATHER_THAN_USE_FILE_MAPPING = nullptr;
        constexpr DWORD NO_FILE_MAPPING_OFFSET = 0;
        HBITMAP font_bitmap = CreateDIBSection(
            font_device_context, 
            &bitmap_info, 
            DIB_RGB_COLORS, 
            &font_bits, 
            ALLOCATE_MEMORY_RATHER_THAN_USE_FILE_MAPPING,
            NO_FILE_MAPPING_OFFSET);
        SelectObject(font_device_context, font_bitmap);
        SetBkColor(font_device_context, RGB(0, 0, 0));

        // GET A HANDLE TO THE SYSTEM FIXED FONT.
        Font system_fixed_font;
        //system_fixed_font.Handle = GetStockObject(ANSI_FIXED_FONT);
        system_fixed_font.Handle = GetStockObject(SYSTEM_FIXED_FONT);

        // ENABLE USING THE FONT.
        SelectObject(font_device_context, system_fixed_font.Handle);

        // GET THE BASIC TEXT METRICS FOR THE FONT.
        GetTextMetrics(font_device_context, &system_fixed_font.TextMetrics);

        // RENDER ALL GLYPHS FOR THE FONT.
        SetTextColor(font_device_context, RGB(255, 255, 255));
        RECT current_glyphs_rectangle =
        {
            .left = 0,
            .top = 0,
            .right = GLYPH_DIMENSION_IN_PIXELS,
            .bottom = GLYPH_DIMENSION_IN_PIXELS
        };
        for (unsigned int character_value = 0; character_value <= std::numeric_limits<unsigned char>::max(); ++character_value)
        //for (unsigned int character_value = 'A'; character_value <= 'Z'; ++character_value)
        {
            // DRAW THE CURRENT CHARACTER.
            constexpr int SINGLE_CHARACTER_COUNT = 1;
            char character = static_cast<char>(character_value);
            DrawText(font_device_context, &character, SINGLE_CHARACTER_COUNT, &current_glyphs_rectangle, DT_CENTER);

            // GET ALL PIXELS FOR THE CURRENT GLYPH.
            for (int y = current_glyphs_rectangle.top; y <= current_glyphs_rectangle.bottom; ++y)
            {
                for (int x = current_glyphs_rectangle.left; x <= current_glyphs_rectangle.right; ++x)
                {
                    COLORREF pixel_color = GetPixel(font_device_context, x, y);
                    if (pixel_color)
                    {
                        pixel_color |= 0xFF000000;
                        system_fixed_font.Pixels.WritePixel(x, y, pixel_color);
                    }
                }
            }

            // MOVE TO THE NEXT CHARACTER.
            current_glyphs_rectangle.left += GLYPH_DIMENSION_IN_PIXELS;
            current_glyphs_rectangle.right += GLYPH_DIMENSION_IN_PIXELS;
            bool current_row_of_glyphs_filled = current_glyphs_rectangle.right >= GLYPH_BITMAP_DIMENSION_IN_PIXELS;
            if (current_row_of_glyphs_filled)
            {
                // MOVE TO THE NEXT ROW FOR DRAWING.
                current_glyphs_rectangle.left = 0;
                current_glyphs_rectangle.right = GLYPH_DIMENSION_IN_PIXELS;
                current_glyphs_rectangle.top += GLYPH_DIMENSION_IN_PIXELS;
                current_glyphs_rectangle.bottom += GLYPH_DIMENSION_IN_PIXELS;
            }
        }

        // RETURN THE FONT.
        return system_fixed_font;
    }
}
