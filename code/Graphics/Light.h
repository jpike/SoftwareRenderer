#pragma once

#include "Graphics/Color.h"

namespace GRAPHICS
{
    /// The type of light.
    enum class LightType
    {
        /// Ambient light that basically illuminates an entire scene.
        AMBIENT = 0,
    };

    /// A light that an provide illumination in a 3D scene.
    class Light
    {
    public:
        /// The type of the light.
        LightType Type = LightType::AMBIENT;
        /// The color of any ambient lighting.
        GRAPHICS::Color AmbientColor = GRAPHICS::Color::BLACK;
    };
}
