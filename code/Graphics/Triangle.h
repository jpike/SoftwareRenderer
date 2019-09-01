#pragma once

#include <array>
#include <cstddef>
#include "Graphics/Material.h"
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
        static Triangle CreateEquilateral(const GRAPHICS::Material* const material);

        // PUBLIC MEMBER VARIABLES FOR EASY ACCESS.
        /// The material of the triangle.  Stored externally for re-use
        /// and cheap copying across many triangles.
        /// @todo   Custom ExternalPointer<> or std::observer_ptr<>?
        const GRAPHICS::Material* Material = nullptr;
        /// The vertices of the triangle.
        /// Should be in counter-clockwise order.
        std::array<MATH::Vector3f, VERTEX_COUNT> Vertices = {};
    };
}
