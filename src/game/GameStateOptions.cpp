#include <GameStateOptions.h>
#include <Stardust.h>
#include <Font.h>
#include <GfxSystem.h>
#include <InputProvider.h>
#include <GameStateMenu.h>
#include <Menu.h>

namespace Acidrain {

    GameStateOptions& GameStateOptions::instance() {
        static GameStateOptions instance;
        return instance;
    }

    GameStateOptions::GameStateOptions() {
        shared_ptr<MenuConfig> menuConfig = shared_ptr<MenuConfig>(new MenuConfig(
                make_shared<Font>("fonts/Neo Sans Pro Bold.ttf", 40.0f),
                make_shared<Font>("fonts/Neo Sans Pro Bold.ttf", 10.0f)
        ));
        menu = make_shared<Menu>(menuConfig);
        setupMenu();
    }

    void GameStateOptions::setupMenu() const {
        MenuItem first;
        first.title = "Resolution";
        first.id = 1;

        MenuItem second;
        second.title = "Fullscreen";
        second.id = 2;

        menu->addItem(first);
        menu->addItem(second);
    }

    void GameStateOptions::onEnter(Stardust* game) {
        GFXSYS.setClearColor(vec3(0, 0, 0));
        menu->reset();
    }

    void GameStateOptions::onExit(Stardust* game) {
    }

    void GameStateOptions::update(Stardust* game, float elapsedSeconds) {
        if (INPUT.isKeyJustPressed(SDL_SCANCODE_ESCAPE)) {
            game->fsm->changeState(&GameStateMenu::instance());
            return;
        }
        menu->update(elapsedSeconds);
    }

    void GameStateOptions::render(Stardust* game, float alpha) {
        GFXSYS.clearScreen();
        menu->render(alpha);
        GFXSYS.show();
    }
} // namespace Acidrain
