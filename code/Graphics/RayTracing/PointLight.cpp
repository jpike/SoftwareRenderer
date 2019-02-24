#include "Graphics/RayTracing/PointLight.h"

namespace GRAPHICS
{
namespace RAY_TRACING
{
    /// Constructor.
    /// @param[in]  world_position - See \ref WorldPosition.
    /// @param[in]  color - See \ref Color.
    PointLight::PointLight(const MATH::Vector3f& world_position, const GRAPHICS::Color& color) :
        WorldPosition(world_position),
        Color(color)
    {}
}
}
