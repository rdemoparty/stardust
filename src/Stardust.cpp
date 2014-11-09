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

        input = std::shared_ptr<InputManager>(new InputManager());

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

    void Stardust::process(float elapsedSeconds) {

        fpsCounter.frameRendered();
        fpsCounter.update(elapsedSeconds);

        if (input->isKeyJustPressed(SDL_SCANCODE_ESCAPE))
            quitGame = true;

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
        const float PLAYER_SPEED = 200.0f;

        animation->update(elapsedSeconds);
        position += velocity * PLAYER_SPEED * elapsedSeconds;

        for (int i = 0; i < npcs.size(); i++) {
            shared_ptr<DrawableEntity> e = npcs.at(i);
            e->currentSprite = animation->getSprite();
            e->position = e->position + velocities[i] * 10.0f * elapsedSeconds;
            e->update(elapsedSeconds);
        }

        entity.position = position;
//        entity.rotation = 0;
        entity.currentSprite = animation->getSprite();
        entity.size = entity.currentSprite.getSize();
        entity.rotation += 1 * elapsedSeconds;
        entity.scale = vec2(sin(entity.rotation) + 2);

        GFXSYS.clearScreen();

        starfield->update(elapsedSeconds);
        starfield->render();

        entity.update(elapsedSeconds);


        spritePool.begin();
        entity.addTo(&spritePool);
        for (auto& e : npcs) {
            e->currentSprite = animation->getSprite();
            e->addTo(&spritePool);
        }
        shader->use();

        mat4 orthoMatrix = glm::ortho(0.0f, 1024.0f, 0.0f, 768.0f, 0.0f, 1.0f);
        shader->setMatrix4Uniform(&orthoMatrix[0][0], "orthoMatrix");

        glActiveTexture(GL_TEXTURE0 + 0);
        shader->setIntUniform(0, "texture");

        spritePool.end();
        shader->unuse();

        drawFps();

//        glColor4f(1, 1, 1, 1);
//        glEnable(GL_BLEND);
//        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
////        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
//        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ZERO);
//
//        font->print(100, 700, "HELLO WORLD!");
//        fontSmall->print(100, 300, "THIS IS THE TIME OF REBELION THROUGHOUT THE GALAXY. YOU ARE ON A MISSION!");
//
//        glColor4f(1, 1, 1, 1);

        input->copyNewStateToOldState();
    }

    void Stardust::drawFps() {
        glEnable(GL_BLEND);
        glUseProgram(0);

        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

        glDisable(GL_TEXTURE_2D);

        glBegin(GL_QUADS);
        glColor4f(0, 0, 0, 0.7f);
        glVertex2d(0, 768);
        glVertex2d(1024, 768);
        glVertex2d(1024, 700);
        glVertex2d(0, 700);
        glEnd();

        glColor4f(1, 1, 1, 1);

        glEnable(GL_TEXTURE_2D);
        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
//        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ZERO);

        std::stringstream s;
        s << "FPS: " << fpsCounter.getFps();

        fontSmall->print(10, 740, s.str().c_str());
    }

    bool Stardust::shouldQuit() {
        return quitGame;
    }

    void Stardust::drawSprite(const Sprite& sprite, const vec2& position) {
        shader->unuse();

        glEnable(GL_BLEND);
        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
//        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ZERO);

        glEnable(GL_TEXTURE_2D);
        sprite.spriteSheet->texture->use();

        const SpriteInfo& spriteInfo = sprite.spriteSheet->sprites[sprite.spriteIndex];

        glBegin(GL_QUADS);
        {
            glTexCoord2f(spriteInfo.texCoords.left(), spriteInfo.texCoords.bottom());
            glVertex2f(position.x, position.y);

            glTexCoord2f(spriteInfo.texCoords.left(), spriteInfo.texCoords.top());
            glVertex2f(position.x, spriteInfo.height + position.y);

            glTexCoord2f(spriteInfo.texCoords.right(), spriteInfo.texCoords.top());
            glVertex2f(position.x + spriteInfo.width, spriteInfo.height + position.y);

            glTexCoord2f(spriteInfo.texCoords.right(), spriteInfo.texCoords.bottom());
            glVertex2f(position.x + spriteInfo.width, position.y);
        }
        glEnd();
    }
} // namespace Acidrain