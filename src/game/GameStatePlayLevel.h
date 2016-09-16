#pragma once

#include <GameState.h>
#include <memory>

namespace Acidrain {

    using namespace std;

    class Stardust;
    class Fbo;
    class GpuProgram;
    class GpuProgramConstantBundle;
    class Texture;

    class GameStatePlayLevel : public GameState<Stardust> {
    public:
        static GameStatePlayLevel &instance();

        virtual void onEnter(Stardust *game) override;
        virtual void onExit(Stardust *game) override;
        virtual void update(Stardust *game, float elapsedSeconds) override;
        virtual void render(Stardust *game, float alpha) override;

    private:
        GameStatePlayLevel();

        void handleGameEvents(const Stardust *game) const;
        float totalElapsedTime = 0;
        int renderMode = 0;
        shared_ptr<Fbo> fbo;
        shared_ptr<Texture> fboTexture;
        shared_ptr<GpuProgram> gpuProgram;
        shared_ptr<GpuProgramConstantBundle> gpuProgramConstantBundle;
    };

} // namespace Acidrain