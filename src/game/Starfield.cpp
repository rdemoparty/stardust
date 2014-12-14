#include <Starfield.h>
#include <TextureGenerator.h>
#include <iostream>
#include <GfxSystem.h>
#include <GpuProgram.h>

namespace Acidrain {

    Starfield::Starfield(int howMany, vec2 size) {
        terrainSize = size;

        spriteWidth = FLARE_SIZE;
        spriteHeight = FLARE_SIZE;

        textCoords.centerAround(vec2(0.5, 0.5));

        flareTexture = TextureGenerator(spriteWidth, spriteHeight).lens(0, spriteWidth / 4).getTexture(0);

        for (int i = 0; i < howMany; i++) {
            auto particle = make_shared<StarParticle>();
            particles.push_back(particle);
            spawn(particle);
        }
    }

    Starfield::~Starfield() {
    }

    void Starfield::spawn(const std::shared_ptr<StarParticle>& particle) {
        particle->speed = (float) (rnd.randomUnitDouble() * 3.0 + 1.0);
        particle->direction = vec2(0.0f, 1.0f) * particle->speed;

        if (particle->firstSpawn) {
            particle->position = glm::vec2(
                    rnd.randomUnitDouble() * terrainSize.x,
                    rnd.randomUnitDouble() * terrainSize.y
            );
            particle->firstSpawn = false;
        } else {
            particle->position = glm::vec2(
                    rnd.randomUnitDouble() * terrainSize.x,
                    -spriteHeight / 2.f
            );
        }
        particle->color = vec4(
                1,
                1,
                1,
                particle->speed * 0.25f
        );
    }

    void Starfield::update(float dt) {
        for (auto& particle : particles) {
            particle->position += particle->direction * dt * 50.0f;

            if (isOutOfTerrain(particle))
                spawn(particle);
        }
    }

    bool Starfield::isOutOfTerrain(const std::shared_ptr<StarParticle>& particle) {
        float halfWidth = spriteWidth / 2.0f;
        float halfHeight = spriteHeight / 2.0f;

        if (particle->position.x < -halfWidth)
            return true;
        else if (particle->position.x > (terrainSize.x + halfWidth))
            return true;
        else if (particle->position.y < -halfHeight)
            return true;
        else if (particle->position.y > (terrainSize.y + halfHeight))
            return true;
        else
            return false;
    }

    void Starfield::draw(shared_ptr<GpuProgram> shader) {
        vbo.empty();
        for (auto& particle : particles) {
            particle->box.centerAround(particle->position);
            vbo.addQuad(
                    particle->box.computeVertices(),
                    textCoords.computeVertices(),
                    particle->color
            );
        }

        shader->use();
        {
            // TODO: HACK! make this better
            mat4 cameraShakeMatrix(1);
            shader->setMatrix4Uniform(&cameraShakeMatrix[0][0], "cameraShakeMatrix");

            GFXSYS.setTransparencyMode(TransparencyMode::Additive);
            flareTexture->useForUnit(0);
            vbo.draw();
        }
        shader->unuse();
    }
} // namespace Acidrain
