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
                // COMPUTE THE VIEWING RAY.
                MATH::Vector2ui pixel_coordinates(x, y);
                Ray ray = Camera.ViewingRay(pixel_coordinates, render_target);

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
                    Color color = ComputeColor(*closest_intersection);
                    render_target.WritePixel(x, y, color);
                }
                else
                {
                    // FILL THE PIXEL WITH THE BACKGROUND COLOR.
                    render_target.WritePixel(x, y, scene.BackgroundColor);
                }
            }
        }
    }

    /// Computes color based on the specified intersection.
    /// @param[in]  intersection - The intersection for which to compute the color.
    /// @return The computed color.
    GRAPHICS::Color RayTracingAlgorithm::ComputeColor(const RayObjectIntersection& intersection) const
    {
        return intersection.Object->GetMaterial().DiffuseColor;
    }
}
}
