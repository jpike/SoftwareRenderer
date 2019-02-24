#pragma once

#include "Graphics/Color.h"

namespace GRAPHICS
{
namespace RAY_TRACING
{
    /// A material defining properties of a surface.
    class Material
    {
    public:
        GRAPHICS::Color DiffuseColor = GRAPHICS::Color::BLACK;
        GRAPHICS::Color AmbientColor = GRAPHICS::Color::BLACK;
        GRAPHICS::Color SpecularColor = GRAPHICS::Color::BLACK;
        float SpecularPower = 0.0f;
        float KReflected = 0.0f;
    };
}
}
