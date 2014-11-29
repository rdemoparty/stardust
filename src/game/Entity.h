#pragma once

#define GLM_FORCE_RADIANS

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include <Sprite.h>

namespace Acidrain {

    using namespace glm;
    using namespace std;

    class Entity {
    public:
        vec2 size = vec2(1);

        vec2 position = vec2(0);
        vec2 scale = vec2(1);
        float rotation = 0;

        mat4 localTransform;
        mat4 worldTransform;

        Entity* parent;

        virtual void update(float dt);
    };

    class SpritePool;

    class DrawableEntity : public Entity {
    public:

        void addTo(SpritePool& spritePool) const;

        vector<vec2> computeVertices() const;

        virtual void update(float dt) override;

//    private:
        vec4 color = vec4(1);
        Sprite currentSprite;
    };

} // namespace Acidrain