#pragma once

#include <Texture.h>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <Vbo.h>
#include "GpuProgram.h"
#include <memory>
#include <map>

namespace Acidrain {

    using namespace std;

    class SpritePool {
    public:
        void clear();

        void addTexturedQuad(const Texture* texture, const vector<vec2>& vertices, const vector<vec2>& texCoords, const vec4& color);

        void draw(std::shared_ptr<GpuProgram> shader);

    private:
        map<const Texture*, Vbo> vbos;
    };

} // namespace Acidrain