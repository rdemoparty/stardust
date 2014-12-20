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
#include <GpuProgramConstants.h>
#include <Scene.h>
#include <Camera.h>

namespace Acidrain {

    using namespace std;
    using namespace glm;

    Stardust::Stardust() {
        ANIMREPO.initialize("animations.json");

        // TODO Adrian: figure out a better name for the event system. It is too generic. Events of?
        EVENTSYS.addListener(this, SDL_QUIT);

        gpuProgram = make_shared<GpuProgram>(
                FILESYS.getFileContent("shaders/normal.vs"),
                FILESYS.getFileContent("shaders/normal.ps")
        );

        font = make_shared<Font>("fonts/Impact.ttf", 70.0f);
        fontSmall = make_shared<Font>("fonts/Impact.ttf", 20.0f);

        fpsCounter = make_shared<FpsCounter>();
        starfield = make_shared<Starfield>(40, vec2(1024, 768));

        GFXSYS.setClearColor(vec3(0.1f, 0.0f, 0.1f));

        gameObjectFactory = make_shared<GameObjectFactory>();
        scene = make_shared<Scene>(gameObjectFactory.get(), vec2(1024, 768));

        // add game objects
        auto player = gameObjectFactory->createByName("player");
        player->currentState.position = vec2(300, 700);
        scene->add(player);

        // create camera
        camera = make_shared<Camera>();
        scene->setCamera(camera);

        // initialize some gpuProgram constants
        gpuProgramConstantBundle = make_shared<GpuProgramConstantBundle>();

        gpuProgramConstantBundle->add("orthoMatrix", GpuProgramConstant(glm::ortho(0.0f, 1024.0f, 768.0f, 0.0f, 0.0f, 1.0f)));
        int textureSamplerIndex = 0;
        gpuProgramConstantBundle->add("diffuseSampler", GpuProgramConstant(textureSamplerIndex));
        gpuProgramConstantBundle->add("cameraShakeMatrix", GpuProgramConstant(camera->getShakeMatrix()));

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

        if (INPUT.isKeyDown(SDL_SCANCODE_ESCAPE))
            quitGame = true;

        static float timeUntilNextSpawn = 0;
        timeUntilNextSpawn -= elapsedSeconds;
        if (timeUntilNextSpawn < 0) {
            GameObject* enemy = gameObjectFactory->createByName("enemy");
            enemy->currentState.position = vec2(rand() % 1024, -64);
            scene->add(enemy);
            timeUntilNextSpawn = rand() % 3 + 1;
        }

        camera->update(elapsedSeconds);
        gpuProgramConstantBundle->add("cameraShakeMatrix", GpuProgramConstant(camera->getShakeMatrix()));

        scene->update(elapsedSeconds);

        starfield->update(elapsedSeconds);
        fpsCounter->update(elapsedSeconds);

        INPUT.copyNewStateToOldState();
    }

    void Stardust::render(float alpha) {
        GFXSYS.clearScreen();

        starfield->draw(gpuProgram, alpha);
        scene->draw(gpuProgram, alpha);

        drawStats();

        fpsCounter->addFrame();
        GFXSYS.show();
    }

    void Stardust::drawStats() {
        GFXSYS.setTransparencyMode(TransparencyMode::Transparent);
        GFXSYS.drawFilledRectangle(vec2(-1), vec2(1024, 50), vec4(0, 0, 0, 0.7f));

        std::stringstream s;
        s << "FPS: " << fpsCounter->getFps() << ", Objects: " << scene->countObjects();

        GFXSYS.setTransparencyMode(TransparencyMode::Additive);
        fontSmall->print(10, 10, s.str().c_str(), vec4(1, 1, 1, 0.9f));
    }

    bool Stardust::shouldQuit() {
        return quitGame;
    }

} // namespace Acidrain