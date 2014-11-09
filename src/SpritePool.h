#pragma once

#include <Texture.h>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <Vbo.h>
#include <map>

namespace Acidrain {

    using namespace std;

    class SpritePool {
    public:
        void begin();

        void addTexturedQuad(const Texture* texture, const vector<vec2>& vertices, const vector<vec2>& texCoords, const vec4& color);

        void end();

    private:
        map<const Texture*, Vbo> vbos;
    };

} // namespace Acidrain