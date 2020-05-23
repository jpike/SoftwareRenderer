#include <algorithm>
#include <cmath>
#include "Graphics/Renderer.h"

namespace GRAPHICS
{
    /// Renders some text onto the render target.
    /// @param[in]  text - The text to render.
    /// @param[in,out]  render_target - The target to render to.
    void Renderer::Render(const GUI::Text& text, RenderTarget& render_target) const
    {
        // MAKE SURE A FONT EXISTS.
        if (!text.Font)
        {
            return;
        }

        // RENDER A GLYPH FOR EACH CHARACTER.
        unsigned int current_glyph_left_x_position = static_cast<unsigned int>(text.LeftTopPosition.X);
        unsigned int current_glyph_top_y_position = static_cast<unsigned int>(text.LeftTopPosition.Y);
        for (char character : text.String)
        {
            // RENDER ALL PIXELS FOR THE CURRENT GLYPH.
            const GUI::Glyph& glyph = text.Font->GlyphsByCharacter[static_cast<unsigned char>(character)];
            for (unsigned int glyph_local_pixel_y_position = 0; glyph_local_pixel_y_position < glyph.HeightInPixels; ++glyph_local_pixel_y_position)
            {
                for (unsigned int glyph_local_pixel_x_position = 0; glyph_local_pixel_x_position < glyph.WidthInPixels; ++glyph_local_pixel_x_position)
                {
                    // ONLY WRITE THE PIXEL IF IT IS VISIBLE.
                    /// @todo   Fancier alpha blending?
                    GRAPHICS::Color pixel_color = glyph.GetPixelColor(glyph_local_pixel_x_position, glyph_local_pixel_y_position);
                    bool pixel_visible = (pixel_color.Alpha > 0);
                    if (pixel_visible)
                    {
                        unsigned int glyph_destination_x_position = current_glyph_left_x_position + glyph_local_pixel_x_position;
                        unsigned int glyph_destination_y_position = current_glyph_top_y_position + glyph_local_pixel_y_position;
                        render_target.WritePixel(glyph_destination_x_position, glyph_destination_y_position, pixel_color);
                    }
                }
            }

            // MOVE TO THE NEXT GLYPH.
            current_glyph_left_x_position += glyph.WidthInPixels;
        }
    }

    /// Renders a 3D object to the render target.
    /// @param[in]  object_3D - The object to render.
    /// @param[in]  lights - Any lights that should illuminate the object.
    /// @param[in,out]  render_target - The target to render to.
    void Renderer::Render(const Object3D& object_3D, const std::vector<Light>& lights, RenderTarget& render_target) const
    {
        // COMPUTE THE FINAL TRANSFORMATION MATRIX FOR THE OBJECT.
        MATH::Matrix4x4f object_world_transform = object_3D.WorldTransform();
        MATH::Matrix4x4f camera_view_transform = Camera.ViewTransform();

        /// @todo   Figure out how we want to put projections into camera class.
        constexpr float WORLD_HALF_SIZE = 100.0f;
        const float LEFT_X_WORLD_BOUNDARY = Camera.WorldPosition.X - WORLD_HALF_SIZE;
        const float RIGHT_X_WORLD_BOUNDARY = Camera.WorldPosition.X + WORLD_HALF_SIZE;
        const float BOTTOM_Y_WORLD_BOUNDARY = Camera.WorldPosition.Y - WORLD_HALF_SIZE;
        const float TOP_Y_WORLD_BOUNDARY = Camera.WorldPosition.Y + WORLD_HALF_SIZE;
        //const float NEAR_Z_WORLD_BOUNDARY = Camera.WorldPosition.Z - 0.5f;
        //const float FAR_Z_WORLD_BOUNDARY = Camera.WorldPosition.Z - 2.5f;
        const float NEAR_Z_WORLD_BOUNDARY = Camera.WorldPosition.Z - 1.0f;
        const float FAR_Z_WORLD_BOUNDARY = Camera.WorldPosition.Z - 500.0f;
        MATH::Matrix4x4f orthographic_projection_transform = Camera::OrthographicProjection(
            LEFT_X_WORLD_BOUNDARY,
            RIGHT_X_WORLD_BOUNDARY,
            BOTTOM_Y_WORLD_BOUNDARY,
            TOP_Y_WORLD_BOUNDARY,
            NEAR_Z_WORLD_BOUNDARY,
            FAR_Z_WORLD_BOUNDARY);

        const MATH::Angle<float>::Degrees VERTICAL_FIELD_OF_VIEW_IN_DEGREES(90.0f);
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

        //MATH::Matrix4x4f final_transform = screen_transform * perspective_projection_transform * camera_view_transform * object_world_transform;

        // RENDER EACH TRIANGLE OF THE OBJECT.
        for (const auto& local_triangle : object_3D.Triangles)
        {
            // TRANSFORM THE TRIANGLE INTO SCREEN-SPACE.
            bool triangle_within_camera_z_boundaries = false;
            Triangle screen_space_triangle = local_triangle;
            for (auto& vertex : screen_space_triangle.Vertices)
            {
                // Y must be flipped since the world Y coordinates are positive going up,
                // the opposite is true for the screen coordinates.
                vertex.Y = -vertex.Y;
                MATH::Vector4f homogeneous_vertex = MATH::Vector4f::HomogeneousPositionVector(vertex);

                MATH::Vector4f world_vertex = object_world_transform * homogeneous_vertex;
                MATH::Vector4f view_vertex = camera_view_transform * world_vertex;
                MATH::Vector4f projected_vertex = perspective_projection_transform * view_vertex;
                MATH::Vector4f screen_vertex = screen_transform * projected_vertex;
                MATH::Vector4f transformed_vertex = screen_vertex;

                //MATH::Vector4f transformed_vertex = final_transform * homogeneous_vertex;
                vertex = MATH::Vector3f(transformed_vertex.X, transformed_vertex.Y, transformed_vertex.Z);
                // The vertex must be de-homogenized.
                vertex = MATH::Vector3f::Scale(1.0f / transformed_vertex.W, vertex);

                // @todo    I'm not sure this clipping is correct.  More study/testing needed.
                bool vertex_within_camera_z_boundaries = 
                    // The vertex must be within the boundaries of the camera's viewing planes.
                    // Since these boundaries are facing along the negative z axis,
                    // the far boundary is actually larger (less negative) than the near boundary.
                    ((FAR_Z_WORLD_BOUNDARY <= world_vertex.Z) && (world_vertex.Z <= NEAR_Z_WORLD_BOUNDARY)) &&
                    // Within the camera viewing plane's boundaries, the w coordinate should never be
                    // greater than the z coordinate - that would indicate that we've basically
                    // "flipped" the size of the viewing plane that we're on.  In those cases,
                    // we must prevent incorrect (flipped) rendering.
                    (std::abs(transformed_vertex.Z) >= std::abs(transformed_vertex.W));
                triangle_within_camera_z_boundaries = (triangle_within_camera_z_boundaries || vertex_within_camera_z_boundaries);
            }

            // RENDER THE SCREEN-SPACE TRIANGLE.
            if (triangle_within_camera_z_boundaries)
            {
                Render(screen_space_triangle, lights, render_target);
            }
        }
    }

    /// Renders a single triangle to the render target.
    /// @param[in]  triangle - The triangle to render (in screen-space coordinates).
    /// @param[in]  lights - Any lights illuminating the triangle.
    /// @param[in,out]  render_target - The target to render to.
    void Renderer::Render(const Triangle& triangle, const std::vector<Light>& lights, RenderTarget& render_target) const
    {
        // GET THE VERTICES.
        // They're needed for all kinds of shading.
        const MATH::Vector3f& first_vertex = triangle.Vertices[0];
        const MATH::Vector3f& second_vertex = triangle.Vertices[1];
        const MATH::Vector3f& third_vertex = triangle.Vertices[2];

        // RENDER THE TRIANGLE BASED ON SHADING TYPE.
        switch (triangle.Material->Shading)
        {
            case ShadingType::WIREFRAME:
            {
                // COMPUTE THE COLOR BASED ON LIGHTING.
                Color wireframe_color = triangle.Material->WireframeColor;
                for (const Light& light : lights)
                {
                    // COMPUTE SHADING BASED ON TYPE OF LIGHT.
                    if (LightType::AMBIENT == light.Type)
                    {
                        /// @todo   Should this be multiplicative or additive?
                        wireframe_color = Color::ComponentMultiplyRedGreenBlue(wireframe_color, light.AmbientColor);
                    }
                }

                // DRAW THE FIRST EDGE.
                DrawLine(
                    first_vertex.X,
                    first_vertex.Y,
                    second_vertex.X,
                    second_vertex.Y,
                    wireframe_color,
                    render_target);

                // DRAW THE SECOND EDGE.
                DrawLine(
                    second_vertex.X,
                    second_vertex.Y,
                    third_vertex.X,
                    third_vertex.Y,
                    wireframe_color,
                    render_target);

                // DRAW THE THIRD EDGE.
                DrawLine(
                    third_vertex.X,
                    third_vertex.Y,
                    first_vertex.X,
                    first_vertex.Y,
                    wireframe_color,
                    render_target);
                break;
            }
            case ShadingType::WIREFRAME_VERTEX_COLOR_INTERPOLATION:
            {
                // COMPUTE THE COLORS BASED ON LIGHTING.
                Color vertex_0_wireframe_color = triangle.Material->VertexWireframeColors[0];
                Color vertex_1_wireframe_color = triangle.Material->VertexWireframeColors[1];
                Color vertex_2_wireframe_color = triangle.Material->VertexWireframeColors[2];
                for (const Light& light : lights)
                {
                    // COMPUTE SHADING BASED ON TYPE OF LIGHT.
                    if (LightType::AMBIENT == light.Type)
                    {
                        /// @todo   Should this be multiplicative or additive?
                        vertex_0_wireframe_color = Color::ComponentMultiplyRedGreenBlue(vertex_0_wireframe_color, light.AmbientColor);
                        vertex_1_wireframe_color = Color::ComponentMultiplyRedGreenBlue(vertex_1_wireframe_color, light.AmbientColor);
                        vertex_2_wireframe_color = Color::ComponentMultiplyRedGreenBlue(vertex_2_wireframe_color, light.AmbientColor);
                    }
                }

                // DRAW THE FIRST EDGE.
                DrawLineWithInterpolatedColor(
                    first_vertex.X,
                    first_vertex.Y,
                    second_vertex.X,
                    second_vertex.Y,
                    vertex_0_wireframe_color,
                    vertex_1_wireframe_color,
                    render_target);

                // DRAW THE SECOND EDGE.
                DrawLineWithInterpolatedColor(
                    second_vertex.X,
                    second_vertex.Y,
                    third_vertex.X,
                    third_vertex.Y,
                    vertex_1_wireframe_color,
                    vertex_2_wireframe_color,
                    render_target);

                // DRAW THE THIRD EDGE.
                DrawLineWithInterpolatedColor(
                    third_vertex.X,
                    third_vertex.Y,
                    first_vertex.X,
                    first_vertex.Y,
                    vertex_2_wireframe_color,
                    vertex_0_wireframe_color,
                    render_target);
                break;
            }
            case ShadingType::FLAT:
            {
                // COMPUTE THE BARYCENTRIC COORDINATES OF THE TRIANGLE VERTICES.
                float top_vertex_signed_distance_from_bottom_edge = (
                    ((second_vertex.Y - third_vertex.Y) * first_vertex.X) +
                    ((third_vertex.X - second_vertex.X) * first_vertex.Y) +
                    (second_vertex.X * third_vertex.Y) -
                    (third_vertex.X * second_vertex.Y));
                float right_vertex_signed_distance_from_left_edge = (
                    ((second_vertex.Y - first_vertex.Y) * third_vertex.X) +
                    ((first_vertex.X - second_vertex.X) * third_vertex.Y) +
                    (second_vertex.X * first_vertex.Y) -
                    (first_vertex.X * second_vertex.Y));

                // GET THE BOUNDING RECTANGLE OF THE TRIANGLE.
                /// @todo   Create rectangle class.
                float min_x = std::min({ first_vertex.X, second_vertex.X, third_vertex.X });
                float max_x = std::max({ first_vertex.X, second_vertex.X, third_vertex.X });
                float min_y = std::min({ first_vertex.Y, second_vertex.Y, third_vertex.Y });
                float max_y = std::max({ first_vertex.Y, second_vertex.Y, third_vertex.Y });

                // COLOR PIXELS WITHIN THE TRIANGLE.
                constexpr float ONE_PIXEL = 1.0f;
                for (float y = min_y; y <= max_y; y += ONE_PIXEL)
                {
                    for (float x = min_x; x <= max_x; x += ONE_PIXEL)
                    {
                        // COMPUTE THE BARYCENTRIC COORDINATES OF THE CURRENT PIXEL POSITION.
                        // The following diagram shows the order of the vertices:
                        //             first_vertex
                        //                 /\
                        //                /  \
                        // second_vertex /____\ third_vertex
                        float current_pixel_signed_distance_from_bottom_edge = (
                            ((second_vertex.Y - third_vertex.Y) * x) +
                            ((third_vertex.X - second_vertex.X) * y) +
                            (second_vertex.X * third_vertex.Y) -
                            (third_vertex.X * second_vertex.Y));
                        float scaled_signed_distance_of_current_pixel_relative_to_bottom_edge = (current_pixel_signed_distance_from_bottom_edge / top_vertex_signed_distance_from_bottom_edge);

                        float current_pixel_signed_distance_from_left_edge = (
                            ((second_vertex.Y - first_vertex.Y) * x) +
                            ((first_vertex.X - second_vertex.X) * y) +
                            (second_vertex.X * first_vertex.Y) -
                            (first_vertex.X * second_vertex.Y));
                        float scaled_signed_distance_of_current_pixel_relative_to_left_edge = (current_pixel_signed_distance_from_left_edge / right_vertex_signed_distance_from_left_edge);

                        float scaled_signed_distance_of_current_pixel_relative_to_right_edge = (
                            1.0f -
                            scaled_signed_distance_of_current_pixel_relative_to_left_edge -
                            scaled_signed_distance_of_current_pixel_relative_to_bottom_edge);

                        // CHECK IF THE PIXEL IS WITHIN THE TRIANGLE.
                        // It's allowed to be on the borders too.
                        constexpr float MIN_SIGNED_DISTANCE_TO_BE_ON_EDGE = 0.0f;
                        constexpr float MAX_SIGNED_DISTANCE_TO_BE_ON_VERTEX = 1.0f;
                        bool pixel_between_bottom_edge_and_top_vertex = (
                            (MIN_SIGNED_DISTANCE_TO_BE_ON_EDGE <= scaled_signed_distance_of_current_pixel_relative_to_bottom_edge) &&
                            (scaled_signed_distance_of_current_pixel_relative_to_bottom_edge <= MAX_SIGNED_DISTANCE_TO_BE_ON_VERTEX));
                        bool pixel_between_left_edge_and_right_vertex = (
                            (MIN_SIGNED_DISTANCE_TO_BE_ON_EDGE <= scaled_signed_distance_of_current_pixel_relative_to_left_edge) &&
                            (scaled_signed_distance_of_current_pixel_relative_to_left_edge <= MAX_SIGNED_DISTANCE_TO_BE_ON_VERTEX));
                        bool pixel_between_right_edge_and_left_vertex = (
                            (MIN_SIGNED_DISTANCE_TO_BE_ON_EDGE <= scaled_signed_distance_of_current_pixel_relative_to_right_edge) &&
                            (scaled_signed_distance_of_current_pixel_relative_to_right_edge <= MAX_SIGNED_DISTANCE_TO_BE_ON_VERTEX));
                        bool pixel_in_triangle = (
                            pixel_between_bottom_edge_and_top_vertex &&
                            pixel_between_left_edge_and_right_vertex &&
                            pixel_between_right_edge_and_left_vertex);
                        if (pixel_in_triangle)
                        {
                            // COMPUTE THE COLOR BASED ON LIGHTING.
                            Color face_color = triangle.Material->FaceColor;
                            for (const Light& light : lights)
                            {
                                // COMPUTE SHADING BASED ON TYPE OF LIGHT.
                                if (LightType::AMBIENT == light.Type)
                                {
                                    /// @todo   Should this be multiplicative or additive?
                                    face_color = Color::ComponentMultiplyRedGreenBlue(face_color, light.AmbientColor);
                                }
                            }

                            // DRAW THE COLORED PIXEL.
                            // The coordinates need to be rounded to integer in order
                            // to plot a pixel on a fixed grid.
                            render_target.WritePixel(
                                static_cast<unsigned int>(std::round(x)),
                                static_cast<unsigned int>(std::round(y)),
                                face_color);
                        }
                    }
                }
                break;
            }
            case ShadingType::FACE_VERTEX_COLOR_INTERPOLATION:
            {
                // COMPUTE THE BARYCENTRIC COORDINATES OF THE TRIANGLE VERTICES.
                float top_vertex_signed_distance_from_bottom_edge = (
                    ((second_vertex.Y - third_vertex.Y) * first_vertex.X) +
                    ((third_vertex.X - second_vertex.X) * first_vertex.Y) +
                    (second_vertex.X * third_vertex.Y) -
                    (third_vertex.X * second_vertex.Y));
                float right_vertex_signed_distance_from_left_edge = (
                    ((second_vertex.Y - first_vertex.Y) * third_vertex.X) +
                    ((first_vertex.X - second_vertex.X) * third_vertex.Y) +
                    (second_vertex.X * first_vertex.Y) -
                    (first_vertex.X * second_vertex.Y));

                // GET THE BOUNDING RECTANGLE OF THE TRIANGLE.
                /// @todo   Create rectangle class.
                float min_x = std::min({ first_vertex.X, second_vertex.X, third_vertex.X });
                float max_x = std::max({ first_vertex.X, second_vertex.X, third_vertex.X });
                float min_y = std::min({ first_vertex.Y, second_vertex.Y, third_vertex.Y });
                float max_y = std::max({ first_vertex.Y, second_vertex.Y, third_vertex.Y });

                // COLOR PIXELS WITHIN THE TRIANGLE.
                constexpr float ONE_PIXEL = 1.0f;
                for (float y = min_y; y <= max_y; y += ONE_PIXEL)
                {
                    for (float x = min_x; x <= max_x; x += ONE_PIXEL)
                    {
                        // COMPUTE THE BARYCENTRIC COORDINATES OF THE CURRENT PIXEL POSITION.
                        // The following diagram shows the order of the vertices:
                        //             first_vertex
                        //                 /\
                        //                /  \
                        // second_vertex /____\ third_vertex
                        float current_pixel_signed_distance_from_bottom_edge = (
                            ((second_vertex.Y - third_vertex.Y) * x) +
                            ((third_vertex.X - second_vertex.X) * y) +
                            (second_vertex.X * third_vertex.Y) -
                            (third_vertex.X * second_vertex.Y));
                        float scaled_signed_distance_of_current_pixel_relative_to_bottom_edge = (current_pixel_signed_distance_from_bottom_edge / top_vertex_signed_distance_from_bottom_edge);

                        float current_pixel_signed_distance_from_left_edge = (
                            ((second_vertex.Y - first_vertex.Y) * x) +
                            ((first_vertex.X - second_vertex.X) * y) +
                            (second_vertex.X * first_vertex.Y) -
                            (first_vertex.X * second_vertex.Y));
                        float scaled_signed_distance_of_current_pixel_relative_to_left_edge = (current_pixel_signed_distance_from_left_edge / right_vertex_signed_distance_from_left_edge);

                        float scaled_signed_distance_of_current_pixel_relative_to_right_edge = (
                            1.0f -
                            scaled_signed_distance_of_current_pixel_relative_to_left_edge -
                            scaled_signed_distance_of_current_pixel_relative_to_bottom_edge);

                        // CHECK IF THE PIXEL IS WITHIN THE TRIANGLE.
                        // It's allowed to be on the borders too.
                        constexpr float MIN_SIGNED_DISTANCE_TO_BE_ON_EDGE = 0.0f;
                        constexpr float MAX_SIGNED_DISTANCE_TO_BE_ON_VERTEX = 1.0f;
                        bool pixel_between_bottom_edge_and_top_vertex = (
                            (MIN_SIGNED_DISTANCE_TO_BE_ON_EDGE <= scaled_signed_distance_of_current_pixel_relative_to_bottom_edge) &&
                            (scaled_signed_distance_of_current_pixel_relative_to_bottom_edge <= MAX_SIGNED_DISTANCE_TO_BE_ON_VERTEX));
                        bool pixel_between_left_edge_and_right_vertex = (
                            (MIN_SIGNED_DISTANCE_TO_BE_ON_EDGE <= scaled_signed_distance_of_current_pixel_relative_to_left_edge) &&
                            (scaled_signed_distance_of_current_pixel_relative_to_left_edge <= MAX_SIGNED_DISTANCE_TO_BE_ON_VERTEX));
                        bool pixel_between_right_edge_and_left_vertex = (
                            (MIN_SIGNED_DISTANCE_TO_BE_ON_EDGE <= scaled_signed_distance_of_current_pixel_relative_to_right_edge) &&
                            (scaled_signed_distance_of_current_pixel_relative_to_right_edge <= MAX_SIGNED_DISTANCE_TO_BE_ON_VERTEX));
                        bool pixel_in_triangle = (
                            pixel_between_bottom_edge_and_top_vertex &&
                            pixel_between_left_edge_and_right_vertex &&
                            pixel_between_right_edge_and_left_vertex);
                        if (pixel_in_triangle)
                        {
                            // The color needs to be interpolated with this kind of shading.
                            Color interpolated_color = GRAPHICS::Color::BLACK;

                            const Color& first_vertex_color = triangle.Material->VertexFaceColors[0];
                            const Color& second_vertex_color = triangle.Material->VertexFaceColors[1];
                            const Color& third_vertex_color = triangle.Material->VertexFaceColors[2];
                            interpolated_color.Red = (
                                (scaled_signed_distance_of_current_pixel_relative_to_right_edge * third_vertex_color.Red) +
                                (scaled_signed_distance_of_current_pixel_relative_to_left_edge * second_vertex_color.Red) +
                                (scaled_signed_distance_of_current_pixel_relative_to_bottom_edge * first_vertex_color.Red));
                            interpolated_color.Green = (
                                (scaled_signed_distance_of_current_pixel_relative_to_right_edge * third_vertex_color.Green) +
                                (scaled_signed_distance_of_current_pixel_relative_to_left_edge * second_vertex_color.Green) +
                                (scaled_signed_distance_of_current_pixel_relative_to_bottom_edge * first_vertex_color.Green));
                            interpolated_color.Blue = (
                                (scaled_signed_distance_of_current_pixel_relative_to_right_edge * third_vertex_color.Blue) +
                                (scaled_signed_distance_of_current_pixel_relative_to_left_edge * second_vertex_color.Blue) +
                                (scaled_signed_distance_of_current_pixel_relative_to_bottom_edge * first_vertex_color.Blue));
                            interpolated_color.Clamp();

                            // The color needs to be shaded based on lighting.
                            for (const Light& light : lights)
                            {
                                // COMPUTE SHADING BASED ON TYPE OF LIGHT.
                                if (LightType::AMBIENT == light.Type)
                                {
                                    /// @todo   Should this be multiplicative or additive?
                                    interpolated_color = Color::ComponentMultiplyRedGreenBlue(interpolated_color, light.AmbientColor);
                                }
                            }

                            // The coordinates need to be rounded to integer in order
                            // to plot a pixel on a fixed grid.
                            render_target.WritePixel(
                                static_cast<unsigned int>(std::round(x)),
                                static_cast<unsigned int>(std::round(y)),
                                interpolated_color);
                        }
                    }
                }
                break;
            }
        }

        /// @todo   Look at reducing duplication of the above!

#if 0
        // COMPUTE THE BARYCENTRIC COORDINATES OF THE TRIANGLE VERTICES.
        float top_vertex_signed_distance_from_bottom_edge = (
            ((second_vertex.Y - third_vertex.Y) * first_vertex.X) +
            ((third_vertex.X - second_vertex.X) * first_vertex.Y) +
            (second_vertex.X * third_vertex.Y) -
            (third_vertex.X * second_vertex.Y));
        float right_vertex_signed_distance_from_left_edge = (
            ((second_vertex.Y - first_vertex.Y) * third_vertex.X) +
            ((first_vertex.X - second_vertex.X) * third_vertex.Y) +
            (second_vertex.X * first_vertex.Y) -
            (first_vertex.X * second_vertex.Y));

        // GET THE BOUNDING RECTANGLE OF THE TRIANGLE.
        /// @todo   Create rectangle class.
        float min_x = std::min({ first_vertex.X, second_vertex.X, third_vertex.X });
        float max_x = std::max({ first_vertex.X, second_vertex.X, third_vertex.X });
        float min_y = std::min({ first_vertex.Y, second_vertex.Y, third_vertex.Y });
        float max_y = std::max({ first_vertex.Y, second_vertex.Y, third_vertex.Y });

        // COLOR PIXELS WITHIN THE TRIANGLE.
        constexpr float ONE_PIXEL = 1.0f;
        for (float y = min_y; y <= max_y; y += ONE_PIXEL)
        {
            for (float x = min_x; x <= max_x; x += ONE_PIXEL)
            {
                // COMPUTE THE BARYCENTRIC COORDINATES OF THE CURRENT PIXEL POSITION.
                // The following diagram shows the order of the vertices:
                //             first_vertex
                //                 /\
                //                /  \
                // second_vertex /____\ third_vertex
                float current_pixel_signed_distance_from_bottom_edge = (
                    ((second_vertex.Y - third_vertex.Y) * x) +
                    ((third_vertex.X - second_vertex.X) * y) +
                    (second_vertex.X * third_vertex.Y) -
                    (third_vertex.X * second_vertex.Y));
                float scaled_signed_distance_of_current_pixel_relative_to_bottom_edge = (current_pixel_signed_distance_from_bottom_edge / top_vertex_signed_distance_from_bottom_edge);

                float current_pixel_signed_distance_from_left_edge = (
                    ((second_vertex.Y - first_vertex.Y) * x) +
                    ((first_vertex.X - second_vertex.X) * y) +
                    (second_vertex.X * first_vertex.Y) -
                    (first_vertex.X * second_vertex.Y));
                float scaled_signed_distance_of_current_pixel_relative_to_left_edge = (current_pixel_signed_distance_from_left_edge / right_vertex_signed_distance_from_left_edge);

                float scaled_signed_distance_of_current_pixel_relative_to_right_edge = (
                    1.0f - 
                    scaled_signed_distance_of_current_pixel_relative_to_left_edge - 
                    scaled_signed_distance_of_current_pixel_relative_to_bottom_edge);

                // CHECK IF THE PIXEL IS WITHIN THE TRIANGLE.
                // It's allowed to be on the borders too.
                constexpr float MIN_SIGNED_DISTANCE_TO_BE_ON_EDGE = 0.0f;
                constexpr float MAX_SIGNED_DISTANCE_TO_BE_ON_VERTEX = 1.0f;
                bool pixel_between_bottom_edge_and_top_vertex = (
                    (MIN_SIGNED_DISTANCE_TO_BE_ON_EDGE <= scaled_signed_distance_of_current_pixel_relative_to_bottom_edge) &&
                    (scaled_signed_distance_of_current_pixel_relative_to_bottom_edge <= MAX_SIGNED_DISTANCE_TO_BE_ON_VERTEX));
                bool pixel_between_left_edge_and_right_vertex = (
                    (MIN_SIGNED_DISTANCE_TO_BE_ON_EDGE <= scaled_signed_distance_of_current_pixel_relative_to_left_edge) &&
                    (scaled_signed_distance_of_current_pixel_relative_to_left_edge <= MAX_SIGNED_DISTANCE_TO_BE_ON_VERTEX));
                bool pixel_between_right_edge_and_left_vertex = (
                    (MIN_SIGNED_DISTANCE_TO_BE_ON_EDGE <= scaled_signed_distance_of_current_pixel_relative_to_right_edge) &&
                    (scaled_signed_distance_of_current_pixel_relative_to_right_edge <= MAX_SIGNED_DISTANCE_TO_BE_ON_VERTEX));
                bool pixel_in_triangle = (
                    pixel_between_bottom_edge_and_top_vertex &&
                    pixel_between_left_edge_and_right_vertex &&
                    pixel_between_right_edge_and_left_vertex);
                if (pixel_in_triangle)
                {
                    // The coordinates need to be rounded to integer in order
                    // to plot a pixel on a fixed grid.
                    render_target.WritePixel(
                        static_cast<unsigned int>(std::round(x)),
                        static_cast<unsigned int>(std::round(y)),
                        Color(0.0f, 0.0f, 1.0f, 1.0f));
                }
                else
                {
                    // The coordinates need to be rounded to integer in order
                    // to plot a pixel on a fixed grid.
                    /*render_target.WritePixel(
                        static_cast<unsigned int>(std::round(x)),
                        static_cast<unsigned int>(std::round(y)),
                        Color(1.0f, 0.0f, 0.0f, 1.0f));*/
                }
            }
        }
#endif
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

    /// Renders a line with the specified endpoints (in screen coordinates) and interpolated color.
    /// @param[in]  start_x - The starting x coordinate of the line.
    /// @param[in]  start_y - The starting y coordinate of the line.
    /// @param[in]  end_x - The ending x coordinate of the line.
    /// @param[in]  end_y - The ending y coordinate of the line.
    /// @param[in]  start_color - The color of the line at the starting coordinate.
    /// @param[in]  end_color - The color of the line at the ending coordinate.
    /// @param[in,out]  render_target - The target to render to.
    void Renderer::DrawLineWithInterpolatedColor(
        const float start_x,
        const float start_y,
        const float end_x,
        const float end_y,
        const Color& start_color,
        const Color& end_color,
        RenderTarget& render_target) const
    {
        // COMPUTE THE LENGTH OF THE ENTIRE LINE.
        MATH::Vector2f vector_from_start_to_end(end_x - start_x, end_y - start_y);
        float line_length = vector_from_start_to_end.Length();

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

            // CALCULATE THE COLOR AT THE CURRENT POINT.
            MATH::Vector2f vector_to_current_pixel(x - start_x, y - start_y);
            float length_to_current_pixel_from_line_start = vector_to_current_pixel.Length();
            float ratio_toward_end_of_line = (length_to_current_pixel_from_line_start / line_length);
            Color interpolated_color = Color::InterpolateRedGreenBlue(start_color, end_color, ratio_toward_end_of_line);

            // DRAW A PIXEL AT THE CURRENT POSITION.
            // The coordinates need to be rounded to integer in order
            // to plot a pixel on a fixed grid.
            render_target.WritePixel(
                static_cast<unsigned int>(std::round(x)),
                static_cast<unsigned int>(std::round(y)),
                interpolated_color);

            // MOVE ALONG THE LINE FOR THE NEXT PIXEL.
            x += x_increment;
            y += y_increment;
        }
    }
}
