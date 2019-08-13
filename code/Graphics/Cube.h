#pragma once

#include <vector>
#include "Graphics/Color.h"
#include "Graphics/Object3D.h"

namespace GRAPHICS
{
    /// A cube that can be rendered.
    class Cube
    {
    public:
        // CONSTRUCTION.
        static Object3D Create(const Color& color);
    };
}
