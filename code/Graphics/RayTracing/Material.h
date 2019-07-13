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
        /// The ambient color of the material.
        GRAPHICS::Color AmbientColor = GRAPHICS::Color::BLACK;
        /// The diffuse color of the material.
        GRAPHICS::Color DiffuseColor = GRAPHICS::Color::BLACK;
        /// The specular color of the material.
        GRAPHICS::Color SpecularColor = GRAPHICS::Color::BLACK;
        /// The specular power defining the shininess of specular highlights.
        float SpecularPower = 0.0f;
        /// How reflective the material is as a proportion from [0, 1].
        float ReflectivityProportion = 0.0f;
    };
}
}
