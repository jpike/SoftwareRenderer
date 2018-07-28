#include <algorithm>
#include <cmath>
#include "Graphics/Renderer.h"

namespace GRAPHICS
{
    /// Renders a 3D object to the render target.
    /// @param[in]  object_3D - The object to render.
    /// @param[in,out]  render_target - The target to render to.
    void Renderer::Render(const Object3D& object_3D, RenderTarget& render_target) const
    {
        // COMPUTE THE FINAL TRANSFORMATION MATRIX FOR THE OBJECT.
        MATH::Matrix4x4f object_world_transform = object_3D.WorldTransform();
        MATH::Matrix4x4f camera_view_transform = Camera.ViewTransform();

        /// @todo   Figure out how we want to put projections into camera class.
        const float LEFT_X_WORLD_BOUNDARY = Camera.WorldPosition.X - 1.0f;
        const float RIGHT_X_WORLD_BOUNDARY = Camera.WorldPosition.X + 1.0f;
        const float BOTTOM_Y_WORLD_BOUNDARY = Camera.WorldPosition.Y - 1.0f;
        const float TOP_Y_WORLD_BOUNDARY = Camera.WorldPosition.Y + 1.0f;
        const float NEAR_Z_WORLD_BOUNDARY = Camera.WorldPosition.Z - 0.5f;
        const float FAR_Z_WORLD_BOUNDARY = Camera.WorldPosition.Z - 2.5f;
        MATH::Matrix4x4f orthographic_projection_transform = Camera::OrthographicProjection(
            LEFT_X_WORLD_BOUNDARY,
            RIGHT_X_WORLD_BOUNDARY,
            BOTTOM_Y_WORLD_BOUNDARY,
            TOP_Y_WORLD_BOUNDARY,
            NEAR_Z_WORLD_BOUNDARY,
            FAR_Z_WORLD_BOUNDARY);

        const MATH::Angle<float>::Degrees VERTICAL_FIELD_OF_VIEW_IN_DEGREES(60.0f);
        const float ASPECT_RATIO_WIDTH_OVER_HEIGHT = 1.0f;
        MATH::Matrix4x4f perspective_projection_transform = Camera::PerspectiveProjection(
            VERTICAL_FIELD_OF_VIEW_IN_DEGREES,
            ASPECT_RATIO_WIDTH_OVER_HEIGHT,
            NEAR_Z_WORLD_BOUNDARY,
            FAR_Z_WORLD_BOUNDARY);

        MATH::Matrix4x4f flip_y_transform = MATH::Matrix4x4f::Scale(MATH::Vector3f(1.0f, -1.0f, 1.0f));
        MATH::Matrix4x4f scale_to_screen_transform = MATH::Matrix4x4f::Scale(MATH::Vector3f(
            static_cast<float>(render_target.GetWidthInPixels()) / 2.0f, 
            static_cast<float>(render_target.GetHeightInPixels()) / 2.0f,
            1.0f));
        MATH::Matrix4x4f translate_to_screen_center_transform = MATH::Matrix4x4f::Translation(MATH::Vector3f(
            static_cast<float>(render_target.GetWidthInPixels()) / 2.0f,
            static_cast<float>(render_target.GetHeightInPixels()) / 2.0f,
            0.0f));
        MATH::Matrix4x4 screen_transform = translate_to_screen_center_transform * scale_to_screen_transform * flip_y_transform;

        MATH::Matrix4x4f final_transform = screen_transform * orthographic_projection_transform * camera_view_transform * object_world_transform;

        // RENDER EACH TRIANGLE OF THE OBJECT.
        for (const auto& local_triangle : object_3D.Triangles)
        {
            // TRANSFORM THE TRIANGLE INTO SCREEN-SPACE.
            Triangle screen_space_triangle = local_triangle;
            for (auto& vertex : screen_space_triangle.Vertices)
            {
                /*vertex.X *= object_3D.Scale.X;
                vertex.Y *= object_3D.Scale.Y;
                vertex.Z *= object_3D.Scale.Z;

                /// @todo   Flip Y more properly.
                vertex.Y = -vertex.Y;

                vertex += object_3D.WorldPosition;*/

                MATH::Vector4f homogeneous_vertex = MATH::Vector4f::HomogeneousPositionVector(vertex);
                MATH::Vector4f transformed_vertex = final_transform * homogeneous_vertex;
                vertex = MATH::Vector3f(transformed_vertex.X, transformed_vertex.Y, transformed_vertex.Z);
                /// @todo   Flip Y more properly.
                //vertex.Y = -vertex.Y;
            }

            // RENDER THE SCREEN-SPACE TRIANGLE.
            Render(screen_space_triangle, render_target);
        }
    }

    /// Renders a single triangle to the render target.
    /// @param[in]  triangle - The triangle to render (in screen-space coordinates).
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
        float length = std::max(std::abs(delta_x), std::abs(delta_y));
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
