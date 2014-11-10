#include <Stardust.h>
#include <FileSystem.h>
#include <GfxSystem.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>

namespace Acidrain {

    using namespace std;
    using namespace glm;

    Randomizer rng;

    vec2 newRandomVelocity() {
        return vec2(
                rng.randomUnitDouble() - 0.5f,
                rng.randomUnitDouble() - 0.5f
        ) * 4.0f;
    }

    shared_ptr<DrawableEntity> newNpc() {
        shared_ptr<DrawableEntity> result = shared_ptr<DrawableEntity>(new DrawableEntity);
        result->position = vec2(1024.0f / 2.0f, 768.0f / 2.0f);
        result->scale = vec2(1);
        result->rotation = 0;
        result->size = vec2(64, 64);
        return result;
    }

    Stardust::Stardust() {

        EVENTSYS.addListener(this, SDL_QUIT);

        input = std::shared_ptr<InputProvider>(new InputProvider());

        shader = make_shared<Shader>(
                FILESYS.getFileContent("shaders/normal.vs"),
                FILESYS.getFileContent("shaders/normal.ps")
        );

        std::cout << "After assigning shader" << std::endl;

        spriteSheet = std::shared_ptr<SpriteSheet>(new SpriteSheet);
        spriteSheet->texture = GFXSYS.loadTexture("sprites/enemyship2b.png");
        spriteSheet->autoAdd(64, 64);

        animationData.frames = {
                {spriteSheet, 0},
                {spriteSheet, 1},
                {spriteSheet, 2},
                {spriteSheet, 3},
                {spriteSheet, 4},
                {spriteSheet, 5},
                {spriteSheet, 6},
                {spriteSheet, 7},
        };
        animationData.frameTimeInMilliseconds = 200;
        animationData.loopType = AnimationLoopType::PING_PONG;

        animation = std::shared_ptr<Animation>(new Animation(&animationData));
        animation->start();

        font = std::shared_ptr<Font>(new Font("fonts/Impact.ttf", 70.0f));
        fontSmall = std::shared_ptr<Font>(new Font("fonts/a.ttf", 20.0f));

        starfield = std::shared_ptr<Starfield>(new Starfield(40, vec2(1024, 768)));

        for (int i = 0; i < 1000; i++) {
            npcs.push_back(newNpc());
            velocities.push_back(newRandomVelocity());
        }

        GFXSYS.setClearColor(vec3(0.1f, 0.0f, 0.1f));
    }


    Stardust::~Stardust() {
    }


    void Stardust::onEvent(SDL_Event const& event) {
        switch (event.type) {
            case SDL_QUIT:
                quitGame = true;
                break;
            default:
                break;
        }
    }

    void Stardust::update(float elapsedSeconds) {
        EVENTSYS.update();

        fpsCounter.update(elapsedSeconds);

        if (input->isKeyJustPressed(SDL_SCANCODE_ESCAPE))
            quitGame = true;

        const float PLAYER_SPEED = 200.0f;
        vec2 velocity = velocityFromInput() * PLAYER_SPEED * elapsedSeconds;
        position += velocity;

        animation->update(elapsedSeconds);

        for (int i = 0; i < npcs.size(); i++) {
            shared_ptr<DrawableEntity> e = npcs.at(i);
            e->currentSprite = animation->getSprite();
            e->position = e->position + velocities[i] * 10.0f * elapsedSeconds;
            e->update(elapsedSeconds);
        }

        entity.position = position;
        entity.currentSprite = animation->getSprite();
        entity.size = entity.currentSprite.getSize();
        entity.rotation += 1 * elapsedSeconds;
        entity.scale = vec2(sin(entity.rotation) + 2);

        starfield->update(elapsedSeconds);
        entity.update(elapsedSeconds);

        input->copyNewStateToOldState();
    }

    vec2 Stardust::velocityFromInput() {
        vec2 velocity = vec2(0);

        if (input->isKeyDown(SDL_SCANCODE_LEFT) || input->isJoystickPressedLeft()) {
            velocity.x = -1;
        } else if (input->isKeyDown(SDL_SCANCODE_RIGHT) || input->isJoystickPressedRight()) {
            velocity.x = 1;
        }

        if (input->isKeyDown(SDL_SCANCODE_UP) || input->isJoystickPressedUp()) {
            velocity.y = 1;
        } else if (input->isKeyDown(SDL_SCANCODE_DOWN) || input->isJoystickPressedDown()) {
            velocity.y = -1;
        }
        return velocity;
    }

    void Stardust::render() {
        GFXSYS.clearScreen();

        shader->use();

        mat4 orthoMatrix = glm::ortho(0.0f, 1024.0f, 768.0f, 0.0f, 0.0f, 1.0f);
        shader->setMatrix4Uniform(&orthoMatrix[0][0], "orthoMatrix");

        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0 + 0);

        shader->setIntUniform(0, "texture");

        starfield->render();

        shader->unuse();

        spritePool.clear();
        entity.addTo(spritePool);
        for (auto& e : npcs) {
            e->currentSprite = animation->getSprite();
            e->addTo(spritePool);
        }

        spritePool.draw(shader);

        drawStats();

        fpsCounter.frameRendered();
        GFXSYS.show();
    }

    void Stardust::drawStats() {
        GFXSYS.setTransparencyMode(TransparencyMode::Transparent);

        GFXSYS.drawCircle(vec2(100), 20, vec4(1, 0, 0, 0.8));
        GFXSYS.drawFilledRectangle(vec2(-1), vec2(1024, 50), vec4(0, 0, 0, 0.7f));

        std::stringstream s;
        s << "FPS: " << fpsCounter.getFps();

        GFXSYS.setTransparencyMode(TransparencyMode::Additive);
        fontSmall->print(10, 10, s.str().c_str(), vec4(1, 1, 1, 0.9f));
    }

    bool Stardust::shouldQuit() {
        return quitGame;
    }

} // namespace Acidrain