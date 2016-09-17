#include <GameStateCutSceneAfterLevel.h>
#include <Stardust.h>
#include <GfxSystem.h>
#include <AudioSystem.h>
#include <InputProvider.h>
#include <Font.h>
#include <GameStatePlayLevel.h>

#include <easylogging++.h>
#include <glm/ext.hpp>
#include <CutScene.h>
#include <GameSession.h>
#include <GameStateShowLevelStats.h>

namespace Acidrain {

    static shared_ptr<CutScene> cutScene;
    static shared_ptr<CutScenePlayer> cutScenePlayer;
    static shared_ptr<Font> cutSceneFont;

    GameStateCutSceneAfterLevel& GameStateCutSceneAfterLevel::instance() {
        static GameStateCutSceneAfterLevel instance;
        return instance;
    }

    void GameStateCutSceneAfterLevel::onEnter(Stardust* game) {
        GFXSYS.setClearColor(vec3(0, 0, 0));

        if (!cutSceneFont)
            cutSceneFont = shared_ptr<Font>(new Font("fonts/Neo Sans Pro Bold.ttf", 32));

        cutScene = shared_ptr<CutScene>(CutSceneRegistry::getCutScene(game->gameSession->getCurrentLevel(), CutScenePosition::AfterLevel));
        if (cutScene) {
            LOG(DEBUG) << "Playing cut scene after level " << game->gameSession->getCurrentLevel();
            cutScenePlayer = shared_ptr<CutScenePlayer>(new CutScenePlayer(cutScene, cutSceneFont));
            cutScenePlayer->start();
        }
    }

    void GameStateCutSceneAfterLevel::update(Stardust* game, float elapsedSeconds) {
        if (INPUT.isKeyJustReleased(SDL_SCANCODE_ESCAPE) && cutScenePlayer && cutScene && cutScene->skippable) {
            cutScenePlayer->skip();
        }

        if (cutScenePlayer)
            cutScenePlayer->update(elapsedSeconds);

        if (!cutScenePlayer || cutScenePlayer->isFinished()) {
            game->gameSession->levelOutroWatched();
            game->fsm->changeState(&GameStateShowLevelStats::instance());
        }
    }


    void GameStateCutSceneAfterLevel::render(Stardust* game, float alpha) {
        GFXSYS.setClearColor(vec3(0.1 * 2, 0.1 * 2, 0.12 * 2));
        GFXSYS.clearScreen();

        if (cutScenePlayer)
            cutScenePlayer->render();

        GFXSYS.show();
    }

    GameStateCutSceneAfterLevel::GameStateCutSceneAfterLevel() {
    }

} // namespace Acidrain
