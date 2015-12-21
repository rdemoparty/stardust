#include <GameStateCredits.h>
#include <GfxSystem.h>
#include <Font.h>
#include <Stardust.h>
#include <MathSupport.h>
#include <GameStateMenu.h>
#include <InputProvider.h>
#include <Fbo.h>
#include <GpuProgram.h>
#include <GpuProgramConstants.h>
#include <FileSystem.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Acidrain {

    GameStateCredits& GameStateCredits::instance() {
        static GameStateCredits instance;
        return instance;
    }

    static double scrollPosition = 0;
    static shared_ptr<Fbo> fbo;
    static shared_ptr<GpuProgram> gpuProgram;
    static shared_ptr<GpuProgramConstantBundle> gpuProgramConstantBundle;
    static float totalElapsedTime = 0;

    void GameStateCredits::onEnter(Stardust* game) {
        if (!font) {
            font = make_shared<Font>("fonts/Neo Sans Pro Bold.ttf", 32.0f);
            // pre-render text in fbos and use those for scrolling
            fbo = make_shared<Fbo>(1024, 768);
            prerender();

            gpuProgram = make_shared<GpuProgram>(
                    FILESYS.getFileContent("shaders/postprocess.vs.glsl"),
                    FILESYS.getFileContent("shaders/postprocess.ps.glsl")
            );

            gpuProgramConstantBundle = make_shared<GpuProgramConstantBundle>();

            gpuProgramConstantBundle->add("orthoMatrix",
                                          GpuProgramConstant(ortho(0.0f, 1024.0f, 768.0f, 0.0f, 0.0f, 1.0f)));
            int textureSamplerIndex = 0;
            gpuProgramConstantBundle->add("diffuseSampler", GpuProgramConstant(textureSamplerIndex));
            gpuProgramConstantBundle->add("resolution", GpuProgramConstant(vec2(GFXSYS.drawableWidth(), GFXSYS.drawableHeight())));
            gpuProgramConstantBundle->add("offset", GpuProgramConstant(vec2(GFXSYS.getOffsetX(), GFXSYS.getOffsetY())));
            gpuProgramConstantBundle->add("time", GpuProgramConstant(totalElapsedTime));
            gpuProgramConstantBundle->add("renderMode", GpuProgramConstant((int)0));
            gpuProgram->addConstants(gpuProgramConstantBundle.get());
        }


        GFXSYS.setClearColor(vec3(0, 0, 0));
        scrollPosition = 1024;
        totalElapsedTime = 0;
    }

    void GameStateCredits::onExit(Stardust* game) {
    }

    void GameStateCredits::update(Stardust* game, float elapsedSeconds) {
        if (INPUT.isKeyJustPressed(SDL_SCANCODE_ESCAPE)) {
            game->fsm->changeState(&GameStateMenu::instance());
            return;
        }

        float animationTotalTime = 5.0f;
        float animationDuration = 0.9f;
        float timeToStartExit = animationTotalTime - animationDuration;

        totalElapsedTime += elapsedSeconds;
        gpuProgramConstantBundle->add("time", GpuProgramConstant(totalElapsedTime));
        float animationCurrentTime = fmod(totalElapsedTime, animationTotalTime);


        if (animationCurrentTime <= animationDuration) {
            scrollPosition = (1.0 - easeOutCubic(animationCurrentTime / animationDuration)) * 600;
        } else if (animationCurrentTime >= timeToStartExit) {
            scrollPosition = easeInCubic((animationCurrentTime - timeToStartExit) / animationDuration) * -600;
        } else {
            scrollPosition = 0;
        }
    }

    static const char* const lines[] = {
            "               BACKFIRE         ",
            "",
            "",
            "Concept & Design: Bogdan Dragoiu",
            "Code: Adrian Scripca"
    };

    void GameStateCredits::render(Stardust* game, float renderAlpha) {
        GFXSYS.clearScreen();
        GFXSYS.setTransparencyMode(TransparencyMode::Opaque);

        // draw the post processed one
        GFXSYS.clearScreen();
        gpuProgram->use();
        fbo->getTexture()->useForUnit(0);
        glBegin(GL_QUADS);
        {
            glTexCoord2d(0, 1);
            glVertex2d(scrollPosition + 0, 0);

            glTexCoord2d(1, 1);
            glVertex2d(scrollPosition + 1024, 0);

            glTexCoord2d(1, 0);
            glVertex2d(scrollPosition + 1024, 768);

            glTexCoord2d(0, 0);
            glVertex2d(scrollPosition + 0, 768);
        }
        glEnd();
        gpuProgram->unuse();

        GFXSYS.show();
    }

    void GameStateCredits::prerender() {
        fbo->use();

        GFXSYS.clearScreen();
        GFXSYS.setTransparencyMode(TransparencyMode::Additive);

        for (int i = 0; i < 5; i++) {
            font->print(0, i*32, lines[i]);
        }
        fbo->unuse();
    }
}
