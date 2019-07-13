#pragma once

#include "Graphics/RayTracing/IObject3D.h"
#include "Graphics/RayTracing/Material.h"
#include "Math/Vector3.h"

namespace GRAPHICS
{
namespace RAY_TRACING
{
    /// A sphere that can be ray traced.
    class Sphere : public IObject3D
    {
    public:
        // PUBLIC METHODS.
        MATH::Vector3f SurfaceNormal(const MATH::Vector3f& surface_point) const override;
        Material GetMaterial() const override;
        std::optional<RayObjectIntersection> Intersect(const Ray& ray) const override;

        // PUBLIC MEMBER VARIABLES FOR EASY ACCESS.
        /// The center of the sphere in world coordinates.
        MATH::Vector3f CenterPosition = MATH::Vector3f();
        /// The radius of the sphere.
        float Radius = 0.0f;
        /// The material defining surface properties of the sphere.
        GRAPHICS::RAY_TRACING::Material Material = GRAPHICS::RAY_TRACING::Material();
    };
}
}