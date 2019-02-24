#pragma once

#include "Graphics/Color.h"
#include "Math/Vector3.h"

namespace GRAPHICS
{
namespace RAY_TRACING
{
    /// A colored light source emitting light from a point in a scene.
    class PointLight
    {
    public:
        // CONSTRUCTION.
        explicit PointLight(const MATH::Vector3f& world_position, const GRAPHICS::Color& color);
        
        // PUBLIC MEMBER VARIABLES FOR EASY ACCESS.
        /// The world position of the light.
        MATH::Vector3f WorldPosition = MATH::Vector3f(0.0f, 0.0f, 0.0f);
        /// The color of the light.
        GRAPHICS::Color Color = GRAPHICS::Color::BLACK;
    };
}
}
