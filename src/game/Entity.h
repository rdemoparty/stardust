#pragma once

#define GLM_FORCE_RADIANS

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include <Sprite.h>
#include <Collisions.h>

namespace Acidrain {

    using namespace glm;
    using namespace std;

    struct PhysicalState {
        vec2 size = vec2(1);
        vec2 position = vec2(0);
        vec2 scale = vec2(1);
        float rotation = 0;
        vec4 color = vec4(1);

        const mat4 getXformMatrix() const;
    };

    PhysicalState interpolate(const PhysicalState& a, const PhysicalState& b, float alpha);

    class SpritePool;

    class DrawableEntity {
    public:

        void addTo(SpritePool& spritePool) const;

        vector<vec2> computeVertices() const;

        void setRenderStateAt(float alpha);

        PhysicalState previousState;
        PhysicalState currentState;
        PhysicalState renderState;

        CollisionHull collisionHull;

        Sprite currentSprite;
    };

} // namespace Acidrain