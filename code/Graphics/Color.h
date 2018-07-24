#pragma once

#include <cstdint>
#include "Graphics/ColorFormat.h"

namespace GRAPHICS
{
    /// An RGBA color.  Can be represented either with floating-point components
    /// between [0,1] or 8-bit integral components between [0,255].
    class Color
    {
    public:
        // STATIC CONSTANTS.
        /// The maximum integral color component, expressed as a float for easy conversion.
        static constexpr float MAX_INTEGRAL_COLOR_COMPONENT = 255.0f;
        /// The color black.
        static const Color BLACK;

        // CONSTRUCTION.
        static Color Unpack(const uint32_t packed_color, const ColorFormat color_format);
        explicit Color(
            const float red,
            const float green,
            const float blue,
            const float alpha);
        explicit Color(
            const uint8_t red,
            const uint8_t green,
            const uint8_t blue,
            const uint8_t alpha);

        // OPERATORS.
        bool operator==(const Color& rhs) const;

        // FLOATING-POINT ACCESS.
        float GetRedAsFloat32() const;
        float GetGreenAsFloat32() const;
        float GetBlueAsFloat32() const;
        float GetAlphaAsFloat32() const;

        // INTEGRAL ACCESS.
        uint8_t GetRedAsUint8() const;
        uint8_t GetGreenAsUint8() const;
        uint8_t GetBlueAsUint8() const;
        uint8_t GetAlphaAsUint8() const;

        // PACKING.
        uint32_t Pack(const ColorFormat color_format) const;

    private:
        // HELPER METHODS.
        float ToFloatColorComponent(const uint8_t color_component_as_uint8) const;
        uint8_t ToIntegerColorComponent(const float color_component_as_float) const;

        // MEMBER VARIABLES.
        /// The red component of the color.
        float Red;
        /// The blue component of the color.
        float Blue;
        /// The green component of the color.
        float Green;
        /// The alpha component of the color.
        float Alpha;
    };
}
