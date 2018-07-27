#include "Graphics/Object3D.h"

namespace GRAPHICS
{
    /// Gets the world transformation matrix of the object.
    /// @return The object's world transform.
    MATH::Matrix4x4f Object3D::WorldTransform() const
    {
        MATH::Matrix4x4f translation_matrix = MATH::Matrix4x4f::Translation(WorldPosition);
        /// @todo   Add scaling!
        MATH::Matrix4x4f rotation_matrix = MATH::Matrix4x4f::Rotation(RotationInRadians);

        MATH::Matrix4x4f world_transform = translation_matrix * rotation_matrix;
        return world_transform;
    }
}
