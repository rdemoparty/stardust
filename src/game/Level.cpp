#include <Level.h>
#include <Scene.h>
#include <Camera.h>
#include <GameObjectFactory.h>
#include <GpuProgramConstants.h>
#include <Starfield.h>
#include <FileSystem.h>
#include <glm/gtc/matrix_transform.hpp>
#include <LevelScript.h>
#include <GfxSystem.h>
#include <AudioSystem.h>

namespace Acidrain {

    using namespace glm;

    static const int PLAYER_OBJECT_ID = -1;

    Level::Level() {
        gpuProgram = make_shared<GpuProgram>(
                FILESYS.getFileContent("shaders/normal.vs"),
                FILESYS.getFileContent("shaders/normal.ps")
        );

        starfield = make_shared<Starfield>(40, vec2(1024, 768));

        gameObjectFactory = make_shared<GameObjectFactory>();
        scene = make_shared<Scene>(gameObjectFactory.get(), vec2(1024, 768));

        // create camera
        camera = make_shared<Camera>();
        scene->setCamera(camera);

        // initialize some gpuProgram constants
        gpuProgramConstantBundle = make_shared<GpuProgramConstantBundle>();

        gpuProgramConstantBundle->add("orthoMatrix", GpuProgramConstant(ortho(0.0f, 1024.0f, 768.0f, 0.0f, 0.0f, 1.0f)));
        int textureSamplerIndex = 0;
        gpuProgramConstantBundle->add("diffuseSampler", GpuProgramConstant(textureSamplerIndex));
        gpuProgramConstantBundle->add("cameraShakeMatrix", GpuProgramConstant(camera->getShakeMatrix()));

        gpuProgram->addConstants(gpuProgramConstantBundle.get());

        levelScript = make_shared<LevelScript>(scene.get());

        // load background song
        song = AUDIOSYS.loadSong("main.ogg");
        song->play(-1);
    }

    Level::~Level() {
    }

    void Level::start() {
        scene->clear();
        levelScript->reset();

        // add game objects
        vec2 platformPosition = vec2(1024 / 2, 700);

        auto player = gameObjectFactory->createByName("player");
        player->setId(PLAYER_OBJECT_ID);
        player->currentState.position = vec2(1024 / 2, 800);
        scene->add(player);

        auto platformTop = gameObjectFactory->createByName("platform_top");
        platformTop->currentState.position = platformPosition;
        scene->add(platformTop, 1);

        auto platformBottom = gameObjectFactory->createByName("platform_bottom");
        platformBottom->currentState.position = platformPosition;
        scene->add(platformBottom, -1);
    }

    void Level::update(float elapsedSeconds) {
        camera->update(elapsedSeconds);
        gpuProgramConstantBundle->add("cameraShakeMatrix", GpuProgramConstant(camera->getShakeMatrix()));

        levelScript->update(elapsedSeconds);
        scene->update(elapsedSeconds);
        starfield->update(elapsedSeconds);
    }

    void Level::render(float frameAlpha) {
        starfield->draw(gpuProgram, frameAlpha);
        scene->draw(gpuProgram, frameAlpha);

        drawPlayerLife();
    }

    void Level::drawPlayerLife() {
        GameObject* player = scene->getById(PLAYER_OBJECT_ID);
        if (player != nullptr) {
            float lifePercent = player->state.life / player->state.maxLife;
            float lifeBarWidth = 200;
            float lifeBarHeight = 14;
            float x = 800;
            float y = 80;
            GFXSYS.setTransparencyMode(TransparencyMode::Additive);
            GFXSYS.drawFilledRectangle(
                    vec2(x, y),
                    vec2(x + lifeBarWidth, y + lifeBarHeight),
                    vec4(1, 1, 1, 0.2f));
            GFXSYS.drawFilledRectangle(
                    vec2(x + 3, y + 3),
                    vec2(x + (lifeBarWidth - 3)* lifePercent, y + lifeBarHeight - 3),
                    mix(vec4(0, 1, 0, 0.6), vec4(1, 0, 0, 0.6), (1.0 - lifePercent)));
//            vec4(1, 1, 1, 0.5f));
        }
    }

    bool Level::isFinished() const {
        return false;
    }

    int Level::objectsCount() const {
        return scene->countObjects();
    }
}