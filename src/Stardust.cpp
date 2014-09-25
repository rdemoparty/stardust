#include <Stardust.h>
#include <FileSystem.h>
#include <GfxSystem.h>
#include <iostream>

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

        GFXSYS.setClearColor(vec3(0.1f, 0.0f, 0.1f));
    }

    Stardust::~Stardust() {
    }

    void Stardust::process(double elapsedSeconds) {
        animation->update(elapsedSeconds);
        position.x += elapsedSeconds * 100.0f;

        GFXSYS.clearScreen();
        drawSprite(animation->getSprite(), position);
    }

    bool Stardust::shouldQuit() {
        return false;
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