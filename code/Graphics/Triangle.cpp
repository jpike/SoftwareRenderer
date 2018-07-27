#include "Graphics/Triangle.h"

namespace GRAPHICS
{
    /// Creates an equilateral triangle with the specified color.
    /// @param[in]  color - The color of the triangle.
    /// @return An equilateral triangle of the specified color.
    Triangle Triangle::CreateEquilateral(const GRAPHICS::Color& color)
    {
        // DEFINE THE OBJECT-SPACE POSITIONS OF THE TRIANGLE VERTICES.
        // The Pythagorean theorem is used to compute the coordinates of the vertices:
        //      /|\
        //     /_|_\
        // height^2 + half_width^2 = side_length^2
        // The efficiency of these calculations has not been measured.
        float side_length = 1.0f;
        float side_length_squared = side_length * side_length;
        float half_width = side_length / 2.0f;
        float half_width_squared = half_width * half_width;
        float height_squared = side_length_squared - half_width_squared;
        float height = sqrt(height_squared);
        float half_height = height / 2.0f;
        float top_x = 0.0f;
        float top_y = half_height;
        float bottom_y = -half_height;
        float left_x = -half_width;
        float right_x = half_width;
        float z = 0.0f;

        // SET THE VERTICES IN A TRIANGLE.
        Triangle triangle;
        triangle.Color = color;
        triangle.Vertices =
        {
            MATH::Vector3f(top_x, top_y, z),
            MATH::Vector3f(right_x, bottom_y, z),
            MATH::Vector3f(left_x, bottom_y, z)
        };
        return triangle;
    }
}
