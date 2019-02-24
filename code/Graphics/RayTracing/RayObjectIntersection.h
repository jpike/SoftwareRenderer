#pragma once

#include <limits>

namespace GRAPHICS
{
namespace RAY_TRACING
{
    // Forward declarations.
    class IObject3D;

    /// An intersection between a ray and an object in a 3D scene.
    class RayObjectIntersection
    {
    public:

        // PUBLIC MEMBER VARIABLES FOR EASY ACCESS.
        /// The distance along the ray to the intersection of the object (in units of the ray).
        /// Initialized to infinity to avoid accidental intersections caused by checking
        /// if this distance is closer between two intersections.
        float DistanceFromRayToObject = std::numeric_limits<float>::infinity();
        /// The intersected object.  Memory is managed externally (outside of this class).
        const IObject3D* Object = nullptr;
    };
}
}
