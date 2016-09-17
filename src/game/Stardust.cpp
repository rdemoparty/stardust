#include <Stardust.h>
#include <FpsCounter.h>
#include <GfxSystem.h>
#include <iostream>
#include <sstream>
#include <SpriteAnimationRepository.h>
#include <Font.h>
#include <InputProvider.h>
#include <Level.h>
#include <states/GameStateIntro.h>
#include <GameSession.h>
#include <ScriptExporterRegistry.h>
#include <GameServiceLocator.h>

namespace Acidrain {

    using namespace std;
    using namespace glm;

    Stardust::Stardust() {
        fsm = make_shared<GameStateMachine<Stardust>>(this);

        ANIMREPO.initialize("animations.json");

        // TODO Adrian: figure out a better name for the event system. It is too generic. Events of?
        EVENTSYS.addListener(this, SDL_QUIT);

        FontRenderStyle renderStyle;
        renderStyle.outlineSize = 2;
        statsFont = shared_ptr<Font>(new Font("fonts/Neo Sans Pro.ttf", 18, renderStyle));
        fpsCounter = make_shared<FpsCounter>();
        gameSession = make_shared<GameSession>();
        level = make_shared<Level>();

        GameServiceLocator::gameSession(gameSession);

        ScriptExporterRegistry::add(this);
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
        fsm->update(elapsedSeconds);
        INPUT.copyNewStateToOldState();
    }

    void Stardust::render(float alpha) {
        fsm->render(alpha);
    }

    void Stardust::drawStats() {
        GFXSYS.setTransparencyMode(TransparencyMode::Transparent);
        GFXSYS.drawFilledRectangle(vec2(0, 0), vec2(1024, 34), vec4(0.0, 0.0, 0.05, 0.8));

        stringstream s;
        s << "FPS: " << fpsCounter->getFps();

        GFXSYS.setTransparencyMode(TransparencyMode::Transparent);
        statsFont->print(10, 10, s.str(), vec4(1, 1, 1, 1), vec4(0, 0, 0, 0.7), FontPrintStyle::OUTLINE);

        s.str(std::string());
        s << "Objects in scene: " << level->objectsCount();
        statsFont->print(100, 10, s.str(), vec4(1, 1, 1, 1), vec4(0, 0, 0, 0.7), FontPrintStyle::OUTLINE);
    }

    bool Stardust::shouldQuit() {
        return quitGame;
    }

    void Stardust::exportToScript(lua_State *L) const {
        // injecting instances of scene, game session attributes
        lua_pushlightuserdata(L, level->scene.get());
        lua_setglobal(L, "SCENE");

        lua_pushlightuserdata(L, gameSession.get());
        lua_setglobal(L, "GAME_SESSION");

        lua_pushlightuserdata(L, level->camera.get());
        lua_setglobal(L, "CAMERA");

        lua_pushlightuserdata(L, nullptr);
        lua_setglobal(L, "NULL_POINTER");
    }
} // namespace Acidrain