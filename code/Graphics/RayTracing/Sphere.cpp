#include <cmath>
#include "Graphics/RayTracing/Sphere.h"

namespace GRAPHICS
{
namespace RAY_TRACING
{
    /// Computes the surface normal of the sphere at given point.
    /// @param[in]  surface_point - The point on the sphere's surface at which to compute a normal.
    /// @return The unit surface normal at the specified point.
    MATH::Vector3f Sphere::SurfaceNormal(const MATH::Vector3f& surface_point) const
    {
        MATH::Vector3f surface_normal;
        surface_normal.X = surface_point.X - CenterPosition.X;
        surface_normal.Y = surface_point.Y - CenterPosition.Y;
        surface_normal.Z = surface_point.Z - CenterPosition.Z;

        MATH::Vector3f normalized_surface_normal = MATH::Vector3f::Normalize(surface_normal);
        return normalized_surface_normal;
    }

    /// Gets the material defining surface properties of the object.
    /// @return The material for the object.
    Material Sphere::GetMaterial() const
    {
        return Material;
    }

    /// Checks for an intersection between a ray and the object.
    /// @param[in]  ray - The ray to check for intersection.
    /// @return A ray-object intersection, if one occurred; std::nullopt otherwise.
    std::optional<RayObjectIntersection> Sphere::Intersect(const Ray& ray) const
    {
        // Prep for quadratic formula.
        float a = MATH::Vector3f::DotProduct(ray.Direction, ray.Direction);
        float b = 2.0f * (
                ray.Direction.X * (ray.Origin.X - CenterPosition.X) +
                ray.Direction.Y * (ray.Origin.Y - CenterPosition.Y) +
                ray.Direction.Z * (ray.Origin.Z - CenterPosition.Z)
            );
        float c = (
            -2.0f * MATH::Vector3f::DotProduct(ray.Origin, CenterPosition) + 
            MATH::Vector3f::DotProduct(ray.Origin, ray.Origin) +
            MATH::Vector3f::DotProduct(CenterPosition, CenterPosition) -
            (Radius * Radius));
        float discriminant = (b * b) - (4 * a * c);
        if (discriminant < 0.0f)
        {
            return std::nullopt;
        }

        float first_intersection_distance = ((-1.0f * b) + std::sqrtf(discriminant)) / (2.0f * a);
        float second_intersection_distance = ((-1.0f * b) - std::sqrtf(discriminant)) / (2.0f * a);

        bool intersects_behind_view = (first_intersection_distance < 0.0f) && (second_intersection_distance < 0.0f);
        if (intersects_behind_view)
        {
            return std::nullopt;
        }

        bool only_first_intersection_is_in_front = (first_intersection_distance >= 0.0f) && (second_intersection_distance < 0.0f);
        if (only_first_intersection_is_in_front)
        {
            RayObjectIntersection intersection;
            intersection.Ray = &ray;
            intersection.DistanceFromRayToObject = first_intersection_distance;
            intersection.Object = this;
            return intersection;
        }

        bool only_second_intersection_is_in_front = (second_intersection_distance >= 0.0f) && (first_intersection_distance < 0.0f);
        if (only_second_intersection_is_in_front)
        {
            RayObjectIntersection intersection;
            intersection.Ray = &ray;
            intersection.DistanceFromRayToObject = second_intersection_distance;
            intersection.Object = this;
            return intersection;
        }

        bool both_intersections_in_front = (first_intersection_distance >= 0.0f) && (second_intersection_distance >= 0.0f);
        if (both_intersections_in_front)
        {
            RayObjectIntersection intersection;
            intersection.Ray = &ray;
            intersection.Object = this;

            // CHOOSE THE EARLIEST INTERSECTION.
            bool first_intersection_is_earliest = (first_intersection_distance < second_intersection_distance);
            if (first_intersection_is_earliest)
            {
                intersection.DistanceFromRayToObject = first_intersection_distance;
            }
            else
            {
                intersection.DistanceFromRayToObject = second_intersection_distance;
            }

            return intersection;
        }

        return std::nullopt;
    }
}
}
