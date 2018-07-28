#pragma once

#include "Math/Angle.h"
#include "Math/Matrix4x4.h"
#include "Math/Vector3.h"

namespace GRAPHICS
{
    /// A camera defining the portion of a 3D scene that is currently viewable.
    class Camera
    {
    public:
        static MATH::Matrix4x4f OrthographicProjection(
            const float left_x_world_boundary,
            const float right_x_world_boundary,
            const float bottom_y_world_boundary,
            const float top_y_world_boundary,
            const float near_z_world_boundary,
            const float far_z_world_boundary);
        static MATH::Matrix4x4f PerspectiveProjection(
            const MATH::Angle<float>::Degrees vertical_field_of_view_in_degrees,
            const float aspect_ratio_width_over_height,
            const float near_z_world_boundary,
            const float far_z_world_boundary);

        MATH::Matrix4x4f ViewTransform() const;

        /// The world position of the camera.
        MATH::Vector3f WorldPosition = MATH::Vector3f(0.0f, 0.0f, 1.0f);
        /// The up direction of the camera.
        MATH::Vector3f UpDirection = MATH::Vector3f(0.0f, 1.0f, 0.0f);
        /// The world position that the camera is looking at.
        MATH::Vector3f LookAtWorldPosition = MATH::Vector3f(0.0f, 0.0f, 0.0f);
    };
}
