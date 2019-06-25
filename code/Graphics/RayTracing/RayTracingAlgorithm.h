#pragma once

#include <optional>
#include "Graphics/Color.h"
#include "Graphics/RayTracing/Camera.h"
#include "Graphics/RayTracing/IObject3D.h"
#include "Graphics/RayTracing/Ray.h"
#include "Graphics/RayTracing/RayObjectIntersection.h"
#include "Graphics/RayTracing/Scene.h"
#include "Graphics/RenderTarget.h"

namespace GRAPHICS
{
/// Holds code related to ray tracing.
namespace RAY_TRACING
{
    /// A basic ray tracing algorithm.
    class RayTracingAlgorithm
    {
    public:
        void Render(const Scene& scene, GRAPHICS::RenderTarget& render_target);
        GRAPHICS::Color ComputeColor(
            const Scene& scene,
            const RayObjectIntersection& intersection,
            const unsigned int remaining_reflection_count) const;

        GRAPHICS::RAY_TRACING::Camera Camera = GRAPHICS::RAY_TRACING::Camera();

    private:
        std::optional<RayObjectIntersection> ComputeClosestIntersection(
            const Scene& scene,
            const Ray& ray,
            const IObject3D* const ignored_object = nullptr) const;
    };
}
}
