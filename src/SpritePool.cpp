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

    void SpritePool::draw(std::shared_ptr<Shader> shader) {
        shader->use();

        mat4 orthoMatrix = glm::ortho(0.0f, 1024.0f, 0.0f, 768.0f, 0.0f, 1.0f);
        shader->setMatrix4Uniform(&orthoMatrix[0][0], "orthoMatrix");

        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0 + 0);
        shader->setIntUniform(0, "texture");

        GFXSYS.setTransparencyMode(TransparencyMode::Transparent);

        for (auto& kv : vbos) {
            kv.first->use();
            kv.second.draw();
        }
        shader->unuse();
        glDisable(GL_TEXTURE_2D);
    }

} // namespace acidrain