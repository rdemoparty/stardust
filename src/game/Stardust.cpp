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
#include "GpuProgramConstants.h"

namespace Acidrain {

    using namespace std;
    using namespace glm;

    Stardust::Stardust() {
        ANIMREPO.initialize("animations.json");

        // TODO Adrian: figure out a better name for the event system. It is too generic. Events of?
        EVENTSYS.addListener(this, SDL_QUIT);

        input = make_shared<InputProvider>();

        gpuProgram = make_shared<GpuProgram>(
                FILESYS.getFileContent("shaders/normal.vs"),
                FILESYS.getFileContent("shaders/normal.ps")
        );

        font = make_shared<Font>("fonts/Impact.ttf", 70.0f);
        fontSmall = make_shared<Font>("fonts/a.ttf", 20.0f);

        fpsCounter = make_shared<FpsCounter>();
        starfield = make_shared<Starfield>(40, vec2(1024, 768));

        GFXSYS.setClearColor(vec3(0.1f, 0.0f, 0.1f));

        // initialize sprite pool
        spritePool = make_shared<SpritePool>();

        gameObjectFactory = make_shared<GameObjectFactory>();
        scene.setGameObjectFactory(gameObjectFactory.get());

        // add game objects
        player = gameObjectFactory->createPlayer(vec2(300, 700));
        Weapon* weapon1 = new Weapon();
        player->addWeapon(weapon1, vec2(-10, -32));

        Weapon* weapon2 = new Weapon();
        player->addWeapon(weapon2, vec2(12, -32));

        scene.add(player);
        scene.add(gameObjectFactory->createEnemy(vec2(300, -64)));
        scene.add(gameObjectFactory->createExplosion(vec2(300, -64)));

        gpuProgramConstantBundle = make_shared<GpuProgramConstantBundle>();

        // initialize some gpuProgram constants
        gpuProgramConstantBundle->add("orthoMatrix", GpuProgramConstant(glm::ortho(0.0f, 1024.0f, 768.0f, 0.0f, 0.0f, 1.0f)));
        int textureSamplerIndex = 0;
        gpuProgramConstantBundle->add("diffuseSampler", GpuProgramConstant(textureSamplerIndex));

        gpuProgram->addConstants(gpuProgramConstantBundle.get());
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

        if (input->isKeyDown(SDL_SCANCODE_ESCAPE))
            quitGame = true;

        if (input->isKeyJustPressed(SDL_SCANCODE_SPACE) || input->isKeyJustPressed(SDL_SCANCODE_LCTRL))
            player->fireWeapons(true);
        else if (input->isKeyJustReleased(SDL_SCANCODE_SPACE) || input->isKeyJustReleased(SDL_SCANCODE_LCTRL))
            player->fireWeapons(false);

        for (auto& gameObject : scene.objects)
            gameObject->update(elapsedSeconds);

        scene.addNewObjectsToScene();

        for (auto& gameObject : scene.objects)
            gameObject->updateAnimation(elapsedSeconds);

        starfield->update(elapsedSeconds);
        fpsCounter->update(elapsedSeconds);

        input->copyNewStateToOldState();
    }

    void Stardust::render() {
        GFXSYS.clearScreen();

        starfield->draw(gpuProgram);

        GFXSYS.setTransparencyMode(TransparencyMode::Additive);
        spritePool->clear();
        for (auto& gameObject : scene.objects)
            gameObject->addTo(*spritePool);
        spritePool->draw(gpuProgram);

        drawStats();

        fpsCounter->addFrame();
        GFXSYS.show();
    }

    void Stardust::drawStats() {
        GFXSYS.setTransparencyMode(TransparencyMode::Transparent);
        GFXSYS.drawFilledRectangle(vec2(-1), vec2(1024, 50), vec4(0, 0, 0, 0.7f));

        std::stringstream s;
        s << "FPS: " << fpsCounter->getFps() << ", Objects: " << scene.objects.size();

        GFXSYS.setTransparencyMode(TransparencyMode::Additive);
        fontSmall->print(10, 10, s.str().c_str(), vec4(1, 1, 1, 0.9f));
    }

    bool Stardust::shouldQuit() {
        return quitGame;
    }

} // namespace Acidrain