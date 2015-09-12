#include <Stardust.h>
#include <FpsCounter.h>
#include <GfxSystem.h>
#include <iostream>
#include <sstream>
#include <SpriteAnimationRepository.h>
#include <Font.h>
#include <InputProvider.h>
#include <Level.h>
#include <GameStateIntro.h>
#include <GameSession.h>
#include <ScriptExporterRegistry.h>

namespace Acidrain {

    using namespace std;
    using namespace glm;

    Stardust::Stardust() {
        fsm = make_shared<GameStateMachine<Stardust>>(this);

        ANIMREPO.initialize("animations.json");

        // TODO Adrian: figure out a better name for the event system. It is too generic. Events of?
        EVENTSYS.addListener(this, SDL_QUIT);

        statsFont = make_shared<Font>("fonts/Impact.ttf", 20.0f);
        fpsCounter = make_shared<FpsCounter>();
        gameSession = make_shared<GameSession>();
        level = make_shared<Level>();

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
        GFXSYS.drawFilledRectangle(vec2(-1), vec2(1024, 50), vec4(0, 0, 0, 0.7f));

        std::stringstream s;
        s << "FPS: " << fpsCounter->getFps() << "  . Objects in scene: " << level->objectsCount();

        GFXSYS.setTransparencyMode(TransparencyMode::Additive);
        statsFont->print(10, 10, s.str().c_str(), vec4(1, 1, 1, 0.9f));
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
    }
} // namespace Acidrain