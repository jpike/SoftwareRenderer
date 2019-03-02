#include <cmath>
#include "Graphics/RayTracing/Camera.h"

namespace GRAPHICS
{
namespace RAY_TRACING
{
    /// Creates a camera looking at the specified world position.
    /// It is assumed that this look at position doesn't match the camera's position.
    /// If that isn't true, this may not work.
    /// @param[in]  look_at_world_position - The world position the camera should look at.
    /// @return A camera looking at the specified position.
    Camera Camera::LookAt(const MATH::Vector3f& look_at_world_position)
    {
        // CREATE A DEFAULT CAMERA.
        Camera camera;

        // COMPUTE THE VIEWING DIRECTION FOR THE CAMERA.
        MATH::Vector3f camera_view_direction = look_at_world_position - camera.WorldPosition;
        MATH::Vector3f normalized_camera_view_direction = MATH::Vector3f::Normalize(camera_view_direction);

        // COMPUTE THE COORDINATE FRAME FOR THE CAMERA.
        MATH::Vector3f negative_camera_view_direction = MATH::Vector3f::Scale(-1.0f, normalized_camera_view_direction);
        camera.CoordinateFrame = MATH::CoordinateFrame::FromUpAndForward(camera.CoordinateFrame.Up, negative_camera_view_direction);

        return camera;
    }

    /// Computes a viewing ray coming from this camera for the specified pixel coordinates.
    /// @param[in]  pixel_coordinates - The coordinates of the render target
    ///     for the pixel through which to compute the viewing ray. 
    /// @param[in]  render_target - The render target for which the viewing ray is to
    ///     be computed.
    /// @return The viewing ray from the camera through the specified coordinates;
    ///     the exact ray will vary depending on the type of projection this camera
    ///     is using.
    Ray Camera::ViewingRay(
        const MATH::Vector2ui& pixel_coordinates,
        const RenderTarget& render_target) const
    {
        // CONVERT THE PIXEL COORDINATES TO THE RANGE OF THE VIEWING PLANE.
        // In order to convert the current pixel coordinate to proper coordinates for the viewing ray,
        // several transformations are needed to convert from a [0, pixel dimension] range to
        // a range for the viewing plane:
        // 1. Each pixel may be thought of as a box.  For most consistent rendering,
        //      the ray should go through the center of each pixel.
        constexpr float OFFSET_TO_CENTER_OF_PIXEL = 0.5f;
        float x_pixel_center = pixel_coordinates.X + OFFSET_TO_CENTER_OF_PIXEL;
        // 2. Shift the coordinates down so that the minimum coordinates are negative.
        //      By doing this by the half-width of the render target, this means the
        //      new center will correspond with the center of the render target.
        unsigned int render_target_width_in_pixels = render_target.GetWidthInPixels();
        float render_target_half_width_in_pixels = render_target_width_in_pixels / 2.0f;
        float x_shifted_down = (x_pixel_center - render_target_half_width_in_pixels);
        // 3. Scale the coordinates to be in the range of the viewing plane
        //      instead of the pixel range of the render target.
        float x_scaled_to_viewing_plane_range = x_shifted_down * ViewingPlane.Width / render_target_width_in_pixels;
        
        // The same conversion must happen for y, but since the render target pixel coordinates
        // have y increasing going down but the viewing plane has y increasing going up,
        // the y coordinate must be flipped.
        unsigned int render_target_height_in_pixels = render_target.GetHeightInPixels();
        float render_target_half_height_in_pixels = render_target_height_in_pixels / 2.0f;
        float y_pixel_center = pixel_coordinates.Y + OFFSET_TO_CENTER_OF_PIXEL;
        float y_shifted_down = (y_pixel_center - render_target_half_height_in_pixels);
        float y_scaled_to_viewing_plane_range = y_shifted_down * ViewingPlane.Height / render_target_height_in_pixels;
        constexpr float FLIP_Y = -1.0f;
        y_scaled_to_viewing_plane_range *= FLIP_Y;

        // COMPUTE A VIEWING RAY ACCORDING TO THE TYPE OF PROJECTION.
        bool using_perspective_projection = (ProjectionType::PERSPECTIVE == Projection);
        if (using_perspective_projection)
        {
            // ADDING PERSPECTIVE SCALING.
            // The coordinates should be scaled based on the ratio of the camera's field of view and the
            // distance of the camera from the viewing plane (the tangent represents the ratio
            // between the horizontal (x) and vertical (y) dimensions with the depth (z) dimension).
            // This helps provide perspective scaling based on distance.
            MATH::Angle<float>::Radians camera_field_of_view_in_radians = MATH::Angle<float>::DegreesToRadians(FieldOfView);
            float half_field_of_view_in_radians = camera_field_of_view_in_radians.Value / 2.0f;
            float ratio_between_camera_view_dimensions_and_distance_from_viewing_plane = std::tan(half_field_of_view_in_radians);
            float x_scaled_based_on_camera_distance = x_scaled_to_viewing_plane_range * ratio_between_camera_view_dimensions_and_distance_from_viewing_plane;
            float y_scaled_based_on_camera_distance = y_scaled_to_viewing_plane_range * ratio_between_camera_view_dimensions_and_distance_from_viewing_plane;

            // RETURN A RAY GOING THROUGH THE SPECIFIED PIXEL.
            // Since we're using a right-handed coordinate system and the camera's focal length needs to be negated.
            MATH::Vector3f view_direction = MATH::Vector3f::Scale(-ViewingPlane.FocalLength, CoordinateFrame.Forward);
            // The view direction needs to be offset based on the perspective scaling.
            MATH::Vector3f horizontal_direction = MATH::Vector3f::Scale(x_scaled_based_on_camera_distance, CoordinateFrame.Right);
            view_direction += horizontal_direction;
            MATH::Vector3f vertical_direction = MATH::Vector3f::Scale(y_scaled_based_on_camera_distance, CoordinateFrame.Up);
            view_direction += vertical_direction;
            MATH::Vector3f normalized_view_direction = MATH::Vector3f::Normalize(view_direction);
            Ray perspective_ray(WorldPosition, normalized_view_direction);
            return perspective_ray;
        }
        else
        {
            // COMPUTE THE RAY ORIGIN.
            // An orthographic projection is assumed by default.
            // The ray should start on the viewing plane in the appropriate direction in front of the camera.
            MATH::Vector3f ray_origin = WorldPosition;
            MATH::Vector3f normalized_camera_view_direction = MATH::Vector3f::Scale(-1.0f, CoordinateFrame.Forward);
            MATH::Vector3f displacement_from_camera_position_to_viewing_plane = MATH::Vector3f::Scale(
                ViewingPlane.FocalLength, 
                normalized_camera_view_direction);
            ray_origin += displacement_from_camera_position_to_viewing_plane;

            // The ray needs to be horizontally displaced so that it shoots straight out from the specified pixel.
            MATH::Vector3f horizontal_pixel_displacement = MATH::Vector3f::Scale(x_scaled_to_viewing_plane_range, CoordinateFrame.Right);
            ray_origin += horizontal_pixel_displacement;

            // The ray needs to be vertically displaced so that it shoots straight out from the specified pixel.
            MATH::Vector3f vertical_pixel_displacement = MATH::Vector3f::Scale(y_scaled_to_viewing_plane_range, CoordinateFrame.Up);
            ray_origin += vertical_pixel_displacement;

            // RETURN A RAY GOING THROUGH THE SPECIFIED PIXEL.
            // It should go from the specified pixel on the viewing plane directly in the direction the camera is facing.
            MATH::Vector3f ray_direction = normalized_camera_view_direction;
            Ray orthographic_ray(ray_origin, ray_direction);
            return orthographic_ray;
        }
    }
}
}
