#include <GameStateShowLevelStats.h>
#include <Stardust.h>
#include <GfxSystem.h>

#include <easylogging++.h>
#include <GameStateNextLevel.h>

namespace Acidrain {

    GameStateShowLevelStats& GameStateShowLevelStats::instance() {
        static GameStateShowLevelStats instance;
        return instance;
    }

    void GameStateShowLevelStats::onEnter(Stardust*) {
        LOG(INFO) << "Show level stats!";
    }

    void GameStateShowLevelStats::onExit(Stardust*) {
    }

    void GameStateShowLevelStats::update(Stardust* game, float elapsedSeconds) {
        game->fsm->changeState(&GameStateNextLevel::instance());
    }

    void GameStateShowLevelStats::render(Stardust* game, float alpha) {
        GFXSYS.setClearColor(vec3(0.1 * 2, 0.1 * 2, 0.12 * 2));
        GFXSYS.clearScreen();

        GFXSYS.show();
    }

    GameStateShowLevelStats::GameStateShowLevelStats() {
    }

} // namespace Acidrain
