#include "Graphics/RayTracing/PointLight.h"

namespace GRAPHICS
{
namespace RAY_TRACING
{
    /// Constructor.
    /// @param[in]  world_position - See @ref WorldPosition.
    /// @param[in]  color - See @ref Color.
    PointLight::PointLight(const MATH::Vector3f& world_position, const GRAPHICS::Color& color) :
        WorldPosition(world_position),
        Color(color)
    {}

    /// Computes the direction from the specified world position to this point light.
    /// @param[in]  other_world_position - The other world position to compute the direction from.
    /// @return The normalized direction from the specified point to this light's position.
    MATH::Vector3f PointLight::DirectionFrom(const MATH::Vector3f& other_world_position) const
    {
        MATH::Vector3f direction_from_other_point_to_light = WorldPosition - other_world_position;
        MATH::Vector3f normalized_direction_from_other_point_to_light = MATH::Vector3f::Normalize(direction_from_other_point_to_light);
        return normalized_direction_from_other_point_to_light;
    }
}
}
