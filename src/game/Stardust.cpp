#include <Stardust.h>
#include <Starfield.h>
#include <FpsCounter.h>
#include <FileSystem.h>
#include <GfxSystem.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
#include <SpriteAnimationRepository.h>
#include <Font.h>
#include <GameObject.h>
#include <GameObjectFactory.h>
#include <InputProvider.h>

namespace Acidrain {

    using namespace std;
    using namespace glm;

    Stardust::Stardust() {
        ANIMREPO.initialize("animations.json");

        EVENTSYS.addListener(this, SDL_QUIT);

        input = std::shared_ptr<InputProvider>(new InputProvider());

        shader = make_shared<Shader>(
                FILESYS.getFileContent("shaders/normal.vs"),
                FILESYS.getFileContent("shaders/normal.ps")
        );

        font = shared_ptr<Font>(new Font("fonts/Impact.ttf", 70.0f));
        fontSmall = shared_ptr<Font>(new Font("fonts/a.ttf", 20.0f));

        fpsCounter = shared_ptr<FpsCounter>(new FpsCounter());
        starfield = shared_ptr<Starfield>(new Starfield(40, vec2(1024, 768)));

        GFXSYS.setClearColor(vec3(0.1f, 0.0f, 0.1f));

        // initialize sprite pool
        spritePool = shared_ptr<SpritePool>(new SpritePool());

        gameObjectFactory = shared_ptr<GameObjectFactory>(new GameObjectFactory());

        // add game objects
        gameObjects.push_back(gameObjectFactory->enemy(vec2(300, -64)));
        gameObjects.push_back(gameObjectFactory->enemy2(vec2(300, -64)));
        gameObjects.push_back(gameObjectFactory->player(vec2(300, 700)));
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

    void Stardust::update(float elapsedSeconds) {
        EVENTSYS.update();

        if (input->isKeyJustPressed(SDL_SCANCODE_ESCAPE))
            quitGame = true;

        for (auto &enemy : gameObjects) {
            enemy->update(elapsedSeconds);
        }

        starfield->update(elapsedSeconds);
        fpsCounter->update(elapsedSeconds);

        input->copyNewStateToOldState();
    }

    void Stardust::render() {
        GFXSYS.clearScreen();

        // draw starfield
        shader->use();
        mat4 orthoMatrix = glm::ortho(0.0f, 1024.0f, 768.0f, 0.0f, 0.0f, 1.0f);
        shader->setMatrix4Uniform(&orthoMatrix[0][0], "orthoMatrix");
        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0 + 0);
        shader->setIntUniform(0, "texture");
        starfield->render();
        shader->unuse();

        // draw entities
        spritePool->clear();
        for (auto &enemy : gameObjects) {
            enemy->addTo(*spritePool);
        }
        spritePool->draw(shader);

        drawStats();

        fpsCounter->frameRendered();
        GFXSYS.show();
    }

    void Stardust::drawStats() {
        GFXSYS.setTransparencyMode(TransparencyMode::Transparent);
        GFXSYS.drawFilledRectangle(vec2(-1), vec2(1024, 50), vec4(0, 0, 0, 0.7f));

        std::stringstream s;
        s << "FPS: " << fpsCounter->getFps();

        GFXSYS.setTransparencyMode(TransparencyMode::Additive);
        fontSmall->print(10, 10, s.str().c_str(), vec4(1, 1, 1, 0.9f));
    }

    bool Stardust::shouldQuit() {
        return quitGame;
    }

} // namespace Acidrain