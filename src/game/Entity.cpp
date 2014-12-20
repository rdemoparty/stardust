#include <Entity.h>
#include <glm/gtc/matrix_transform.hpp>
#include <SpritePool.h>
//#include <iostream>
// #include <glm/ext.hpp>

namespace Acidrain {

    using namespace glm;

    PhysicalState interpolate(const PhysicalState& a, const PhysicalState& b, float alpha) {
        PhysicalState result;
        result.size = mix(a.size, b.size, alpha);
        result.position = mix(a.position, b.position, alpha);
        result.scale = mix(a.scale, b.scale, alpha);
        result.rotation = mix(a.rotation, b.rotation, alpha);
        result.color = mix(a.color, b.color, alpha);
        return result;
    }

    const mat4 PhysicalState::getXformMatrix() const {
        mat4 matrix = mat4(1);
        matrix = glm::translate(matrix, vec3(position.x, position.y, 0));
        matrix = glm::rotate(matrix, rotation, vec3(0, 0, 1));

        // Notice the weird scaling: vec3(x, y, x)
        // We use the third dimension component to pass the pristine scale value
        // to the collision area (see Collisions.h). This helps us to extract the
        // scale value from the transform matrix and use it to scale the collision
        // hull circles properly
        matrix = glm::scale(matrix, vec3(scale.x, scale.y, scale.x));
        return matrix;
    }

    void DrawableEntity::setRenderStateAt(float alpha) {
        renderState = interpolate(previousState, currentState, alpha);
    }

    vector<vec2> DrawableEntity::computeVertices() const {
        Box box(renderState.size);
        vector<vec2> vertices = box.computeVertices();
        for (auto& v : vertices) {
            vec4 v4 = vec4(v.x, v.y, 0, 1);
            v4 = renderState.getXformMatrix() * v4;
            v.x = v4.x;
            v.y = v4.y;
        }
        return vertices;
    }

    void DrawableEntity::addTo(SpritePool& spritePool) const {
        spritePool.addTexturedQuad(
                currentSprite.getTexture(),
                computeVertices(),
                currentSprite.getTexCoords(),
                renderState.color
        );
    }

} // namespace Acidrain