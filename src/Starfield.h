#pragma once

#include <Texture.h>
#include <memory>
#include <glm/vec2.hpp>
#include <vector>
#include <Randomizer.h>

namespace Acidrain {

    struct StarParticle {
        glm::vec2   position;
        glm::vec2   direction;
        float       speed;
        bool        firstSpawn = true;
    };

    class Starfield {
    public:

        Starfield(int howMany, glm::vec2 terrainSize);
        virtual ~Starfield();

        void update(float dt);
        void render();

    private:

        void spawn(const std::shared_ptr<StarParticle>& particle);
        bool isOutOfTerrain(const std::shared_ptr<StarParticle>& particle);
        void draw(const std::shared_ptr<StarParticle>& particle);

        int spriteWidth;
        int spriteHeight;

        Randomizer rnd;
        glm::vec2 terrainSize;
        std::shared_ptr<Texture> flareTexture;
        std::vector<std::shared_ptr<StarParticle>> particles;
    };

} // namespace Acidrain