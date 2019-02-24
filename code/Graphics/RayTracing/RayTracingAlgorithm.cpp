#include <cmath>
#include "Graphics/RayTracing/RayTracingAlgorithm.h"
#include "Math/Angle.h"

namespace GRAPHICS
{
namespace RAY_TRACING
{
    /// Renders a scene to the specified render target.
    /// @param[in]  scene - The scene to render.
    /// @param[in,out]  render_target - The target to render to.
    void RayTracingAlgorithm::Render(const Scene& scene, GRAPHICS::RenderTarget& render_target)
    {
        // RENDER EACH ROW OF PIXELS.
        unsigned int render_target_height_in_pixels = render_target.GetHeightInPixels();
        for (unsigned int y = 0; y < render_target_height_in_pixels; ++y)
        {
            // RENDER EACH COLUMN IN THE CURRENT ROW.
            unsigned int render_target_width_in_pixels = render_target.GetWidthInPixels();
            for (unsigned int x = 0; x < render_target_width_in_pixels; ++x)
            {
                // CREATE THE VIEWING RAY.
                // It should go from the camera's position through the viewing plane toward the current pixel.
                // The viewing plane goes from (-1, -1) in the bottom-left to (1, 1) in the top-right and is centered on (0, 0).
                MATH::Angle<float>::Radians camera_field_of_view_in_radians = MATH::Angle<float>::DegreesToRadians(Camera.FieldOfView);

                // In order to convert the current pixel coordinate to proper coordinates for the viewing ray,
                // several transformations are needed to convert from a [0, pixel dimension] range to
                // a [-1, 1] range for the viewing plane:
                // 1. Shift the coordinates down so that the minimum coordinates are negative.
                //      By doing this by the half-width of the render target, this means the
                //      new center will correspond with the center of the render target.
                float render_target_half_width_in_pixels = render_target_width_in_pixels / 2.0f;
                float x_shifted_down = (x - render_target_half_width_in_pixels);
                // 2. Scale the coordinates to be in the [-1, 1] range of the viewing plane
                //      instead of the pixel range of the render target.
                constexpr float VIEWING_PLANE_DIMENSION = 2.0f;
                float x_scaled_to_viewing_plane_range = x_shifted_down * VIEWING_PLANE_DIMENSION / render_target_width_in_pixels;
                // 3. Scale the coordinates based on the ratio of the camera's field of view and the
                //      distance of the camera from the viewing plane (the tangent represents the ratio
                //      between the horizontal (x) and vertical (y) dimensions with the depth (z) dimension).
                //      This helps provide perspective scaling based on distance.
                float half_field_of_view_in_radians = camera_field_of_view_in_radians.Value / 2.0f;
                float ratio_between_camera_view_dimensions_and_distance_from_viewing_plane = std::tan(half_field_of_view_in_radians);
                float view_x_direction = x_scaled_to_viewing_plane_range * ratio_between_camera_view_dimensions_and_distance_from_viewing_plane;

                // The same conversion must happen for y, but since the render target pixel coordinates
                // have y increasing going down but the viewing plane has y increasing going up,
                // the y coordinate must be flipped.
                float render_target_half_height_in_pixels = render_target_height_in_pixels / 2.0f;
                float y_shifted_down = (y - render_target_half_height_in_pixels);
                float y_scaled_to_viewing_plane_range = y_shifted_down * VIEWING_PLANE_DIMENSION / render_target_height_in_pixels;
                float y_scaled_based_on_camera_distance = y_scaled_to_viewing_plane_range * ratio_between_camera_view_dimensions_and_distance_from_viewing_plane;
                constexpr float FLIP_Y = -1.0f;
                float view_y_direction = y_scaled_based_on_camera_distance * FLIP_Y;
                
                // The viewing plane is arbitrarily placed at z = -1.
                constexpr float VIEWING_PLANE_Z = -1.0f;
                MATH::Vector3f view_direction(view_x_direction, view_y_direction, VIEWING_PLANE_Z);

                Ray ray(Camera.WorldPosition, view_direction);

                // FIND THE CLOSEST OBJECT IN THE SCENE THAT THE RAY INTERSECTS.
                std::optional<RayObjectIntersection> closest_intersection = std::nullopt;
                for (const auto& current_object : scene.Objects)
                {
                    // CHECK IF THE RAY INTERSECTS THE CURRENT OBJECT.
                    std::optional<RayObjectIntersection> intersection = current_object->Intersect(ray);
                    bool ray_hit_object = (std::nullopt != intersection);
                    if (!ray_hit_object)
                    {
                        // CONTINUE SEEING IF OTHER OBJECTS ARE HIT.
                        continue;
                    }

                    // UPDATE THE CLOSEST INTERSECTION APPROPRIATELY.
                    if (closest_intersection)
                    {
                        // ONLY OVERWRITE THE CLOSEST INTERSECTION IF THE NEWEST ONE IS CLOSER.
                        bool new_intersection_closer = (intersection->DistanceFromRayToObject < closest_intersection->DistanceFromRayToObject);
                        if (new_intersection_closer)
                        {
                            closest_intersection = intersection;
                        }
                    }
                    else
                    {
                        // SET THIS FIRST INTERSECTION AS THE CLOSEST.
                        closest_intersection = intersection;
                    }
                }

                // COLOR THE CURRENT PIXEL.
                if (closest_intersection)
                {
                    // COMPUTE THE CURRENT PIXEL'S COLOR.
                    Material material = closest_intersection->Object->GetMaterial();
                    render_target.WritePixel(x, y, material.DiffuseColor);
                }
                else
                {
                    // FILL THE PIXEL WITH THE BACKGROUND COLOR.
                    render_target.WritePixel(x, y, scene.BackgroundColor);
                }
            }
        }
    }
}
}
