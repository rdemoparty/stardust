#include <SpritePool.h>

#define GLM_FORCE_RADIANS
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

        glActiveTexture(GL_TEXTURE0 + 0);
        shader->setIntUniform(0, "texture");

        for (auto& kv : vbos) {
            glEnable(GL_BLEND);
            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

            glEnable(GL_TEXTURE_2D);
            kv.first->use();

            kv.second.draw();
        }
        shader->unuse();
    }

} // namespace acidrain