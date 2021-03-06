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
#include <SDL_image.h>
#include <GpuProgram.h>
#include <GpuProgramConstants.h>
#include <Vbo.h>
#include <FpsCounter.h>
#include <TextLayout.h>
#include <CutScene.h>


namespace Acidrain {

    static Script* script;
    static shared_ptr<GpuProgram> gpuProgram;
    static shared_ptr<GpuProgramConstantBundle> gpuProgramConstantBundle;
    static shared_ptr<CutScene> cutScene;
    static shared_ptr<CutScenePlayer> cutScenePlayer;


    GameStateTest& GameStateTest::instance() {
        static GameStateTest instance;
        return instance;
    }


    shared_ptr<Font> newFont;
    shared_ptr<Font> bigFont;
    FpsCounter fpsCounter;
    shared_ptr<TextLayout> textLayout;

    void GameStateTest::onEnter(Stardust* game) {
        FontRenderStyle renderStyle;
        renderStyle.outlineSize = 2;
        renderStyle.shadowOffsetX = 3;
        renderStyle.shadowOffsetY = 3;

        bigFont = shared_ptr<Font>(new Font("fonts/arial.ttf", 90, renderStyle));
        newFont = shared_ptr<Font>(new Font("fonts/Neo Sans Pro Bold.ttf", 90, renderStyle));

        string textToRender = "B A C K F I R E";
        Box constraint(200, 300, 1024, 50);
        textLayout = shared_ptr<TextLayout>(new TextLayout(textToRender, constraint, newFont.get()));
        // ------------------------------------

        script = new Script("scripts/dialog.lua");

        GFXSYS.setClearColor(vec3(0, 0, 0));
        AUDIOSYS.playMusic("menu3.ogg");

        // initialize shader
        gpuProgram = make_shared<GpuProgram>(
                FILESYS.getFileContent("shaders/test.vs.glsl"),
                FILESYS.getFileContent("shaders/test.ps.glsl")
        );

        gpuProgramConstantBundle = make_shared<GpuProgramConstantBundle>();

        gpuProgramConstantBundle->set("orthoMatrix",
                                      GpuProgramConstant(ortho(0.0f, 1024.0f * TEXT_DOWNSAMPLE_FACTOR, 768.0f * TEXT_DOWNSAMPLE_FACTOR, 0.0f, 0.0f, 1.0f)));

//        glViewport(0, 0, GFXSYS.windowWidth(), GFXSYS.windowHeight());
//        gpuProgramConstantBundle->add("orthoMatrix",
//                                      GpuProgramConstant(ortho(0.0f, (float)GFXSYS.windowWidth(), (float)GFXSYS.windowHeight(), 0.0f, 0.0f, 1.0f)));

        int textureSamplerIndex = 0;
        gpuProgramConstantBundle->set("diffuseSampler", GpuProgramConstant(textureSamplerIndex));
        gpuProgramConstantBundle->set("cameraShakeMatrix", GpuProgramConstant(glm::mat4()));
        gpuProgramConstantBundle->set("textDiffuseColor", GpuProgramConstant(vec4(0.5, 0.5, 0.6, 0.9)));
        gpuProgramConstantBundle->set("textOutlineColor", GpuProgramConstant(vec4(0.0, 0.0, 0.0, 0.4)));
        gpuProgramConstantBundle->set("textShadowColor", GpuProgramConstant(vec4(0.0, 0.0, 0.0, 1)));

        gpuProgram->addConstants(gpuProgramConstantBundle.get());



        // trigger the creation of a cutscene
        new Script("cutscenes/test.cutscene.lua");

        shared_ptr<Font> cutSceneFont = shared_ptr<Font>(new Font("fonts/Neo Sans Pro Bold.ttf", 32));
        cutScene = shared_ptr<CutScene>(CutSceneRegistry::getCutScene(1, CutScenePosition::BeforeLevel));
        if (cutScene) {
            cutScenePlayer = shared_ptr<CutScenePlayer>(new CutScenePlayer(cutScene, cutSceneFont));
            cutScenePlayer->start();
        }


//        exit(0);


//        cutScene = shared_ptr<CutScene>(new CutScene);
//        cutScene->musicUri = "main.ogg";
//        cutScene->speechUri = "sounds/rain/acquiring_modules_change_the_behaviour_of_the_ship.ogg";
//
//        Slide* slide = new Slide(GFXSYS.loadTexture("sprites/splash1.png"), 8);
//        slide->setFadeSeconds(2, 2);
//
//        slide->addCaption(
//                Caption::create("This is the caption\nfor the first slide")
//                        ->atPosition(vec2(100, 400))
//                        ->withColor(vec4(1, 0, 0, 1))
//                        ->withDelayFromSlideStart(2)
//                        ->activeForSeconds(3)
//                        ->withFading(1, 1)
//        );
//        slide->addCaption(
//                Caption::create("Some extra text")
//                        ->atPosition(vec2(100, 500))
//                        ->withColor(vec4(1, 1, 1, 1))
//                        ->withDelayFromSlideStart(4)
//                        ->activeForSeconds(3)
//                        ->withFading(1, 1)
//        );
//
//        cutScene->addSlide(slide);
//
//        Slide* slide2 = new Slide(GFXSYS.loadTexture("sprites/splash2.png"), 10);
//        slide2->setFadeSeconds(2, 2);
//        slide2->addCaption("This is the caption\nfor the SECOND SLIDE !!11", vec2(100, 500));
//        cutScene->addSlide(slide2);
//
//        FontRenderStyle fontRenderStyle;
//        fontRenderStyle.outlineSize = 2;
//        fontRenderStyle.shadowOffsetX = -2;
//        fontRenderStyle.shadowOffsetY = -2;
//        cutScenePlayer = shared_ptr<CutScenePlayer>(new CutScenePlayer(cutScene,
//                                                                       shared_ptr<Font>(new Font("fonts/Neo Sans Pro Bold.ttf", 32, fontRenderStyle))));
//        cutScenePlayer->start();
    }

    void GameStateTest::onExit(Stardust* game) {
        delete script;
    }

    float textAlpha = 0;

    void GameStateTest::update(Stardust* game, float elapsedSeconds) {
//        DialogRepository::getInstance().updateAll(elapsedSeconds);

        fpsCounter.update(elapsedSeconds);
        textAlpha += elapsedSeconds;

        if (INPUT.isKeyJustPressed(SDL_SCANCODE_ESCAPE)) {
            game->quitGame = true;
        }

        if (cutScenePlayer)
            cutScenePlayer->update(elapsedSeconds);
    }


    void GameStateTest::render(Stardust* game, float alpha) {
        GFXSYS.setClearColor(vec3(0.1 * 2, 0.1 * 2, 0.12 * 2));
        GFXSYS.clearScreen();

        if (cutScenePlayer)
            cutScenePlayer->render();

        GFXSYS.show();
        fpsCounter.addFrame();

        return;

//        DialogRepository::getInstance().renderAll();

        GFXSYS.setTransparencyMode(TransparencyMode::Transparent);

        gpuProgramConstantBundle->set("textDiffuseColor", GpuProgramConstant(vec4(1.0, 0.5, 0.2, 1)));
        gpuProgram->use();
        newFont->texture()->useForUnit(0);

//        const char* text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.\nVivamus nunc mauris, feugiat at tortor quis, ullamcorper porta tellus.\nPhasellus sit amet eleifend orci.";
//        newFont->print(100, 200, text, vec4(1, 0.5, 0.2, 1));
//        newFont->print(100, 200, text, vec4(0.5, 0.5, 0.6, (sin(textAlpha) / 2.0) + 0.5));
//        newFont->print(100, 200, text, vec4(0.5, 0.5, 0.6, 1));

        stringstream ss;
        ss << "FPS: " << fpsCounter.getFps();
        bigFont->texture()->useForUnit(0);
        bigFont->print(0, 100, ss.str().c_str(), vec4(1.0, 0.5, 0.2, 1));

        gpuProgram->unuse();
//        GFXSYS.drawFilledRectangle(textLayout->getTextBox().tl, textLayout->getTextBox().tl, vec4(0, 0, 0, 0.5));
        GFXSYS.drawFilledRectangle(textLayout->getTextBox().tl, textLayout->getTextBox().br, vec4(0, 0, 0, 0.2));

        gpuProgramConstantBundle->set("textDiffuseColor", GpuProgramConstant(vec4(1, 1, 1, 1)));
//        gpuProgramConstantBundle->add("textDiffuseColor", GpuProgramConstant(vec4(1)));
        gpuProgramConstantBundle->set("textOutlineColor", GpuProgramConstant(vec4(0, 0, 0, 0.2)));

        gpuProgram->use();
        newFont->texture()->useForUnit(0);
        textLayout->render();

//        GFXSYS.renderFullScreenTexturedQuad();
        gpuProgram->unuse();

        GFXSYS.show();
        fpsCounter.addFrame();
    }

    GameStateTest::GameStateTest() {
    }
} // namespace Acidrain
