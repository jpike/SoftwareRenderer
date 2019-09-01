#pragma once

#include <vector>
#include "Graphics/Color.h"

namespace GRAPHICS
{
    /// The different kinds of shading that are supported.
    enum class ShadingType
    {
        INVALID = 0,
        WIREFRAME,
        WIREFRAME_VERTEX_COLOR_INTERPOLATION,
        FLAT,
        FACE_VERTEX_COLOR_INTERPOLATION,
    };

    /// A material defining properties of a surface and how it's shaded.
    class Material
    {
    public:
        /// The type of shading for the material.
        ShadingType Shading = ShadingType::INVALID;

        /// The color of an edge, if wireframe shading is used.
        GRAPHICS::Color WireframeColor = GRAPHICS::Color::BLACK;

        /// The colors of each of the vertices for use in wireframe shading.
        /// Expected to be in counter-clockwise vertex order, and the number of vertex colors
        /// can vary to handle different kinds of polygonal shapes.
        std::vector<GRAPHICS::Color> VertexWireframeColors = {};

        /// The color of a face, if solid-color face (flat) shading is used.
        GRAPHICS::Color FaceColor = GRAPHICS::Color::BLACK;

        /// The colors of each of the vertices for use in interpolated face shading.
        /// Expected to be in counter-clockwise vertex order, and the number of vertex colors
        /// can vary to handle different kinds of polygonal shapes.
        std::vector<GRAPHICS::Color> VertexFaceColors = {};
    };
}
