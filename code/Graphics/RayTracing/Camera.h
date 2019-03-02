#pragma once

#include "Graphics/ProjectionType.h"
#include "Graphics/RayTracing/Ray.h"
#include "Graphics/RayTracing/ViewingPlane.h"
#include "Graphics/RenderTarget.h"
#include "Math/Angle.h"
#include "Math/CoordinateFrame.h"
#include "Math/Vector2.h"
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
        // CONSTRUCTION.
        static Camera LookAt(const MATH::Vector3f& look_at_world_position);

        // VIEWING RAY METHODS.
        Ray ViewingRay(
            const MATH::Vector2ui& pixel_coordinates,
            const RenderTarget& render_target) const;
        
        // PUBLIC MEMBER VARIABLES FOR EASY ACCESS.
        /// The type of projection the camera is currently using.
        ProjectionType Projection = ProjectionType::ORTHOGRAPHIC;
        /// The position of the camera in the world.
        MATH::Vector3f WorldPosition = MATH::Vector3f(0.0f, 0.0f, 1.0f);
        /// The coordinate frame for the camera's orientation.
        MATH::CoordinateFrame CoordinateFrame = MATH::CoordinateFrame();
        /// The field of view of the camera.
        MATH::Angle<float>::Degrees FieldOfView = MATH::Angle<float>::Degrees(45.0f);
        /// The viewing plane onto which the image is projected.
        GRAPHICS::RAY_TRACING::ViewingPlane ViewingPlane = GRAPHICS::RAY_TRACING::ViewingPlane();
    };
}
}
