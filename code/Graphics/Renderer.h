#pragma once

#include <vector>
#include "Graphics/Color.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/Triangle.h"

namespace GRAPHICS
{
    /// A software renderer.
    class Renderer
    {
    public:
        void Render(const std::vector<Triangle>& triangles, RenderTarget& render_target) const;

    private:
        void Render(const Triangle& triangle, RenderTarget& render_target) const;

        void DrawLine(
            const float start_x,
            const float start_y,
            const float end_x,
            const float end_y,
            const Color& color,
            RenderTarget& render_target) const;
    };
}
