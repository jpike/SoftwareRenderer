#include <algorithm>
#include <cmath>
#include "Graphics/Renderer.h"

namespace GRAPHICS
{
    /// Renders a list of triangles to the render target.
    /// @param[in]  triangles - The triangles to render.
    /// @param[in,out]  render_target - The target to render to.
    void Renderer::Render(const std::vector<Triangle>& triangles, RenderTarget& render_target) const
    {
        // RENDER EACH TRIANGLE.
        for (const auto& triangle : triangles)
        {
            Render(triangle, render_target);
        }
    }

    /// Renders a single triangle to the render target.
    /// @param[in]  triangle - The triangle to render.
    /// @param[in,out]  render_target - The target to render to.
    void Renderer::Render(const Triangle& triangle, RenderTarget& render_target) const
    {
        // DRAW THE FIRST EDGE.
        const MATH::Vector3f& first_vertex = triangle.Vertices[0];
        const MATH::Vector3f& second_vertex = triangle.Vertices[1];
        DrawLine(
            first_vertex.X,
            first_vertex.Y,
            second_vertex.X,
            second_vertex.Y,
            triangle.Color,
            render_target);

        // DRAW THE SECOND EDGE.
        const MATH::Vector3f& third_vertex = triangle.Vertices[2];
        DrawLine(
            second_vertex.X,
            second_vertex.Y,
            third_vertex.X,
            third_vertex.Y,
            triangle.Color,
            render_target);

        // DRAW THE THIRD EDGE.
        DrawLine(
            third_vertex.X,
            third_vertex.Y,
            first_vertex.X,
            first_vertex.Y,
            triangle.Color,
            render_target);
    }

    /// Renders a line with the specified endpoints (in screen coordinates).
    /// @param[in]  start_x - The starting x coordinate of the line.
    /// @param[in]  start_y - The starting y coordinate of the line.
    /// @param[in]  end_x - The ending x coordinate of the line.
    /// @param[in]  end_y - The ending y coordinate of the line.
    /// @param[in]  color - The color of the line to draw.
    /// @param[in,out]  render_target - The target to render to.
    void Renderer::DrawLine(
        const float start_x,
        const float start_y,
        const float end_x,
        const float end_y,
        const Color& color,
        RenderTarget& render_target) const
    {
        // COMPUTE THE INCREMENTS ALONG EACH AXIS FOR EACH PIXEL.
        // Each time we draw a pixel, we need to move slightly
        // further along the axes.
        float delta_x = end_x - start_x;
        float delta_y = end_y - start_y;
        float length = std::max(abs(delta_x), abs(delta_y));
        float x_increment = delta_x / length;
        float y_increment = delta_y / length;

        // HAVE THE LINE START BEING DRAWN AT THE STARTING COORDINATES.
        float x = start_x;
        float y = start_y;

        // GET THE MAXIMUM POSSIBLE POSITION VALUES.
        float max_x_position = static_cast<float>(render_target.GetWidthInPixels() - 1);
        float max_y_position = static_cast<float>(render_target.GetHeightInPixels() - 1);

        // DRAW PIXELS FOR THE LINE.
        for (float pixel_index = 0.0f; pixel_index <= length; ++pixel_index)
        {
            // PREVENT WRITING BEYOND THE BOUNDARIES OF THE RENDER TARGET.
            bool x_boundary_exceeded = (
                (x < 0.0f) ||
                (x > max_x_position));
            bool y_boundary_exceeded = (
                (y < 0.0f) ||
                (y > max_y_position));
            bool boundary_exceeded = (x_boundary_exceeded || y_boundary_exceeded);
            if (boundary_exceeded)
            {
                // Continue to the next iteration of the loop in
                // case there is another pixel to draw.
                continue;
            }

            // DRAW A PIXEL AT THE CURRENT POSITION.
            // The coordinates need to be rounded to integer in order
            // to plot a pixel on a fixed grid.
            render_target.WritePixel(
                static_cast<unsigned int>(std::round(x)),
                static_cast<unsigned int>(std::round(y)),
                color);

            // MOVE ALONG THE LINE FOR THE NEXT PIXEL.
            x += x_increment;
            y += y_increment;
        }
    }
}
