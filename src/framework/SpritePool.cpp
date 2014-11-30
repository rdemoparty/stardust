#include <SpritePool.h>

#define GLM_FORCE_RADIANS
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GfxSystem.h>

namespace Acidrain {

    using namespace glm;

    void SpritePool::clear() {
        for (auto& kv : vbos)
            kv.second.empty();
    }

    void SpritePool::addTexturedQuad(const Texture* texture, const vector<vec2>& vertices, const vector<vec2>& texCoords, const vec4& color) {
        vbos[texture].addQuad(vertices, texCoords, color);
    }

    void SpritePool::draw(std::shared_ptr<GpuProgram> shader) {
        GFXSYS.setTransparencyMode(TransparencyMode::Transparent);
        shader->use();

        for (auto& kv : vbos) {
            kv.first->useForUnit(0);
            kv.second.draw();
        }
        shader->unuse();
    }

} // namespace acidrain