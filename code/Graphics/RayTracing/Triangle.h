#pragma once

#include <array>
#include "Graphics/RayTracing/IObject3D.h"
#include "Graphics/RayTracing/Material.h"
#include "Math/Vector3.h"

namespace GRAPHICS
{
namespace RAY_TRACING
{
    /// A triangle that can be ray traced.
    class Triangle : public IObject3D
    {
    public:
        // STATIC CONSTANTS.
        /// The number of vertices in a triangle.
        static constexpr std::size_t VERTEX_COUNT = 3;

        // INTERSECTION.
        std::optional<RayObjectIntersection> Intersect(const Ray& ray) const override;

        // PUBLIC MEMBER VARIABLES FOR EASY ACCESS.
        /// The vertices of the triangle.
        /// Should be in counter-clockwise order.
        std::array<MATH::Vector3f, VERTEX_COUNT> Vertices = {};
        /// The material defining surface properties of the triangle.
        GRAPHICS::RAY_TRACING::Material Material = GRAPHICS::RAY_TRACING::Material();

    private:
        // HELPER METHODS.
        MATH::Vector3f SurfaceNormal() const;
    };
}
}
