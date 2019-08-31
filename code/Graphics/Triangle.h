#pragma once

#include <array>
#include <cstddef>
#include "Graphics/Color.h"
#include "Math/Vector3.h"

namespace GRAPHICS
{
    /// A triangle that can be rendered.
    class Triangle
    {
    public:
        // STATIC CONSTANTS.
        /// The number of vertices in a triangle.
        static constexpr std::size_t VERTEX_COUNT = 3;

        // CONSTRUCTION.
        static Triangle CreateEquilateral(const Color& color);

        // PUBLIC MEMBER VARIABLES FOR EASY ACCESS.
        /// The color of the triangle.
        GRAPHICS::Color Color = Color::BLACK;
        /// The vertices of the triangle.
        /// Should be in counter-clockwise order.
        std::array<MATH::Vector3f, VERTEX_COUNT> Vertices = {};
    };
}
