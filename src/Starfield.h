#pragma once

#include <Texture.h>
#include <memory>
#include <glm/vec2.hpp>
#include <vector>
#include <Randomizer.h>
#include <Vbo.h>
#include <Box.h>

namespace Acidrain {

    const int FLARE_SIZE = 48;

    struct StarParticle {
        glm::vec2 position;
        glm::vec2 direction;

        Box box = Box(vec2(FLARE_SIZE));

        float speed;
        bool firstSpawn = true;
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

        Box textCoords;
        Vbo vbo;
        Randomizer rnd;
        glm::vec2 terrainSize;
        std::shared_ptr<Texture> flareTexture;
        std::vector<std::shared_ptr<StarParticle>> particles;
    };

} // namespace Acidrain