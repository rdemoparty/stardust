#pragma once

#include <Texture.h>
#include <memory>
#include <glm/vec2.hpp>
#include <vector>
#include <Randomizer.h>
#include <Vbo.h>
#include <Box.h>

namespace Acidrain {

    using namespace std;
    using namespace glm;

    class GpuProgram;

    const int FLARE_SIZE = 32;

    struct StarParticle {
        vec2 position;
        vec2 direction;
        vec4 color;

        Box box = Box(vec2(FLARE_SIZE));

        float speed;
        bool firstSpawn = true;
    };

    class Starfield {
    public:

        Starfield(int howMany, vec2 terrainSize);

        virtual ~Starfield();

        void update(float dt);

        void draw(shared_ptr<GpuProgram> shader);

    private:

        void spawn(const shared_ptr<StarParticle>& particle);

        bool isOutOfTerrain(const shared_ptr<StarParticle>& particle);

        int spriteWidth;
        int spriteHeight;

        Box textCoords;
        Vbo vbo;
        Randomizer rnd;
        vec2 terrainSize;
        shared_ptr<Texture> flareTexture;
        vector<shared_ptr<StarParticle>> particles;
    };

} // namespace Acidrain