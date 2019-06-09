#include <algorithm>
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
                    Color color = ComputeColor(scene, *closest_intersection);
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

    /// Computes color based on the specified intersection in the scene.
    /// @param[in]  scene - The scene in which the color is being computed.
    /// @param[in]  intersection - The intersection for which to compute the color.
    /// @return The computed color.
    GRAPHICS::Color RayTracingAlgorithm::ComputeColor(
        const Scene& scene, 
        const RayObjectIntersection& intersection) const
    {
        // INITIALIZE THE COLOR TO HAVE NO CONTRIBUTION FROM ANY SOURCES.
        Color final_color = Color::BLACK;

        // ADD IN THE AMBIENT COLOR.
        const Material& intersected_material = intersection.Object->GetMaterial();
        final_color += intersected_material.AmbientColor;

        // ADD IN DIFFUSE COLOR FROM LIGHTS.
        Color light_total_color = Color::BLACK;
        MATH::Vector3f intersection_point = intersection.IntersectionPoint();
        MATH::Vector3f unit_surface_normal = intersection.Object->SurfaceNormal(intersection_point);
        for (const PointLight& light : scene.PointLights)
        {
            // ADD COLOR FROM THE CURRENT LIGHT.
            // This is based on the Lambertian shading model.
            // An object is maximally illuminated when facing toward the light.
            // An object tangent to the light direction or facing away receives no illumination.
            // In-between, the amount of illumination is proportional to the cosine of the angle between
            // the light and surface normal (where the cosine can be computed via the dot product).
            MATH::Vector3f unit_direction_from_point_to_light = light.DirectionFrom(intersection_point);
            constexpr float NO_ILLUMINATION = 0.0f;
            float illumination_proportion = MATH::Vector3f::DotProduct(unit_surface_normal, unit_direction_from_point_to_light);
            illumination_proportion = std::max(NO_ILLUMINATION, illumination_proportion);

            // ADD THE CURRENT LIGHT'S COLOR.
            Color current_light_color = Color::ScaleRedGreenBlue(light.Color, illumination_proportion);
            light_total_color += current_light_color;
        }

        // The diffuse color is multiplied component-wise by the amount of light.
        Color diffuse_color = Color::ComponentMultiplyRedGreenBlue(
            intersected_material.DiffuseColor,
            light_total_color);
        final_color += diffuse_color;

        return final_color;
    }
}
}
