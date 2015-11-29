#include <GameStateTest.h>
#include <Stardust.h>
#include <Dialog.h>
#include <Font.h>
#include <GfxSystem.h>
#include <AudioSystem.h>
#include <InputProvider.h>

#include <luaconf.h>
#include <lua.hpp>
#include <FileSystem.h>
#include <easylogging++.h>
#include <glm/ext.hpp>
#include <DialogRepository.h>
#include <Script.h>

namespace Acidrain {

    GameStateTest& GameStateTest::instance() {
        static GameStateTest instance;
        return instance;
    }

    static Script* script;

    void GameStateTest::onEnter(Stardust* game) {
        script = new Script("scripts/dialog.lua");

        GFXSYS.setClearColor(vec3(0, 0, 0));
        AUDIOSYS.playMusic("menu3.ogg");
    }

    void GameStateTest::onExit(Stardust* game) {
        delete script;
    }

    void GameStateTest::update(Stardust* game, float elapsedSeconds) {
        DialogRepository::getInstance().updateAll(elapsedSeconds);

        if (INPUT.isKeyJustPressed(SDL_SCANCODE_ESCAPE)) {
            game->quitGame = true;
        }
    }

    void GameStateTest::render(Stardust* game, float alpha) {
        GFXSYS.clearScreen();

        GFXSYS.setTransparencyMode(TransparencyMode::Transparent);
        DialogRepository::getInstance().renderAll();

        GFXSYS.show();
    }

    GameStateTest::GameStateTest() {
    }
} // namespace Acidrain
