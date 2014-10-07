#include <Starfield.h>
#include <TextureGenerator.h>
#include <iostream>

namespace Acidrain {

    Starfield::Starfield(int howMany, glm::vec2 size) {
        terrainSize = size;

        spriteWidth = 48;
        spriteHeight = 48;

        flareTexture = TextureGenerator(spriteWidth, spriteHeight).lens(0, spriteWidth/2).getTexture(0);

        for (int i = 0; i < howMany; i++) {
            auto particle = std::shared_ptr<StarParticle>(new StarParticle);
            particles.push_back(particle);
            spawn(particle);
        }
    }

    Starfield::~Starfield() {
    }

    void Starfield::spawn(const std::shared_ptr<StarParticle> &particle) {
        particle->speed = rnd.randomUnitDouble() * 3.0 + 1.0;
        particle->direction = glm::vec2(0.0f, -1.0f) * particle->speed;

        if (particle->firstSpawn) {
            particle->position = glm::vec2(
                    rnd.randomUnitDouble() * terrainSize.x,
                    rnd.randomUnitDouble() * terrainSize.y
            );
            particle->firstSpawn = false;
        } else {
            particle->position = glm::vec2(
                    rnd.randomUnitDouble() * terrainSize.x,
                    terrainSize.y
            );
        }
    }

    void Starfield::update(float dt) {
        for (auto &particle : particles) {
            particle->position += particle->direction * dt * 50.0f;

            if (isOutOfTerrain(particle))
                spawn(particle);
        }
    }

    bool Starfield::isOutOfTerrain(const std::shared_ptr<StarParticle>& particle) {
        return particle->position.x < -(spriteWidth/2) ||
                particle->position.x > (terrainSize.x + spriteWidth/2) ||
                particle->position.y < -(spriteWidth/2) ||
                particle->position.y > (terrainSize.y + spriteWidth/2);
    }

    void Starfield::render() {
        glEnable(GL_BLEND);
        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ZERO);
//        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ZERO);

        glEnable(GL_TEXTURE_2D);
        flareTexture->use();

        for (auto &particle : particles)
            draw(particle);
    }

    void Starfield::draw(const std::shared_ptr<StarParticle>& particle) {
        glColor4f(1, 1, 1, particle->speed * 0.25f);

        glBegin(GL_QUADS);
        {
            glTexCoord2f(0, 1);
            glVertex2f(particle->position.x, particle->position.y);

            glTexCoord2f(0, 0);
            glVertex2f(particle->position.x, flareTexture->getHeight() + particle->position.y);

            glTexCoord2f(1, 0);
            glVertex2f(particle->position.x + flareTexture->getWidth(), flareTexture->getHeight() + particle->position.y);

            glTexCoord2f(1, 1);
            glVertex2f(particle->position.x + flareTexture->getWidth(), particle->position.y);
        }
        glEnd();
    }    
    
} // namespace Acidrain