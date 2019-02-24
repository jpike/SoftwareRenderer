#pragma once

#include "Math/Angle.h"
#include "Math/Vector3.h"

namespace GRAPHICS
{
namespace RAY_TRACING
{
    /// A camera defining the portion of a 3D scene that is currently viewable.
    /// @todo   After interfaces are worked out, see about combining this with
    ///     the regular GRAPHICS::Camera class.
    class Camera
    {
    public:
        /// The position of the camera in the world.
        MATH::Vector3f WorldPosition = MATH::Vector3f();
        /// The field of view of the camera.
        MATH::Angle<float>::Degrees FieldOfView = MATH::Angle<float>::Degrees(0.0f);
    };
}
}
