#pragma once

#include <optional>
#include "Graphics/RayTracing/Ray.h"
#include "Graphics/RayTracing/RayObjectIntersection.h"

namespace GRAPHICS
{
namespace RAY_TRACING
{
    /// An interface for ray-traceable objects in a 3D scene.
    class IObject3D
    {
    public:
        /// Checks for an intersection between a ray and the object.
        /// Intended to be implemented in derived classes so that each
        /// derived class can handle its own specific intersection logic.
        /// @param[in]  ray - The ray to check for intersection.
        /// @return A ray-object intersection, if one occurred; std::nullopt otherwise.
        virtual std::optional<RayObjectIntersection> Intersect(const Ray& ray) const = 0;
    };
}
}