#pragma once

#include <vector>
#include "Graphics/Color.h"

namespace GRAPHICS
{
    /// The different kinds of shading that are supported.
    enum class ShadingType
    {
        /// Objects are rendered as wireframes with a single color.
        /// Defaults to wireframe to ensure at least something gets rendered.
        WIREFRAME = 0,
        /// Objects are rendered as wireframes with colors interpolated across vertices.
        WIREFRAME_VERTEX_COLOR_INTERPOLATION,
        /// Objects are rendered with a solid, flat color.
        FLAT,
        /// Objects are rendered with colors interpolated across vertices across the entire face.
        FACE_VERTEX_COLOR_INTERPOLATION,
        /// An extra enum to indicate the number of different shading types.
        COUNT
    };

    /// A material defining properties of a surface and how it's shaded.
    class Material
    {
    public:
        /// The type of shading for the material.
        ShadingType Shading = ShadingType::WIREFRAME;

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
