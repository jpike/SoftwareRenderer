#include "Graphics/Color.h"

namespace GRAPHICS
{
    const Color Color::BLACK(0.0f, 0.0f, 0.0f, 1.0f);

    /// Unpacks a color from a packed color format.
    /// @param[in]  packed_color - The color to unpack.
    /// @param[in]  color_format - The format of data in the packed color.
    /// @return The unpacked color.
    Color Color::Unpack(const uint32_t packed_color, const ColorFormat color_format)
    {
        // UNPACK ACCORDING TO THE COLOR FORMAT.
        switch (color_format)
        {
            case ColorFormat::RGBA:
            {
                // UNPACK THE COLOR.
                uint8_t red = static_cast<uint8_t>((packed_color & 0xFF000000) >> 24);
                uint8_t green = static_cast<uint8_t>((packed_color & 0x00FF0000) >> 16);
                uint8_t blue = static_cast<uint8_t>((packed_color & 0x0000FF00) >> 8);
                uint8_t alpha = static_cast<uint8_t>(packed_color & 0x000000FF);
                Color unpacked_color(red, green, blue, alpha);
                return unpacked_color;
            }
            case ColorFormat::ARGB:
            {
                // UNPACK THE COLOR.
                uint8_t alpha = static_cast<uint8_t>((packed_color & 0xFF000000) >> 24);
                uint8_t red = static_cast<uint8_t>((packed_color & 0x00FF0000) >> 16);
                uint8_t green = static_cast<uint8_t>((packed_color & 0x0000FF00) >> 8);
                uint8_t blue = static_cast<uint8_t>(packed_color & 0x000000FF);
                Color unpacked_color(red, green, blue, alpha);
                return unpacked_color;
            }
            default:
                // RETURN A DEFAULT COLOR.
                return Color::BLACK;
        }
    }

    /// Constructor taking floating-point components.
    /// @param[in]  red - The red component of the color.
    /// @param[in]  green - The green component of the color.
    /// @param[in]  blue - The blue component of the color.
    /// @param[in]  alpha - The alpha component of the color.
    Color::Color(
        const float red,
        const float green,
        const float blue,
        const float alpha) :
        Red(red),
        Green(green),
        Blue(blue),
        Alpha(alpha)
    {}

    /// Constructor taking integer components.
    /// @param[in]  red - The red component of the color.
    /// @param[in]  green - The green component of the color.
    /// @param[in]  blue - The blue component of the color.
    /// @param[in]  alpha - The alpha component of the color.
    Color::Color(
        const uint8_t red,
        const uint8_t green,
        const uint8_t blue,
        const uint8_t alpha) :
        Red(0.0f),
        Green(0.0f),
        Blue(0.0f),
        Alpha(0.0f)
    {
        // CONVERT THE INTEGER COLORS TO FLOATING-POINT.
        Red = ToFloatColorComponent(red);
        Green = ToFloatColorComponent(green);
        Blue = ToFloatColorComponent(blue);
        Alpha = ToFloatColorComponent(alpha);
    }

    /// Determines if two colors are equal.
    /// @param[in]  rhs - The color to compare with.
    /// @return True if the provided color equals this color; false otherwise.
    bool Color::operator==(const Color& rhs) const
    {
        // COMPARE USING INTEGER FORMAT TO AVOID FLOATING-POINT ROUNDING ISSUES.
        // Early returns are used for each color component to avoid unnecessary
        // comparisons if unequal components are found.
        bool red_equal = (this->GetRedAsUint8() == rhs.GetRedAsUint8());
        if (!red_equal) return false;

        bool green_equal = (this->GetGreenAsUint8() == rhs.GetGreenAsUint8());
        if (!green_equal) return false;

        bool blue_equal = (this->GetBlueAsUint8() == rhs.GetBlueAsUint8());
        if (!blue_equal) return false;

        bool alpha_equal = (this->GetAlphaAsUint8() == rhs.GetAlphaAsUint8());
        if (!alpha_equal) return false;

        // All color components were equal.
        return true;
    }

    /// Returns the red component in 32-bit floating-point format.
    /// @return The red component as a float.
    float Color::GetRedAsFloat32() const
    {
        return Red;
    }

    /// Returns the green component in 32-bit floating-point format.
    /// @return The green component as a float.
    float Color::GetGreenAsFloat32() const
    {
        return Green;
    }

    /// Returns the blue component in 32-bit floating-point format.
    /// @return The blue component as a float.
    float Color::GetBlueAsFloat32() const
    {
        return Blue;
    }

    /// Returns the alpha component in 32-bit floating-point format.
    /// @return The alpha component as a float.
    float Color::GetAlphaAsFloat32() const
    {
        return Alpha;
    }

    /// Returns the red component in 8-bit integral format.
    /// @return The red component as a 8-bit integer.
    uint8_t Color::GetRedAsUint8() const
    {
        uint8_t red_as_uint8 = ToIntegerColorComponent(Red);
        return red_as_uint8;
    }

    /// Returns the green component in 8-bit integral format.
    /// @return The green component as a 8-bit integer.
    uint8_t Color::GetGreenAsUint8() const
    {
        uint8_t green_as_uint8 = ToIntegerColorComponent(Green);
        return green_as_uint8;
    }

    /// Returns the blue component in 8-bit integral format.
    /// @return The blue component as a 8-bit integer.
    uint8_t Color::GetBlueAsUint8() const
    {
        uint8_t blue_as_uint8 = ToIntegerColorComponent(Blue);
        return blue_as_uint8;
    }

    /// Returns the alpha component in 8-bit integral format.
    /// @return The alpha component as a 8-bit integer.
    uint8_t Color::GetAlphaAsUint8() const
    {
        uint8_t alpha_as_uint8 = ToIntegerColorComponent(Alpha);
        return alpha_as_uint8;
    }

    uint32_t Color::Pack(const ColorFormat color_format) const
    {
        // GET THE COLOR COMPONENTS.
        uint8_t red = GetRedAsUint8();
        uint8_t green = GetGreenAsUint8();
        uint8_t blue = GetBlueAsUint8();
        uint8_t alpha = GetRedAsUint8();

        // PACK ACCORDING TO THE COLOR FORMAT.
        switch (color_format)
        {
            case ColorFormat::RGBA:
            {
                // PACK THE COLOR.
                uint32_t packed_color =
                    (red << 24) |
                    (green << 16) |
                    (blue << 8) |
                    alpha;
                return packed_color;
            }
            case ColorFormat::ARGB:
            {
                // PACK THE COLOR.
                uint32_t packed_color =
                    (alpha << 24) |
                    (red << 16) |
                    (green << 8) |
                    blue;
                return packed_color;
            }
            default:
                // RETURN A DEFAULT COLOR.
                return 0x00000000;
        }
    }

    /// Converts an 8-bit integral color component to floating-point format.
    /// @param[in]  color_component_as_uint8 - The color component to convert.
    /// @return The floating-point version of the color component.
    float Color::ToFloatColorComponent(const uint8_t color_component_as_uint8) const
    {
        // CAST THE INTEGRAL COMPONENT TO FLOATING-POINT TO AVOID TRUNCATION.
        float original_color_component = static_cast<float>(color_component_as_uint8);

        // SCALE THE COLOR COMPONENT FROM RANGE [0,255] to [0,1].
        float color_component_as_float = (original_color_component / MAX_INTEGRAL_COLOR_COMPONENT);

        return color_component_as_float;
    }

    /// Converts a 32-bit color component to 8-bit integral format.
    /// @param[in]  color_component_as_float - The color component to convert.
    /// @return The 8-bit integer version of the color component.
    uint8_t Color::ToIntegerColorComponent(const float color_component_as_float) const
    {
        // SCALE THE COLOR COMPONENT FROM RANGE [0,1] TO [0,255].
        float scaled_color_component = (color_component_as_float * MAX_INTEGRAL_COLOR_COMPONENT);

        // CAST THE SCALED COLOR COMPONENT TO AN 8-BIT INTEGER.
        uint8_t color_component_as_uint8 = static_cast<uint8_t>(scaled_color_component);
        return color_component_as_uint8;
    }
}
