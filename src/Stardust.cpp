#include <Stardust.h>
#include <FileSystem.h>
#include <GfxSystem.h>
#include <iostream>
#include <glm/glm.hpp>

namespace Acidrain {

    using namespace std;
    using namespace glm;

    struct Entity {
        vec2 position = vec2(0);
        vec2 scale = vec2(1);
        float rotation = 0;
        Animation visual;
    };

    Stardust::Stardust() {

        EVENTSYS.addListener(this, SDL_QUIT);

        input = std::shared_ptr<InputManager>(new InputManager());

        shader = shared_ptr<Shader>(new Shader(
                FILESYS.getFileContent("shaders/normal.vs"),
                FILESYS.getFileContent("shaders/normal.ps")
        ));

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

        GFXSYS.setClearColor(vec3(0.1f, 0.0f, 0.1f));
    }

    Stardust::~Stardust() {
    }


    void Stardust::onEvent(SDL_Event const &event) {
        switch (event.type) {
            case SDL_QUIT:
                quitGame = true;
                break;
            default:
                break;
        }
    }

    void Stardust::process(float elapsedSeconds) {
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

        GFXSYS.clearScreen();

        shader->unuse();
        starfield->update(elapsedSeconds);
        starfield->render();

        glColor4f(1, 1, 1, 1);
        drawSprite(animation->getSprite(), position);


        glColor4f(1, 1, 1, 1);
        glEnable(GL_BLEND);
        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
//        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ZERO);

        font->print(100, 700, "HELLO WORLD!");
        fontSmall->print(100, 300, "THIS IS THE TIME OF REBELION THROUGHOUT THE GALAXY. YOU ARE ON A MISSION!");

        glColor4f(1, 1, 1, 1);

        input->copyNewStateToOldState();
    }

    bool Stardust::shouldQuit() {
        return quitGame;
    }

    void Stardust::drawSprite(const Sprite &sprite, const vec2 &position) {
        shader->unuse();

        glEnable(GL_BLEND);
        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
//        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ZERO);

        glEnable(GL_TEXTURE_2D);
        sprite.spriteSheet->texture->use();

        const SpriteInfo &spriteInfo = sprite.spriteSheet->sprites[sprite.spriteIndex];

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