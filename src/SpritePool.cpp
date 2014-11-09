#include <SpritePool.h>

namespace Acidrain {

    void SpritePool::begin() {
        for (auto& kv : vbos)
            kv.second.empty();
    }

    void SpritePool::addTexturedQuad(const Texture* texture, const vector<vec2>& vertices, const vector<vec2>& texCoords, const vec4& color) {
        vbos[texture].addQuad(vertices, texCoords, color);
    }

    void SpritePool::end() {
        for (auto& kv : vbos) {
            glEnable(GL_BLEND);
            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

            glEnable(GL_TEXTURE_2D);
            kv.first->use();

            kv.second.draw();
        }
    }

} // namespace acidrain