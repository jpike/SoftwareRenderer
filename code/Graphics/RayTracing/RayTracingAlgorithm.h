#pragma once

#include "Graphics/RayTracing/Camera.h"
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

        GRAPHICS::RAY_TRACING::Camera Camera = GRAPHICS::RAY_TRACING::Camera();
    };
}
}
